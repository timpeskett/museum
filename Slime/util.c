#include "util.h"


int ConvToTrue( int conv, int xsect, int ysect )
{
    switch( xsect * 2 + ysect )
    {
        case 3:
            conv = 90 - conv;
            break;
        case 1:
            conv += 90;
            break;
        case -3:
            conv = 90 - conv + 180;
            break;
        case -1:
            conv += 270;
            break;
        default:
            conv = 0;
            break;
    }
    return conv;
}


int TrueToConv( int tru, int *xsect, int *ysect )
{
    *xsect = 1;
    *ysect = 1;

    switch( tru / 90 )
    {
        case 0:
            tru = 90 - tru;
            break;
        case 1:
            *ysect = -1;
            tru -= 90;
            break;
        case 2:
            *xsect = *ysect = -1;
            tru = 90 - (tru - 180);
            break;
        case 3:
            *xsect = -1;
            tru -= 270;
            break;
        default:
            break;
    }

    return tru;
}


float Magnitude( float x, float y )
{
    return sqrt( pow( x, 2 ) + pow( y, 2 ) );
}
