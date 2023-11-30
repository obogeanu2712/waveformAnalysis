#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <map>
#include <TGraph.h>

using namespace std;

class Event
{
public:
    uint8_t board;
    uint8_t channel;
    uint16_t energy;
    shared_ptr<vector<int16_t>> waveform; // asume you create a shared ptr in the reading function
    Event(uint8_t board, uint8_t channel, uint16_t energy, shared_ptr<vector<int16_t>> waveform);
};

shared_ptr<vector<Event>> readEvents(string fileName, string configFileName);

void drawEvent(const Event &event);

shared_ptr<TGraph> drawWaveform(const shared_ptr<vector<int16_t>> &values, uint8_t board, uint8_t channel);

void drawTwoWaveforms(const shared_ptr<vector<int16_t>> &values1, const shared_ptr<vector<int16_t>> &values2);

void drawHistogram(const shared_ptr<vector<int16_t>> energies, uint8_t board, uint8_t channel);

shared_ptr<vector<int16_t>> subtractBackground(const shared_ptr<vector<int16_t>> &values, int16_t noiseSamples);

shared_ptr<vector<int16_t>> reverseWaveform(const shared_ptr<vector<int16_t>> &values);

int16_t leadingEdgeDiscrimination(const shared_ptr<vector<int16_t>> &values, int16_t threshold);

int16_t energyExtractionMax(const shared_ptr<vector<int16_t>> &values);

int16_t energyExtractionGate(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate);
shared_ptr<map<pair<uint8_t, uint8_t>, shared_ptr<vector<int16_t>>>> MaxEnergyMap(const shared_ptr<map<pair<uint8_t, uint8_t>, shared_ptr<vector<shared_ptr<vector<int16_t>>>>>> &waveformsMap, int16_t noiseSamples);