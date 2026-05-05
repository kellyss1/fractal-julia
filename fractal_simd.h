
#ifndef FRACTAL_SIMD_H
#define FRACTAL_SIMD_H

//#pragma once --le dice al compilador que solo incluya este header una vez, es una forma mas moderna de hacer lo mismo que el include guard

#include <cstdint>

void julia_simd(double x_min, double x_max, double y_min, double y_max, int width, int height, uint32_t* pixel_buffer);

#endif //FRACTAL_SIMD_H