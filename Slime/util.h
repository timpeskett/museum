#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <math.h>

#define oppang(x) ((x>=180)?(x-180):(x+180))
#define PI (3.1415926)
#define RAD(x) ((x)*PI/180)
#define DEGREES(x) ((x)*180/PI)

int ConvToTrue( int conv, int xsect, int ysect );

int TrueToConv( int tru, int *xsect, int *ysect );

float Magnitude( float x, float y );

#endif
