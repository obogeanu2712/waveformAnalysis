#include <vector>
#include <TApplication.h>
#include <TText.h>
#include <TCanvas.h>
#include <string>
#include "functions.hpp" //my header
#include <memory>
using namespace std;

int main(int argc, char **argv)
{

    shared_ptr<vector<Event>> Events = readEvents("Run_000001.cap", "config.json");

    TApplication app("Display waveforms", &argc, argv);

    // Display waveforms in TApp

    for (const Event &event : *Events)
    {
        drawEvent(event);
    }

    app.Run();
}
