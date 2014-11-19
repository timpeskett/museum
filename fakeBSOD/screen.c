#include "screen.h"

int GetScreenSettings( DEVMODE *settings, int screenw, int screenh )
{
    DWORD i;
    int ret = 1;

    for( i = 0; ret; i++ )
    {
        ret = EnumDisplaySettings( NULL, i, settings );
        if( settings->dmPelsWidth == screenw && settings->dmPelsHeight == screenh )
        {
            return ret;
        }
    }

    return 0;
}
