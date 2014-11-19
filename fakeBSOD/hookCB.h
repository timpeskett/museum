#ifndef HOOKCB_H
#define HOOKCB_H

#include "main.h"
#include <windows.h>

extern HINSTANCE AppInstance;
HINSTANCE dllhand;

LRESULT CALLBACK LowLevelKeyboardProc( int, WPARAM, LPARAM );
HHOOK InstallShellHook( void );


#endif
