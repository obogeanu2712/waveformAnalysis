#include "eventProto.capnp.h"
#include "functions.hpp"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <nlohmann/json.hpp>
#include <TGraph.h>
#include <TCanvas.h>
#include <TString.h>
#include <TText.h>
#include <TLine.h>
#include <TH1I.h>
#include <numeric>
#include <algorithm>
#include <memory>
#include <map>
#include <fcntl.h>
#include <fstream>

using namespace std;
using json = nlohmann::json;

Event::Event(uint8_t board, uint8_t channel, uint16_t energy, shared_ptr<vector<int16_t>> waveform) : board(channel), channel(channel), energy(energy), waveform(waveform) {}

shared_ptr<vector<Event>> readEvents(string fileName, string configFileName)
{
    shared_ptr<vector<Event>> eventsPtr = make_shared<vector<Event>>();

    ifstream jsonConfigFile(configFileName);
    json jsonConfig;
    jsonConfigFile >> jsonConfig;
    std::vector<std::vector<int>> counts = jsonConfig["detectors"].get<vector<vector<int>>>();

    int fd = open(fileName.c_str(), O_RDONLY);

    kj::FdInputStream inputStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(inputStream);

    while (bufferedStream.tryGetReadBuffer() != nullptr)
    {
        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});
        WaveData::Reader wavedata = message.getRoot<WaveData>();

        auto waveEvents = wavedata.getEvents();

        // Iterate through the events
        for (auto event : waveEvents)
        {

            // // Access event fields and extract necessary waveforms
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();

            // Check if there are any other waveforms to store and store them
            if (counts[board][channel])
            {
                // Read the data for the event of interest

                uint16_t energy = event.getEnergy();
                auto waveformReader = event.getWaveform1();

                shared_ptr<vector<int16_t>> waveform = make_shared<vector<int16_t>>();
                for (const int16_t &value : waveformReader)
                {
                    waveform->push_back(value);
                }
                Event eventObject = Event(board, channel, energy, waveform);
                eventsPtr->push_back(eventObject);
            }
        }
    }
    return eventsPtr;
}

shared_ptr<vector<int16_t>> subtractBackground(const shared_ptr<vector<int16_t>> &values, int16_t noiseSamples)
{
    shared_ptr<vector<int16_t>> result = make_shared<vector<int16_t>>();
    result->reserve(values->size());
    int16_t meanBackground = (accumulate(values->begin(), values->begin() + noiseSamples, 0)) / noiseSamples;
    transform(values->begin(), values->end(),
              back_inserter(*result), [meanBackground](int16_t element)
              { return element - meanBackground; });

    return result;
}

shared_ptr<vector<int16_t>> reverseWaveform(const shared_ptr<vector<int16_t>> &values)
{

    shared_ptr<vector<int16_t>> result = make_shared<vector<int16_t>>();
    result->reserve(values->size());
    transform(values->begin(), values->end(),
              back_inserter(*result), [](int16_t element)
              { return -element; });

    return result;
}

int16_t leadingEdgeDiscrimination(const shared_ptr<vector<int16_t>> &values, int16_t threshold)
{
    vector<int16_t>::iterator it = find_if(values->begin(), values->end(), [threshold](int16_t element)
                                           { return element > threshold; });
    if (it == values->end())
        return 0;
    return distance(values->begin(), it);
}
int16_t energyExtractionMax(const shared_ptr<vector<int16_t>> &values)
{ // implemented for positive signals with extracted baseline
    vector<int16_t>::iterator max = max_element(values->begin(), values->end());
    return *max;
}

shared_ptr<vector<int16_t>> energyExtractionMax(const shared_ptr<vector<shared_ptr<vector<int16_t>>>> &waveforms)
{
    shared_ptr<vector<int16_t>> energies = make_shared<vector<int16_t>>();
    for (vector<shared_ptr<vector<int16_t>>>::iterator it = waveforms->begin(); it != waveforms->end(); it++)
    {
        energies->push_back(energyExtractionMax(*it));
    }
    return energies;
}
int16_t energyExtractionGate(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate)
{
    int16_t pulseBegin;
    if (leadingEdgeDiscrimination(values, threshold) > preGate)
    {
        pulseBegin = leadingEdgeDiscrimination(values, threshold) - preGate;
    }
    else
    {
        pulseBegin = 0;
    }
    int16_t pulseEnd = pulseBegin + gateLength;
    return accumulate(values->begin() + pulseBegin, values->end() + pulseEnd, 0) / gateLength;
}

void drawWaveform(const shared_ptr<vector<int16_t>> &values, uint8_t board, uint8_t channel)
{
    TGraph *graph = new TGraph(values->size());
    graph->SetTitle(Form("Board %d Channel %d", board, channel));
    for (int pointIndex = 0; pointIndex < values->size(); pointIndex++)
    {
        graph->SetPoint(pointIndex, pointIndex, (*values)[pointIndex]);
    }

    graph->Draw("APL");
    gPad->Update();
    gPad->WaitPrimitive("ggg");

    delete graph;
}

void drawTwoWaveforms(const shared_ptr<vector<int16_t>> &values1, const shared_ptr<vector<int16_t>> &values2)
{
    TGraph *graph1, *graph2;
    graph1 = new TGraph(values1->size());
    graph2 = new TGraph(values2->size());

    for (int pointIndex = 0; pointIndex < values1->size(); pointIndex++)
    {
        graph1->SetPoint(pointIndex, pointIndex, (*values1)[pointIndex]);
    }

    for (int pointIndex = 0; pointIndex < values2->size(); pointIndex++)
    {
        graph2->SetPoint(pointIndex, pointIndex, (*values2)[pointIndex]);
    }

    graph1->Draw("APL");
    graph2->Draw("PL");
    gPad->Update();
    gPad->WaitPrimitive("ggg");

    delete graph1;
    delete graph2;
}

void drawHistogram(const shared_ptr<vector<int16_t>> energies, uint8_t board, uint8_t channel)
{
    shared_ptr<TH1I> histogram = make_shared<TH1I>("histogram", Form("Board %d Channel %d", board, channel), 1000, 0, 0);
    for (vector<int16_t>::iterator it = energies->begin(); it != energies->end(); it++)
    {
        histogram->Fill(*it);
    }
    histogram->Draw();
    gPad->Update();
    gPad->WaitPrimitive("ggg");
}