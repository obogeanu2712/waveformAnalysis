#include "eventProto.capnp.h"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <fcntl.h>
#include <map>
#include <utility>
#include <iostream>
#include <vector>
#include "json.hpp"
#include <fstream>


using WaveformReader = capnp::List<int16_t, capnp::Kind::PRIMITIVE>::Reader;
using json = nlohmann::json;

int main() {

    std::ofstream waveforms_file("waveforms.txt");


    std::ifstream json_config_file("config.json");
    json jsonConfig;

    //Read JSON file and store it inside an object

    json_config_file >> jsonConfig;


    std::map<std::pair<uint8_t, uint8_t>, std::vector<int16_t>> waveforms;

    //Store the number of waveforms to be extracted

    std::map<std::pair<uint8_t, uint8_t>, int> waveforms_number;

    json jsonArrayofarrays = jsonConfig["detectors"];

    for(int board = 0; board < jsonArrayofarrays.size(); board++) {
        for(int channel = 0; channel < jsonArrayofarrays[board].size(); channel++) {
            waveforms_number[std::make_pair(board, channel)] = jsonArrayofarrays[board][channel];
        }
    }

    // Open the .cap file for reading
    int fd = open("Run_000001.cap", O_RDONLY);

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
            if(waveforms_number[BoardChannelKey]) {
                auto waveform_list = event.getWaveform1();

                for(const auto& value : waveform_list) {
                    waveforms[BoardChannelKey].push_back(value);
                }
                waveforms_number[BoardChannelKey]--;
            }
            
            
        }

    }

    
    json_config_file.close();
    waveforms_file.close();
}
