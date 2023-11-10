#include <vector>
#include <cstdint>
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void drawWaveform(std::vector<__INT16_TYPE__>* values);
void drawTwoWaveforms(std::vector<__INT16_TYPE__>* values1, std::vector<__INT16_TYPE__>* values2);
void drawPointOnGraph(__INT16_TYPE__ x, __INT16_TYPE__ y);
std::vector<__INT16_TYPE__>* subtractBackground(std::vector<__INT16_TYPE__>* values, __INT16_TYPE__ noise_samples);
std::vector<__INT16_TYPE__>* reverseWaveform(std::vector<__INT16_TYPE__>* values);
__INT16_TYPE__ leadingEdgeDiscrimination(std::vector<__INT16_TYPE__>* values, __INT16_TYPE__ threshold);
__INT16_TYPE__ energyExtractionMax(std::vector<__INT16_TYPE__>*values);
#endif