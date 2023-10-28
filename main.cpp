#include "eventProto.capnp.h"
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <fcntl.h>
#include <map>
#include <utility>
#include <iostream>
int main() {

    std::map<std::pair<uint8_t, uint8_t>, uint16_t> channelBoardCounts;

    // Open the .cap file for reading
    kj::FdInputStream inputStream(open("Run_000001.cap", O_RDONLY));
    kj::BufferedInputStreamWrapper bufferedStream(inputStream);
    // Read the .cap file
    
    while (bufferedStream.tryGetReadBuffer() != nullptr) {
        
        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});
        Dataset::Reader dataset = message.getRoot<Dataset>();

        auto Events = dataset.getEvents();

        // Iterate through the events
        for (auto event : Events) {
            // // Access event fields
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();
            channelBoardCounts[std::make_pair(board, channel)]++;
            // // uint16_t energy = event.getEnergy();
            // // uint64_t timestamp = event.getTimestamp();
            // // float psd = event.getPsd();
            // // auto waveform = event.getWaveform();

            // // Process the event data as needed


        }

    }
    for(const auto& object : channelBoardCounts) {
        std::cout << "Board " << static_cast<int>(object.first.first) << ", Channel " 
            << static_cast<int>(object.first.second)
            << ": " << static_cast<int>(object.second) << " counts" << std::endl;
    }
    return 0;
}
