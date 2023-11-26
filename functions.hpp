#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <map>

using namespace std;
using waveform_t = vector<int16_t>;
using energies_t = vector<int16_t>;
using pair_t = pair<uint8_t, uint8_t>;
using waveformsMap_t = map<pair_t, shared_ptr<vector<shared_ptr<waveform_t>>>>;
using energyMap_t = map<pair_t, shared_ptr<energies_t>>;
void drawWaveform(const shared_ptr<waveform_t> &values, uint8_t board, uint8_t channel, int index);
void drawTwoWaveforms(const shared_ptr<waveform_t> &values1, const shared_ptr<waveform_t> &values2);
void drawHistogram(const shared_ptr<energies_t> energies, uint8_t board, uint8_t channel);
void drawHistogram(const shared_ptr<energyMap_t> &energyMap);

shared_ptr<waveform_t> subtractBackground(const shared_ptr<waveform_t> &values, int16_t noiseSamples);
shared_ptr<vector<shared_ptr<waveform_t>>> subtractBackground(const shared_ptr<vector<shared_ptr<waveform_t>>> &waveforms, int16_t noiseSamples);
shared_ptr<waveformsMap_t> subtractBackground(const shared_ptr<waveformsMap_t> &waveforms, int16_t noiseSamples);

shared_ptr<waveform_t> reverseWaveform(const shared_ptr<waveform_t> &values);
shared_ptr<waveformsMap_t> reverseWaveform(const shared_ptr<waveformsMap_t> &waveforms);

int16_t leadingEdgeDiscrimination(const shared_ptr<waveform_t> &values, int16_t threshold);

int16_t energyExtractionMax(const shared_ptr<waveform_t> &values);
shared_ptr<energies_t> energyExtractionMax(const shared_ptr<vector<shared_ptr<waveform_t>>> &waveforms);
shared_ptr<energyMap_t> energyExtractionMax(const shared_ptr<waveformsMap_t> &waveformsMap);

int16_t energyExtractionGate(const shared_ptr<waveform_t> &values, int16_t threshold, int16_t gateLength, int16_t preGate);
shared_ptr<energyMap_t> MaxEnergyMap(const shared_ptr<waveformsMap_t> &waveformsMap, int16_t noiseSamples);