#ifndef FRACTAL_OPENMP_H
#define FRACTAL_OPENMP_H


#include <cstdint>

void julia_openmp_regiones(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
void julia_openmp_for(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
void julia_openmp_for_simd(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);

#endif // FRACTAL_OPENMP_H