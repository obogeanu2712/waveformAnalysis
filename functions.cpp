#include "functions.hpp"
#include <TGraph.h>
#include <TCanvas.h>
void drawWaveform(std::vector<int16_t> values) {
    TGraph* graph = new TGraph(values.size());
    for(int point_index = 0; point_index < values.size(); point_index++) {
        graph->SetPoint(point_index, point_index, values[point_index]);
    }
    graph->Draw("APL");
    gPad -> Update();
    gPad -> WaitPrimitive("ggg");
    
    delete graph;
}