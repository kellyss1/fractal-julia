#ifndef PALETTE_H
#define PALETTE_H

#include <vector>
#include <cstdint>

#define PALETTE_SIZE 16

extern std::vector<uint32_t> color_ramp;
extern std::vector<uint32_t> color_ramp_blue;
extern std::vector<uint32_t> color_ramp_green;
extern std::vector<uint32_t> color_ramp_mono;

#endif // PALETTE_H