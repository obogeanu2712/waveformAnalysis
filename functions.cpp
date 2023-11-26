#include "functions.hpp"
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

#include <iostream>

using namespace std;

shared_ptr<waveform_t> subtractBackground(const shared_ptr<waveform_t> &values, int16_t noiseSamples)
{
    shared_ptr<waveform_t> result = make_shared<waveform_t>();
    result->reserve(values->size());
    int16_t meanBackground = (accumulate(values->begin(), values->begin() + noiseSamples, 0)) / noiseSamples;
    transform(values->begin(), values->end(),
              back_inserter(*result), [meanBackground](int16_t element)
              { return element - meanBackground; });

    return result;
}

shared_ptr<vector<shared_ptr<waveform_t>>> subtractBackground(const shared_ptr<vector<shared_ptr<waveform_t>>> &waveforms, int16_t noiseSamples)
{
    shared_ptr<vector<shared_ptr<waveform_t>>> result = make_shared<vector<shared_ptr<waveform_t>>>();
    for (vector<shared_ptr<waveform_t>>::iterator it = waveforms->begin(); it != waveforms->end(); it++)
    {
        result->push_back(subtractBackground(*it, noiseSamples));
    }
    return result;
}

shared_ptr<waveformsMap_t> subtractBackground(shared_ptr<waveformsMap_t> &waveformsMap, int16_t noiseSamples)
{
    shared_ptr<waveformsMap_t> result = make_shared<waveformsMap_t>();
    for (waveformsMap_t::iterator it = waveformsMap->begin(); it != waveformsMap->end(); it++)
    {
        (*result)[it->first] = subtractBackground(it->second, noiseSamples);
    }
    return result;
}

shared_ptr<waveform_t> reverseWaveform(const shared_ptr<waveform_t> &values)
{

    shared_ptr<waveform_t> result = make_shared<waveform_t>();
    result->reserve(values->size());
    transform(values->begin(), values->end(),
              back_inserter(*result), [](int16_t element)
              { return -element; });

    return result;
}

shared_ptr<waveformsMap_t> reverseWaveform(const shared_ptr<waveformsMap_t> &waveforms)
{
    shared_ptr<waveformsMap_t> result = make_shared<waveformsMap_t>();
    for (waveformsMap_t::iterator it = waveforms->begin(); it != waveforms->end(); it++)
    {
        if (!((*result)[it->first]))
            (*result)[it->first] = shared_ptr<vector<shared_ptr<waveform_t>>>();
        for (vector<shared_ptr<waveform_t>>::iterator it1 = it->second->begin(); it1 != it->second->end(); it1++)
        {
            (*result)[it->first]->push_back(reverseWaveform(*it1));
        }
    }
    return result;
}

int16_t leadingEdgeDiscrimination(const shared_ptr<waveform_t> &values, int16_t threshold)
{
    waveform_t::iterator it = find_if(values->begin(), values->end(), [threshold](int16_t element)
                                      { return element > threshold; });
    if (it == values->end())
        return 0;
    return distance(values->begin(), it);
}
int16_t energyExtractionMax(const shared_ptr<waveform_t> &values)
{ // implemented for positive signals with extracted baseline
    waveform_t::iterator max = max_element(values->begin(), values->end());
    return *max;
}

shared_ptr<energies_t> energyExtractionMax(const shared_ptr<vector<shared_ptr<waveform_t>>> &waveforms)
{
    shared_ptr<energies_t> energies = make_shared<energies_t>();
    for (vector<shared_ptr<waveform_t>>::iterator it = waveforms->begin(); it != waveforms->end(); it++)
    {
        energies->push_back(energyExtractionMax(*it));
    }
    return energies;
}

shared_ptr<energyMap_t> energyExtractionMax(const shared_ptr<waveformsMap_t> &waveformsMap)
{
    shared_ptr<energyMap_t> result = make_shared<energyMap_t>();
    for (waveformsMap_t::iterator it = waveformsMap->begin(); it != waveformsMap->end(); it++)
    {
        (*result)[it->first] = energyExtractionMax(it->second);
    }
    return result;
}
int16_t energyExtractionGate(const shared_ptr<waveform_t> &values, int16_t threshold, int16_t gateLength, int16_t preGate)
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

void drawWaveform(const shared_ptr<waveform_t> &values, uint8_t board, uint8_t channel, int index)
{
    TGraph *graph = new TGraph(values->size());
    graph->SetTitle(Form("Board %d Channel %d Index %d", board, channel, index));
    for (int pointIndex = 0; pointIndex < values->size(); pointIndex++)
    {
        graph->SetPoint(pointIndex, pointIndex, (*values)[pointIndex]);
    }

    graph->Draw("APL");
    gPad->Update();
    gPad->WaitPrimitive("ggg");

    delete graph;
}

void drawTwoWaveforms(const shared_ptr<waveform_t> &values1, const shared_ptr<waveform_t> &values2)
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

void drawHistogram(const shared_ptr<energies_t> energies, uint8_t board, uint8_t channel)
{
    shared_ptr<TH1I> histogram = make_shared<TH1I>("histogram", Form("Board %d Channel %d", board, channel), 1000, 0, 0);
    for (energies_t::iterator it = energies->begin(); it != energies->end(); it++)
    {
        histogram->Fill(*it);
    }
    histogram->Draw();
    gPad->Update();
    gPad->WaitPrimitive("ggg");
}

void drawHistogram(const shared_ptr<energyMap_t> &energyMap)
{
    for (energyMap_t::iterator it = energyMap->begin(); it != energyMap->end(); it++)
    {
        drawHistogram(it->second, it->first.first, it->first.second);
    }
}