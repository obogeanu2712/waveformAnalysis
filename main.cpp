#include <vector>
#include <TApplication.h>
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
        drawWaveform(event.waveform, event.board, event.channel);
    }
}
