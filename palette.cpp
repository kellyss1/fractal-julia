#include "palette.h"

/*		
#FFFFCC
#FFF5B5
#FFEC9D
#FEE187
#FED470
#FEBF5A
#FEAB49
#FD9740
#FD7C37
#FC5B2E
#F43D25
#E6211E
#D41020
#C00225
#A10026
#800026
*/

uint32_t bswap32(uint32_t a) {
    return 
    ((a & 0x000000ff) << 24) | 
    ((a & 0x0000ff00) << 8) |
    ((a & 0x00ff0000) >> 8) |
    ((a & 0xff000000) >> 24);  
    
}

std::vector<uint32_t> color_ramp = {
//los bytes en c++ estan invertidos ABGR, los colores necesitan estar al revez de dos en dos 
    bswap32(0xFFFFCCFF), //1
    bswap32(0xFFF5B5FF), //2
    bswap32(0xFFEC9DFF), //3
    bswap32(0xFEE187FF), //4
    bswap32(0xFED470FF), //5
    bswap32(0xFEBF5AFF), //6
    bswap32(0xFEAB49FF), //7
    bswap32(0xFD9740FF), //8
    bswap32(0xFD7C37FF), //9
    bswap32(0xFC5B2EFF), //10
    bswap32(0xF43D25FF), //11
    bswap32(0xE6211EFF), //12
    bswap32(0xD41020FF), //13
    bswap32(0xC00225FF), //14
    bswap32(0xA10026FF), //15
    bswap32(0x800026FF)  //16
};