#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <map>
#include <TGraph.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
class Event
{
public:
    uint8_t board;
    uint8_t channel;
    int16_t thresholdIndex;
    uint16_t fileEnergy;
    int16_t energyMax;
    int16_t energyGate;
    shared_ptr<vector<int16_t>> waveform; // asume you create a shared ptr in the reading function
    bool saturated;
    bool pileup;
    Event(uint8_t board, uint8_t channel, uint16_t energy, shared_ptr<vector<int16_t>> waveform);
};

shared_ptr<vector<Event>> readEvents(string configFileName);

void drawEvent(const Event &event, const json &jsonConfig);

void drawEvents(const shared_ptr<vector<Event>> &events, string configFileName);

shared_ptr<TGraph> drawWaveform(const shared_ptr<vector<int16_t>> &values, uint8_t board, uint8_t channel);

void drawTwoWaveforms(const shared_ptr<vector<int16_t>> &values1, const shared_ptr<vector<int16_t>> &values2);

void drawHistogram(const shared_ptr<vector<int16_t>> energies, uint8_t board, uint8_t channel);

shared_ptr<vector<int16_t>> subtractBackground(const shared_ptr<vector<int16_t>> &values, int16_t noiseSamples);

shared_ptr<vector<int16_t>> reverseWaveform(const shared_ptr<vector<int16_t>> &values);

int16_t leadingEdgeDiscrimination(const shared_ptr<vector<int16_t>> &values, int16_t threshold);

bool saturated(const shared_ptr<vector<int16_t>> &values, int16_t gate);

bool pileup(const shared_ptr<vector<int16_t>> &values, float amplitudeFraction, int16_t threshold, int16_t gateLength);

int16_t energyExtractionMax(const shared_ptr<vector<int16_t>> &values);

int16_t energyExtractionGate(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate);

shared_ptr<vector<Event>> processEvents(const shared_ptr<vector<Event>> &rawEvents, string configFileName);