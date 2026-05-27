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

void julia_openmp_for_simd(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    // Implementación de la función de Julia utilizando SIMD
    // Aquí puedes usar intrínsecos de SIMD para acelerar el cálculo
    // Asegúrate de manejar correctamente los bordes y las iteraciones

    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    //Genera un vector con los valores de x para cada píxel
    //(-1.5, -1.5, -1.5, -1.5, -1.5, -1.5, -1.5, -1.5)
    __m256 xmin = _mm256_set1_ps(x_min);
    //(1, 1, 1, 1, 1, 1, 1, 1)
    __m256 ymax = _mm256_set1_ps(y_max);

    __m256 sxcale = _mm256_set1_ps(dx); //dx,dx,dx,dx,dx,dx,dx,dx
    __m256 sycale = _mm256_set1_ps(dy); //dy,dy,dy,dy,dy,dy,dy,dy

    __m256 c_real = _mm256_set1_ps(c.real()); //creal(c),creal(c),creal(c),creal(c),creal(c),creal(c),creal(c),creal(c)
    __m256 c_imag = _mm256_set1_ps(c.imag()); //cimag(c),cimag(c),cimag(c),cimag(c),cimag(c),cimag(c),cimag(c),cimag(c)

    __m256 max_norma = _mm256_set1_ps(4.0f); // Norma máxima para determinar si el punto escapa
    __m256 one = _mm256_set1_ps(1.0f); // Incremento de 1 iteración para los puntos que no han escapado

    #pragma omp parallel for
    for(int i = 0; i < width; i ++){
        for(int j = 0; j < height; j += 8){ // Procesamos 8 píxeles a la vez

            // Carga los valores de x e y para los píxeles actuales
            __m256 mx = _mm256_set1_ps(i); // x_min + i*dx
            __m256 y = _mm256_set_ps(j+7, j+6, j+5, j+4, j+3, j+2, j+1, j+0); // y_max - (j + 7)*dy, y_max - (j + 6)*dy, ..., y_max - j*dy

            //xmin+mx*sxcale ---> (x0, x1, x2, x3, x4, x5, x6, x7) <-- real
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, sxcale)) ; // mx*sxcale

            //ymax - my*yscale --> (y0, y1, y2, y3, y4, y5, y6, y7) <-- imaginario
            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(y, sycale)) ; // my*yscale

            //Verificar si los 8 complejos (cr, ci) están dentro del conjunto de Julia utilizando iteraciones
            int iter = 1;
            __m256 mk = _mm256_set1_ps(iter); // iteraciones actuales
            __m256 zr = cr; // Parte real de z
            __m256 zi = ci; // Parte imaginaria de z

            while(iter < max_iteraciones) {
                // Calcula Zn+1 = z^2 + c para los 8 complejos
                __m256 zr2 = _mm256_mul_ps(zr, zr); // zr^2
                __m256 zi2 = _mm256_mul_ps(zi, zi); // zi^2
                __m256 zrzi = _mm256_mul_ps(zr, zi); // zr*zi

                // z^2 + c
                __m256 new_zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real); // zr^2 - zi^2 + creal(c)
                __m256 new_zi = _mm256_add_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), zrzi), c_imag); // 2*zr*zi + cimag(c)

                //--normas
                zr2 = _mm256_mul_ps(zr, zr); // zr^2
                zi2 = _mm256_mul_ps(zi, zi); // zi^2
            
                zr = new_zr;
                zi = new_zi;

                __m256 norma2 = _mm256_add_ps(zr2, zi2); // |z|^2

                //si norma2 < 4.0f devuelve el color 0xFFFFFFFF correspondiente, sino 0x000000
                __m256 mask = _mm256_cmp_ps(norma2, max_norma, _CMP_LE_OQ); // Compara |z|^2 con 4.0 para determinar si el punto escapa

                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk) ; // Si el punto no escapa, incrementa en 1 la iteración

                if (_mm256_movemask_ps(mask) == 0) {
                    break; // Todos los complejos han escapado
                }

                iter++;
            }

            float d[8];
            _mm256_storeu_ps(d, mk); // Almacena los resultados de las iteraciones en un arreglo

            for(int it = 0; it < 8; it++){
                int index = (j+it)*width + i;
                if(index<width*height){
                    if(d[it] < max_iteraciones){
                        int color_index = (int) d[it] % PALETTE_SIZE;
                        pixel_buffer[index] = color_ramp[color_index]; // Asigna el color correspondiente al píxel
                    }
                    else {
                        pixel_buffer[index] = 0xFF000000; // Color negro
                    }
                }
            }
        }

    }

