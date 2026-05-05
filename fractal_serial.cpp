#include "fractal_serial.h"
#include "palette.h"

#include <complex>

extern int max_iteraciones;
extern std::complex<double> c;

void julia_serial_1(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    double dx = (x_max - x_min)/ width;
    double dy = (y_max - y_min)/ height;

    for(int i=0; i < width; i++){
        for(int j=0; j<height; j++){
            //z = x+yi = (x+y)
            //calculamos el complejo y verificamos si esta acotado
            //entre 0 y 1
            double x = x_min+i*dx;
            double y= y_max-j*dy;
            
            std::complex<double> z(x,y);
            auto color = acotado_1(z); //es igual al var en java
            pixel_buffer[j * width + i] = color;

        }
    }

}

uint32_t acotado_1(std::complex<double> z0) {
    /*
    dados: c, z0 
    zn+1 = zn^2 + c
    */
    int iter =1;

    std::complex<double> z = z0;

    while(iter<max_iteraciones && std::abs(z) < 2.0){
        //zn+1 = zn^2 + c
        z = z*z + c;
        iter++;
    }
    
    if(iter < max_iteraciones){ //Si se sale de la norma2 antes de llegar a max_iteraciones, entonces el punto no es acotado
        //la norma2
        // return 0xFF0000FF; //rojo  

        int index = iter % PALETTE_SIZE; //obtenemos el indice del color en la paleta
        return color_ramp[index]; //retornamos el color correspondiente al indice
    }
    return 0xFF000000; //negro RGBA estan al revez
}

//--------------------------------------------------------

uint32_t acotado_2(double x, double y) {
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

void julia_serial_2(double x_min, double x_max, double y_min, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    double dx = (x_max - x_min)/ width;
    double dy = (y_max - y_min)/ height;

    for(int i=0; i < width; i++){
        for(int j=0; j<height; j++){
            //z = x+yi = (x+y)
            //calculamos el complejo y verificamos si esta acotado
            //entre 0 y 1
            double x = x_min+i*dx;
            double y= y_max-j*dy;
            
            auto color = acotado_2(x, y); //es igual al var en java
            pixel_buffer[j * width + i] = color;

        }
    }

}

