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

    std::ofstream json_file("counts.json");
    std::ofstream counts_file("counts.txt");
    std::ifstream json_config_file("config.json");
    json jsonCounts;
    json jsonConfig;

    std::map<std::pair<uint8_t, uint8_t>, std::vector<int16_t>> waveforms;
    std::map<std::pair<uint8_t, uint8_t>, int> waveformCounts;

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
            // // Access event fields
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();

            std::pair BoardChannelKey = std::make_pair(board, channel);

            auto waveform_list = event.getWaveform1();

            if(waveform_list.size()) waveformCounts[BoardChannelKey]++;
        }

    }

    for (const auto& waveform : waveformCounts) {
        int board = static_cast<int>(waveform.first.first);
        int channel = static_cast<int>(waveform.first.second);
        int counts = static_cast<int>(waveform.second);
        jsonCounts["detectors"][board][channel] = counts;
        counts_file << "Board " << board << " Channel " << channel << " has  " << counts 
        << " waveform counts" << std::endl << std::endl;
    }




    json_file << jsonCounts.dump(2);

    json_file.close();
    counts_file.close();
}
