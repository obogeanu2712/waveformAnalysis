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
<<<<<<< HEAD
    pair<int16_t, double_t> CFD;
=======
    double_t CFDindex;
>>>>>>> 72489049804f5f572022a0c5102be0937bc875b7
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

shared_ptr<vector<int16_t>> delayWithGaussian(const shared_ptr<vector<int16_t>>& values, int16_t delay);

shared_ptr<vector<int16_t>> attenuate(const shared_ptr<vector<int16_t>>& values, double_t attenuation);

shared_ptr<vector<int16_t>> sumSignals(const shared_ptr<vector<int16_t>>& values1, const shared_ptr<vector<int16_t>>& values2);

<<<<<<< HEAD
pair<int16_t, double_t> CFD(const shared_ptr<vector<int16_t>> &values, double_t attenuation, int16_t delay, int16_t threshold);
=======
double_t CFD(const shared_ptr<vector<int16_t>> &values, double_t attenuation, int16_t delay, int16_t threshold);
>>>>>>> 72489049804f5f572022a0c5102be0937bc875b7

bool saturated(const shared_ptr<vector<int16_t>> &values, int16_t gate);

bool pileup(const shared_ptr<vector<int16_t>> &values, double_t amplitudeFraction, int16_t threshold, int16_t gateLength);

int16_t energyExtractionMax(const shared_ptr<vector<int16_t>> &values);

int16_t energyExtractionGate(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate);

pair<int16_t, bool> EnergyGatePileup(const shared_ptr<vector<int16_t>> &values, int16_t threshold, int16_t gateLength, int16_t preGate, double_t amplitudeFraction);

shared_ptr<vector<Event>> processEvents(const shared_ptr<vector<Event>> &rawEvents, string configFileName);