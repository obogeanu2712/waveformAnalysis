#include <vector>
#include <cstdint>
#include <memory>

#pragma once

void drawWaveform(const std::shared_ptr<std::vector<int16_t>> &values);
void drawTwoWaveforms(const std::shared_ptr<std::vector<int16_t>> &values1, const std::shared_ptr<std::vector<int16_t>> &values2);
std::shared_ptr<std::vector<int16_t>> subtractBackground(const std::shared_ptr<std::vector<int16_t>> &values, int16_t noise_samples);
std::shared_ptr<std::vector<int16_t>> reverseWaveform(const std::shared_ptr<std::vector<int16_t>> &values);
int16_t leadingEdgeDiscrimination(const std::shared_ptr<std::vector<int16_t>> &values, int16_t threshold);
int16_t energyExtractionMax(const std::shared_ptr<std::vector<int16_t>> &values);
int16_t energyExtractionGate(const std::shared_ptr<std::vector<int16_t>> &values, int16_t threshold, int16_t gateLength);