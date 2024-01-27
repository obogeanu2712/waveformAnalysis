#include "eventProto.capnp.h"
#include "functions.hpp"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <nlohmann/json.hpp>
#include <TGraph.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLatex.h>
#include <TLine.h>
#include <TH1I.h>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>
#include <fcntl.h>
#include <fstream>
#include <random>
#include <cstdint>


using namespace std;
using json = nlohmann::json;

// ofstream dataOut("data.txt");

Event::Event(uint8_t board, uint8_t channel, uint16_t energy, shared_ptr<vector<int16_t>> waveform) : board(channel), channel(channel), fileEnergy(energy), waveform(waveform) {}

shared_ptr<vector<Event>> readEvents(string configFileName)
{
    shared_ptr<vector<Event>> eventsPtr = make_shared<vector<Event>>();

    ifstream jsonConfigFile(configFileName);
    json jsonConfig;
    jsonConfigFile >> jsonConfig;
    string fileName = jsonConfig["fileName"];
    vector<vector<int>> counts = jsonConfig["detectors"].get<vector<vector<int>>>();

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
                                           { return abs(element) > threshold; });
    if (it == values->end())
        return 0;
    return distance(values->begin(), it);
}

shared_ptr<vector<int16_t>> delayWithGaussian(const shared_ptr<vector<int16_t>> &values, int16_t delay)
{

    shared_ptr<vector<int16_t>> delayed = make_shared<vector<int16_t>>(*values);

    // mean on delay

    double_t mean = static_cast<double_t>(accumulate(delayed->begin(), delayed->begin() + delay, 0)) / delayed->size();

    // standard deviation on delay
    double_t stdDeviation1 = sqrt(accumulate(delayed->begin(), delayed->begin() + delay, 0,
                                             [mean](double_t acc, int16_t x)
                                             {
                                                 return acc + (x - mean) * (x - mean);
                                             }) /
                                  delay);

    // normal distribution instance

    normal_distribution<double_t> distribution1(mean, stdDeviation1);

    // rotate vector with 'delay' samples

    rotate(delayed->rbegin(), delayed->rbegin() + delay, delayed->rend());

    // populate shifted samples with the normal distribution

    random_device rd;

    default_random_engine generator(rd());

    // generate random values

    transform(delayed->begin(), delayed->begin() + delay, delayed->begin(),
              [&distribution1, &generator](int16_t)
              {
                  return static_cast<int16_t>(distribution1(generator));
              });

    return delayed;
}

shared_ptr<vector<int16_t>> attenuate(const shared_ptr<vector<int16_t>> &values, double_t attenuation)
{

    shared_ptr<vector<int16_t>> attenuated = make_shared<vector<int16_t>>();
    attenuated->reserve(values->size());
    transform(values->begin(), values->end(), back_inserter(*attenuated), [attenuation](int16_t element){
        return static_cast<int16_t>(attenuation * element);
    });
    return attenuated;
}

shared_ptr<vector<int16_t>> sumSignals(const shared_ptr<vector<int16_t>> &values1, const shared_ptr<vector<int16_t>> &values2)
{
    shared_ptr<vector<int16_t>> sum = make_shared<vector<int16_t>>();

    sum->reserve(values1->size());

    transform(values1->begin(), values1->end(), values2->begin(), sum->begin(), plus<int16_t>());

    return sum;
}

int16_t CFD(const shared_ptr<vector<int16_t>> &values, double_t attenuation, int16_t delay)
{
    shared_ptr<vector<int16_t>> delayed = delayWithGaussian(values, delay);

    shared_ptr<vector<int16_t>> flippedAttenuated = reverseWaveform(attenuate(values, attenuation));

    shared_ptr<vector<int16_t>> sum = sumSignals(delayed, flippedAttenuated);

    return 0;
}

bool saturated(const shared_ptr<vector<int16_t>> &values, int16_t gate)
{
    vector<int16_t>::iterator max = max_element(values->begin(), values->end());
    vector<int16_t>::iterator it = find_if(max, max + gate, [max](int16_t element)
                                           { return (element != *max); });

    return it == max + gate;
}

bool pileup(const shared_ptr<vector<int16_t>> &values, float amplitudeFraction, int16_t threshold, int16_t gateLength)
{
    vector<int16_t>::iterator aboveThreshold = find_if(values->begin(), values->end(), [threshold](int16_t element)
                                                       { return element > threshold; });
    if (aboveThreshold == values->end())
        return false;
    vector<int16_t>::iterator underThreshold = find_if(aboveThreshold, values->end(), [threshold](int16_t element)
                                                       { return element < threshold; });
    if (underThreshold == values->end())
        return false;

    vector<int16_t>::iterator max = max_element(aboveThreshold, underThreshold);

    vector<int16_t>::iterator gateEnd;

    if (underThreshold + gateLength > values->end())
        gateEnd = values->end();
    else
    {
        gateEnd = underThreshold + gateLength;
    }

    vector<int16_t>::iterator secondPeak = find_if(underThreshold, gateEnd, [amplitudeFraction, max](int16_t element)
                                                   { return element > *max * amplitudeFraction; });
    return secondPeak != gateEnd;
}

int16_t energyExtractionMax(const shared_ptr<vector<int16_t>> &values)
{ // implemented for positive signals with extracted baseline
    vector<int16_t>::iterator max = max_element(values->begin(), values->end());
    return *max;
}

int16_t energyExtractionGate(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate)
{

    int16_t pulseBegin, pulseEnd;
    int16_t leadingEdge = leadingEdgeDiscrimination(values, threshold);
    if (leadingEdge > preGate)
    {
        pulseBegin = leadingEdge - preGate;
    }
    else
    {
        pulseBegin = 0;
    }
    if (pulseBegin + gateLength < values->size())
    {
        pulseEnd = pulseBegin + gateLength;
    }
    else
    {
        pulseEnd = values->size() - 1;
    }
    return accumulate(values->begin() + pulseBegin, values->begin() + pulseEnd, 0) / gateLength;
}

pair<int16_t, bool> EnergyGatePileup(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate, float amplitudeFraction)
{
    pair<int16_t, bool> result;
    result.first = energyExtractionGate(values, threshold, gateLength, preGate);
    result.second = pileup(values, amplitudeFraction, threshold, gateLength);
    // i could choose not to store the energy if i detect pileup
    return result;
}

shared_ptr<vector<Event>> processEvents(const shared_ptr<vector<Event>> &rawEvents, string configFileName)
{

    ifstream jsonConfigFile(configFileName);
    json jsonConfig;
    jsonConfigFile >> jsonConfig;

    int16_t noiseSamples = jsonConfig["noiseSamples"];
    int16_t threshold = jsonConfig["threshold"];
    int16_t gateLength = jsonConfig["gateLength"];
    int16_t preGate = jsonConfig["preGate"];
    int16_t saturationGate = jsonConfig["saturationGate"];
    float amplitudeFraction = jsonConfig["amplitudeFraction"];

    shared_ptr<vector<Event>> processedEvents = make_shared<vector<Event>>();
    processedEvents->reserve(rawEvents->size());
    transform(rawEvents->begin(), rawEvents->end(), back_inserter(*processedEvents),
              [noiseSamples, threshold, gateLength, preGate, saturationGate, amplitudeFraction](const Event &event)
              { 
        Event processedEvent(event);
        processedEvent.waveform = reverseWaveform(subtractBackground(processedEvent.waveform, noiseSamples));
        processedEvent.energyMax = energyExtractionMax(processedEvent.waveform);
        processedEvent.energyGate = energyExtractionGate(processedEvent.waveform, threshold, gateLength, preGate);
        processedEvent.saturated = saturated(processedEvent.waveform, saturationGate);
        processedEvent.thresholdIndex = leadingEdgeDiscrimination(processedEvent.waveform, threshold);
        processedEvent.pileup = pileup(processedEvent.waveform, amplitudeFraction, threshold, gateLength);
        return processedEvent; });
    return processedEvents;
}

shared_ptr<TGraph> drawWaveform(const shared_ptr<vector<int16_t>> &values, uint8_t board, uint8_t channel)
{
    shared_ptr<TGraph> graph(new TGraph(values->size()));
    graph->SetTitle(Form("Board %d Channel %d", board, channel));
    for (int pointIndex = 0; pointIndex < values->size(); pointIndex++)
    {
        graph->SetPoint(pointIndex, pointIndex, (*values)[pointIndex]);
    }
    return graph;
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

void drawEvent(const Event &event, const json &jsonConfig)
{
    shared_ptr<TGraph> graph = drawWaveform(event.waveform, event.board, event.channel);
    graph->Draw("APL");

    int16_t gateLength = jsonConfig["gateLength"];

    int16_t delay = jsonConfig["delay"];

    double_t attenuation = jsonConfig["attenuation"];


    int16_t thresholdX = event.thresholdIndex;
    int16_t thresholdY = (*(event.waveform))[event.thresholdIndex];

    shared_ptr<TLine> verticalLine(new TLine(thresholdX, graph->GetYaxis()->GetXmin(), thresholdX, graph->GetYaxis()->GetXmax()));
    shared_ptr<TLine> horizontalLine(new TLine(graph->GetXaxis()->GetXmin(), thresholdY, graph->GetXaxis()->GetXmax(), thresholdY));
    shared_ptr<TLine> gateVerticalLine(new TLine(thresholdX + gateLength, graph->GetYaxis()->GetXmin(), thresholdX + gateLength, graph->GetYaxis()->GetXmax()));
    verticalLine->SetLineStyle(2);
    verticalLine->Draw();
    horizontalLine->SetLineStyle(2);
    horizontalLine->Draw();
    gateVerticalLine->SetLineStyle(2);
    gateVerticalLine->Draw();
    Double_t YTextCoordinate = 0.7;
    shared_ptr<TText> text(new TText(0.7, YTextCoordinate -= 0.05, Form("File Energy: %d", event.fileEnergy)));
    text->SetNDC();
    text->Draw();

    shared_ptr<TText> text1(new TText(0.7, YTextCoordinate -= 0.05, Form("Max Energy: %d", event.energyMax)));
    text1->SetNDC();
    text1->Draw();

    shared_ptr<TText> text2(new TText(0.7, YTextCoordinate -= 0.05, Form("Gate Energy: %d", event.energyGate)));
    text2->SetNDC();
    text2->Draw();

    shared_ptr<TText> text3;

    if (event.saturated)
    {
        text3 = make_shared<TText>(0.7, YTextCoordinate -= 0.05, "Saturated");
        text3->SetNDC();
        text3->Draw();
    }

    shared_ptr<TText> text4;

    if (event.pileup)
    {
        text4 = make_shared<TText>(0.7, YTextCoordinate -= 0.05, "Pile up");
        text4->SetNDC();
        text4->Draw();
    }
    // // verify delay function
    // shared_ptr<TGraph> graph1 = drawWaveform(delayWithGaussian(event.waveform, delay), event.board, event.channel);
    // graph1->Draw("PL");

    //verify attenuation function
    shared_ptr<TGraph> graph1 = drawWaveform(delayWithGaussian(attenuate(event.waveform, attenuation), delay), event.board, event.channel);
    graph1->Draw("PL");

    gPad->Update();
    gPad->WaitPrimitive("ggg");
    gPad->Clear();
}

void drawEvents(const shared_ptr<vector<Event>> &events, string configFileName)
{

    ifstream jsonFile(configFileName);
    json jsonConfig;
    jsonFile >> jsonConfig;

    for (const Event &event : *events)
    {
        drawEvent(event, jsonConfig);
    }
}