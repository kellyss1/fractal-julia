#include "fractal_openmp.h"
#include "palette.h"

#include <omp.h>
#include <complex>
#include <immintrin.h>

extern int max_iteraciones;
extern std::complex<double> c;

//--------------------------------------------------------

uint32_t acotado_openmp(double x, double y) {
    /*
    dados: c, z0 
    zn+1 = zn^2 + c
    */
    int iter =1;

    double zr=x;
    double zi=y;

    while(iter<max_iteraciones && (zr*zr+zi*zi)< 4.0){
        //zn+1 = zn^2 + c
        double dr = zr*zr-zi*zi + c.real();
        double di = 2.0*zr*zi + c.imag();

        zr = dr;
        zi = di;

        iter++;
    }
    
    if(iter < max_iteraciones){
        int index = iter % PALETTE_SIZE; //obtenemos el indice del color en la paleta
        return color_ramp[index]; //retornamos el color correspondiente al indice
    }
    return 0xFF000000; //negro RGBA estan al revez
}


void julia_openmp_regiones(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int thread_count = omp_get_num_threads();

        int delta = std::ceil(width *1.0 / thread_count);
        int start = thread_id * delta;
        int end = (thread_id + 1) * delta;

        if(thread_id == thread_count - 1){
            end = width; //el ultimo hilo se encarga de lo que quede
        }

        for(int i = start; i < end; i++){
            for(int j = 0; j < height; j++){
                double x = x_min + i * dx;
                double y = y_max - j * dy;

                auto color = acotado_openmp(x, y);
                pixel_buffer[j * width + i] = color;
            }
        }
    }

}

void julia_openmp_for(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    #pragma omp parallel for
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            double x = x_min + i * dx;
            double y = y_max - j * dy;

            auto color = acotado_openmp(x, y);
            pixel_buffer[j * width + i] = color;
        }
    }

}

void julia_openmp_for_simd(double x_min, double x_max, double y_min, double y_max, int width, int height, uint32_t* pixel_buffer){
// Implementación SIMD aquí
    //std::memset(pixel_buffer, 0xFF000000, width * height * sizeof(uint32_t));
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;
 
    //(x_min) (x_min) (x_min) (x_min) (x_min) (x_min) (x_min) (x_min)
    //(-1.5) (-1.5) (-1.5) (-1.5) (-1.5) (-1.5) (-1.5) (-1.5)
    __m256 xmin = _mm256_set1_ps(x_min);
    //(y_max) (y_max) (y_max) (y_max) (y_max) (y_max) (y_max) (y_max)
    //(-1.5) (-1.5) (-1.5) (-1.5) (-1.5) (-1.5) (1) (1)
    __m256 ymax = _mm256_set1_ps(y_max);
 
    __m256 xscale = _mm256_set1_ps(dx);
    __m256 yscale = _mm256_set1_ps(dy);
 
    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());
 
    __m256 max_norma = _mm256_set1_ps(4.0f);
 
    __m256 one = _mm256_set1_ps(1.0f);
 
    for(int i = 0; i<width; i++){
        for (int j = 0; j < height; j+=8)
        {
            __m256 mx = _mm256_set1_ps(i);
            __m256 my = _mm256_set_ps(j+7, j+6, j+5, j+4, j+3, j+2, j+1, j+0 );
 
            //x_min+mx*scale -->(x,,0,x1,x2,x3,x4,x5,x6,x7) <-- REAL
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, xscale));
            //x_max-my*scale -->(y,,y0,y1,y2,y3,y4,y5,y6,y7) <-- IMAGINARIO
            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale));
 
            //verificar si 8 compeljos cr y ci estan acotados
            int iter = 1;
            __m256 mk = _mm256_set1_ps(iter);
           
            __m256 zr = cr;
            __m256 zi = ci;
 
            while(iter<max_iteraciones){
                //zn+1 = zn^2 + c
                //zn^2 = (zr + zi*i)^2 = (zr^2 - zi^2) + 2*zr*zi*i
                __m256 zr2 = _mm256_mul_ps(zr, zr);//zr al 2
                __m256 zi2 = _mm256_mul_ps(zi, zi);//zi al 2
                __m256 zr_zi = _mm256_mul_ps(zr, zi);//zr*zi
 
                zr = _mm256_add_ps( _mm256_sub_ps(zr2, zi2), c_real); //real
                zi = _mm256_add_ps( _mm256_add_ps( zr_zi, zr_zi), c_imag); //imag
               
                //norma = zr^2 + zi^2
                zr2 = _mm256_mul_ps(zr, zr);//zr al 2
                zi2 = _mm256_mul_ps(zi, zi);//zi al 2
                __m256 norma2 = _mm256_add_ps(zr2, zi2); //norma
 
                //si norma2<=4.0f, devuelve 0xFFFFFFFF, sino devuelve 0xFF000000
                __m256 mask = _mm256_cmp_ps(norma2, max_norma, _CMP_LE_OQ); //compara norma con 4.0f
 
               mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);
                if (_mm256_testz_ps(mask, _mm256_set1_ps(-1)))
                {
                   break; //si todos los elementos de mask son 0, salir del ciclo
                }
               
                iter++;
 
            }
            float d[8];
            _mm256_storeu_ps(d, mk);
 
            for (int it = 0; it < 8; it++)
            {
                int index = (j+it)*width + i;
                if(index < width*height){
                    if(d[it] < max_iteraciones){
                        int color_index = (int)d[it] % PALETTE_SIZE;
                        pixel_buffer[index] = color_ramp[color_index];
                    }else{
                        pixel_buffer[index] = 0xFF000000; // Negro
                    }
                }
            }
           
        }
       
    }

}

