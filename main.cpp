#include <vector>
#include <TApplication.h>
#include <TText.h>
#include <TCanvas.h>
#include <string>
#include "functions.hpp" //my header
#include <memory>
#include <TH1D.h>
using namespace std;

int main(int argc, char **argv)
{

    shared_ptr<vector<Event>> Events = readEvents("config.json");

    TApplication app("Display waveforms", &argc, argv);

    // Process Events

    shared_ptr<vector<Event>> processedEvents = processEvents(Events, "config.json");

    // Display waveforms in TApp
    drawEvents(processedEvents, "config.json");

    //create histogram and display it

    // shared_ptr<TH1D> histogram = FineTimestampsHistogram(processedEvents);

    // histogram->Draw();
    // gPad->Update();
    // gPad->WaitPrimitive("ggg");

    app.Run();
}
