#include <vector>
#include <cstdint>
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void drawWaveform(std::vector<__INT16_TYPE__>* values);
void drawTwoWaveforms(std::vector<__INT16_TYPE__>* values1, std::vector<__INT16_TYPE__>* values2);
std::vector<__INT16_TYPE__>* subtractBackground(std::vector<__INT16_TYPE__>* values, int noise_samples); 

#endif