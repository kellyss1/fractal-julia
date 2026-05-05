#ifndef FRACTAL_SERIAL_H
#define FRACTAL_SERIAL_H

#include <cstdint>
#include <complex>

void julia_serial_1(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
uint32_t acotado_1(std::complex<double> z0);

void julia_serial_2(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
uint32_t acotado_2(double x, double y);

#endif

//locales son entre comillas, globales entre #ifndef y #endif