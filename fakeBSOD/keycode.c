#include "keycode.h"

int IsValidKeycode( char *keycode )
{
    int i = 0;

    for( i = 0; i < 3; i++ )
    {
        if( !isprint( *(keycode + i ) ) )
        {
            return 0;
        }
    }

    return 1;
}



unsigned int MapCharToVirtual( char convertee )
{
    int i, charcode = 0;

    for( i = 0; i < 256; i++ )
    {
        charcode = MapVirtualKey( i, 2 );
        if( tolower(convertee) == (char)charcode || toupper(convertee) == (char)charcode )
        {
            return i;
        }
    }

    return 0;
}
