#include "eventProto.capnp.h"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <fcntl.h>
#include <map>
#include <utility>
#include <iostream>
#include <vector>

using WaveformReader = capnp::List<int16_t, capnp::Kind::PRIMITIVE>::Reader;

int main() {

    std::map<std::pair<uint8_t, uint8_t>, WaveformReader> waveforms;

    // Open the .cap file for reading
    int fd = open("Run_000001.cap", O_RDONLY);

    if(fd == -1) {
        std::cout << "File couldn't be opened" << std::endl;
        return 1;
    } 

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
            // WaveformReader waveform1 = event.getWaveform1();
            waveforms[std::make_pair(board, channel)] = event.getWaveform1();
        }

    }

    ///Print the stored values for the wave form
    for (const auto waveform : waveforms) {
        std::cout << "Board " << static_cast<int>(waveform.first.first) << " and Channel " 
            << static_cast<int>(waveform.first.second) << ":" << std::endl;
        auto waveform_values = waveform.second;
        for(int i = 0; i < waveform_values.size(); i++) {
            int value = static_cast<int>(waveform_values[i]);
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}
