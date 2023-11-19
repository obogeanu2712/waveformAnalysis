#include "functions.hpp"
#include <TGraph.h>
#include <TCanvas.h>
#include <TString.h>
#include <TText.h>
#include <TLine.h>
#include <numeric>
#include <algorithm>
#include <memory>
void drawWaveform(const std::shared_ptr<std::vector<int16_t>> &values)
{
    TGraph *graph = new TGraph(values->size());
    for (int pointIndex = 0; pointIndex < values->size(); pointIndex++)
    {
        graph->SetPoint(pointIndex, pointIndex, (*values)[pointIndex]);
    }
    graph->Draw("APL");
    gPad->Update();
    gPad->WaitPrimitive("ggg");

    delete graph;
}

void drawTwoWaveforms(const std::shared_ptr<std::vector<int16_t>> &values1, const std::shared_ptr<std::vector<int16_t>> &values2)
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

// void drawPointOnGraph(int16_t x, int16_t y)
// {
//     TLine *line1 = new TLine(x, gPad->GetUymin(), x, gPad->GetUymax());
//     TLine *line2 = new TLine(gPad->GetUymin(), y, gPad->GetUymax(), y);
//     line1->Draw("same");
//     line2->Draw("same");
//     gPad->Update();
//     gPad->WaitPrimitive("ggg");
//     delete line1;
//     delete line2;
// }

std::shared_ptr<std::vector<int16_t>> subtractBackground(const std::shared_ptr<std::vector<int16_t>> &values, int16_t noise_samples)
{

    std::shared_ptr<std::vector<int16_t>> result = std::make_shared<std::vector<int16_t>>();

    // result->assign(values->begin(), values->end());

    int16_t meanBackground = (std::accumulate(values->begin(), values->begin() + noise_samples, 0)) / noise_samples;

    // for (int i = 0; i < result->size(); i++)
    // {
    //     (*result)[i] -= mean_noise;
    // }

    std::transform(values->begin(), values->end(),
                   std::back_inserter(*result), [meanBackground](int16_t element)
                   { return element - meanBackground; });

    return result;
}

std::shared_ptr<std::vector<int16_t>> reverseWaveform(const std::shared_ptr<std::vector<int16_t>> &values)
{

    std::shared_ptr<std::vector<int16_t>> result = std::make_shared<std::vector<int16_t>>();

    std::transform(values->begin(), values->end(),
                   std::back_inserter(*result), [](int16_t element)
                   { return -element; });

    return result;
}

int16_t leadingEdgeDiscrimination(const std::shared_ptr<std::vector<int16_t>> &values, int16_t threshold)
{
    std::vector<int16_t>::iterator it = std::find_if(values->begin(), values->end(), [threshold](int16_t element)
                                                     { return element > threshold; });
    return *it;
}
int16_t energyExtractionMax(const std::shared_ptr<std::vector<int16_t>> &values)
{ // implemented for positive signals with extracted baseline
    std::vector<int16_t>::iterator max = std::max_element(values->begin(), values->end());
    return *max;
}

int16_t energyExtractionGate(const std::shared_ptr<std::vector<int16_t>> &values, int16_t threshold, int16_t gateLength)
{
    int16_t pulseBegin = leadingEdgeDiscrimination(values, threshold);
    int16_t pulseEnd = pulseBegin + gateLength;
    return std::accumulate(values->begin() + pulseBegin, values->end() + pulseEnd, 0) / gateLength;
}