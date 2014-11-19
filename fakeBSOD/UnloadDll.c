#include "UnloadDll.h"


int UnloadDll( void )
{
    FILE *spdll;
    int i;

    spdll = fopen( "ShellProc.dll", "wb" );

    for( i = 0; i < arraysize; i++ )
    {
        fprintf( spdll, "%c", *(dllp + i) );
    }

    fclose( spdll );

    return 0;
}
