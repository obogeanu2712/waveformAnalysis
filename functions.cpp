#include "functions.hpp"
#include <TGraph.h>
#include <TCanvas.h>
#include <TString.h>
#include <TText.h>
#include <numeric>
void drawWaveform(std::vector<int16_t>* values) {
    TGraph* graph = new TGraph(values -> size());
    for(int point_index = 0; point_index < values -> size(); point_index++) {
        graph->SetPoint(point_index, point_index, (*values)[point_index]);
    }
    graph->Draw("APL");
    gPad -> Update();

    gPad -> WaitPrimitive("ggg");
    
    delete graph;
}

void drawTwoWaveforms(std::vector<int16_t>* values1, std::vector<int16_t>* values2) {
    TGraph *graph1, *graph2;
    graph1 = new TGraph(values1 -> size());
    graph2 = new TGraph(values2 -> size());

    for(int point_index = 0; point_index < values1 -> size(); point_index++) {
        graph1->SetPoint(point_index, point_index, (*values1)[point_index]);
    }

    for(int point_index = 0; point_index < values2 -> size(); point_index++) {
        graph2->SetPoint(point_index, point_index, (*values2)[point_index]);
    }

    graph1 -> Draw("APL");
    graph2 -> Draw("PL");
    gPad -> Update();
    gPad -> WaitPrimitive("ggg");

    delete graph1;
    delete graph2;
}

std::vector<int16_t>* subtractBackground(std::vector<int16_t>* values) {

    std::vector<int16_t>* result = new std::vector<int16_t>;

    result->assign(values->begin(), values->end());

    int64_t integral = (std::accumulate(values->begin(), values->end(), 0))/(values->size());

    for(int i = 0; i < result->size(); i++) {
        (*result)[i] -= integral;
    }

    return result;
}