#include "hookCB.h"

#define VK_A 0x41


LRESULT CALLBACK LowLevelKeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
    KBDLLHOOKSTRUCT *pointer;

    pointer = (KBDLLHOOKSTRUCT*)lParam;

    switch( pointer->vkCode )
    {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_LWIN:
        case VK_RWIN:
        case VK_TAB:
        case VK_F4:
        case VK_LMENU:
        case VK_RMENU:
        case VK_ESCAPE:
        case VK_DELETE:
            pointer->flags &= 0xDF;
            pointer->vkCode = VK_A;
            pointer->scanCode = MapVirtualKey( VK_A, 0 );
            return 1;
            break;
        default:
            break;
    }

    return CallNextHookEx( NULL, code, wParam, lParam );
}


HHOOK InstallShellHook( void )
{
    FARPROC Procedure;

    dllhand = LoadLibrary( "ShellProc.dll" );

    if( dllhand == NULL )
    {
        DBGMSG("c", "d" );
    }

    Procedure = GetProcAddress( dllhand, "ShellProc@12" );

    if( Procedure == NULL )
    {
        DBGMSG( "e", "f" );
    }

    return SetWindowsHookEx( WH_SHELL, (HOOKPROC)Procedure, dllhand, 0 );
}
