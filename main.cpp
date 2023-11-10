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
#include <string>
#include "functions.hpp" //my header
using json = nlohmann::json;

int main(int argc, char** argv) {


    std::ifstream json_config_file("config.json");
    json jsonConfig;

    //Read JSON file and store it inside an object
    json_config_file >> jsonConfig;
    auto counts = jsonConfig["detectors"].get<std::vector<std::vector<int>>>(); //Store the number of waveforms to be extracted
    int16_t noise_samples = jsonConfig["noise_samples"];
    std::string file_name = jsonConfig["file_name"];
    int16_t threshold = jsonConfig["threshold"];

    std::map<std::pair<uint8_t, uint8_t>, std::vector<std::vector<int16_t>>> waveforms;

    // Open the .cap file for reading
    
    int fd = open(file_name.c_str(), O_RDONLY);

    kj::FdInputStream inputStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(inputStream);
    // Read the .cap file
    
    while (bufferedStream.tryGetReadBuffer() != nullptr) {
        
        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});
        WaveData::Reader wavedata = message.getRoot<WaveData>();

        auto waveEvents = wavedata.getEvents();

        // Iterate through the events
        for (auto event : waveEvents) {
            // // Access event fields and extract necessary waveforms
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();
            std::pair BoardChannelKey = std::make_pair(board, channel);
            //Check if there are any other waveforms to store and store them
            if(counts[board][channel]) {
                auto waveform_list = event.getWaveform1();
                std::vector<int16_t> waveform;
                for(const auto& value : waveform_list) {
                    waveform.push_back(value);
                }
                waveforms[BoardChannelKey].push_back(waveform);
                counts[board][channel]--;
            }
        }
    }

    //Display waveforms in TApp

    TApplication app("Display waveforms", &argc, argv);

    for(const auto& waveforms_vector : waveforms) {
        int board = static_cast<int>(waveforms_vector.first.first);
        int channel = static_cast<int>(waveforms_vector.first.second);

        std::vector<std::vector<int16_t>> values = waveforms_vector.second;

        for(int waveform_index = 0; waveform_index < values.size(); waveform_index++) {
            // std::vector<int16_t>* displayed_waveform = reverseWaveform(subtractBackground(&values[waveform_index], noise_samples));
            // drawWaveform(displayed_waveform);
            drawWaveform(&values[waveform_index]);
            drawWaveform(subtractBackground(&values[waveform_index], noise_samples));
            drawWaveform(reverseWaveform(subtractBackground(&values[waveform_index],noise_samples)));
            // uint16_t timestamp = leadingEdgeDiscrimination(displayed_waveform, noise_samples, threshold);
            // drawPointOnGraph(timestamp, (*displayed_waveform)[timestamp]);
        }
    }

}
