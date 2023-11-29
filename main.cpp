#include "eventProto.capnp.h"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <fcntl.h>
#include <map>
#include <utility>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <TFile.h>
#include <TGraph.h>
#include <TString.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TH1I.h>
#include <string>
#include "functions.hpp" //my header
#include <memory>
using json = nlohmann::json;
using namespace std;

// using waveform_t = vector<int16_t>;
// using energies_t = vector<int16_t>;
// using pair_t = pair<uint8_t, uint8_t>;
// using waveformsMap_t = map<pair<uint8_t, uint8_t>, shared_ptr<vector<shared_ptr<vector<int16_t>>>>>;
// using energyMap_t = map<pair<uint8_t, uint8_t>, shared_ptr<vector<int16_t>>>;

int main(int argc, char **argv)
{

    // ifstream jsonConfigFile("config.json");
    // json jsonConfig;
    // // Read JSON file and store it inside an object
    // jsonConfigFile >> jsonConfig;
    // vector<vector<int>> counts = jsonConfig["detectors"].get<vector<vector<int>>>(); // Store the number of waveforms to be extracted
    // int16_t noiseSamples = jsonConfig["noiseSamples"];
    // string fileName = jsonConfig["fileName"];
    // int16_t threshold = jsonConfig["threshold"];
    // // Open the .cap file for reading
    // int fd = open(fileName.c_str(), O_RDONLY);
    // kj::FdInputStream inputStream(fd);
    // kj::BufferedInputStreamWrapper bufferedStream(inputStream);
    // // Read the .cap file
    // shared_ptr<map<pair<uint8_t, uint8_t>, shared_ptr<vector<shared_ptr<vector<int16_t>>>>>> waveforms =
    //     make_shared<map<pair<uint8_t, uint8_t>, shared_ptr<vector<shared_ptr<vector<int16_t>>>>>>(); // map to store waveforms with board / channel key
    // while (bufferedStream.tryGetReadBuffer() != nullptr)
    // {
    //     capnp::PackedMessageReader message(bufferedStream, {100000000, 64});
    //     WaveData::Reader wavedata = message.getRoot<WaveData>();
    //     auto waveEvents = wavedata.getEvents();
    //     // Iterate through the events
    //     for (auto event : waveEvents)
    //     {
    //         // // Access event fields and extract necessary waveforms
    //         uint8_t board = event.getBoard();
    //         uint8_t channel = event.getChannel();
    //         pair BoardChannelKey = make_pair(board, channel);
    //         if (!(*waveforms)[BoardChannelKey])
    //         {
    //             (*waveforms)[BoardChannelKey] = make_shared<vector<shared_ptr<vector<int16_t>>>>();
    //         }
    //         // Check if there are any other waveforms to store and store them
    //         if (counts[board][channel])
    //         {
    //             auto waveformList = event.getWaveform1();
    //             shared_ptr<vector<int16_t>> waveform = make_shared<vector<int16_t>>();
    //             for (const int16_t &value : waveformList)
    //             {
    //                 waveform->push_back(value);
    //             }
    //             (*waveforms)[BoardChannelKey]->push_back(waveform);
    //             // counts[board][channel]--; // comment to extract all
    //         }
    //     }
    // }

    shared_ptr<vector<Event>> Events = readEvents("Run_000001.cap", "config.json");

    TApplication app("Display waveforms", &argc, argv);

    // Display waveforms in TApp

    for (const Event &event : *Events)
    {
        drawWaveform(event.waveform, event.board, event.channel);
    }

    // for (const auto &waveformsVector : *waveforms)
    // {
    //     uint8_t board = waveformsVector.first.first;
    //     uint8_t channel = waveformsVector.first.second;
    //     shared_ptr<vector<shared_ptr<waveform_t>>> values = waveformsVector.second;
    //     for (int waveformIndex = 0; waveformIndex < values->size(); waveformIndex++)
    //     {
    //         // drawWaveform(waveform1, board, channel);
    //         // drawWaveform(subtractBackground(waveform1, noiseSamples), board, channel);
    //         drawWaveform((*values)[waveformIndex], board, channel, waveformIndex + 1);
    //     }
    // }
    // Create energy map
    // subtractBackground(waveforms, noiseSamples);
    // shared_ptr<energyMap_t> energies = energyExtractionMax(waveforms);
    // Draw energy histogram
    // drawHistogram(energies);
}
