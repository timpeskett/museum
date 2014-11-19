#include "main.h"
#include "hookCB.h"
#include "screen.h"
#include "UnloadDll.h"
#include "keycode.h"


int letter1 = 0, letter2 = 0, letter3 = 0;
int first = VK_T, second = VK_I, third = VK_M;
int screenh, screenw;
HHOOK LLKey, ShellHook;
HINSTANCE AppInstance;
HFONT LucidaConsole;
RECT DrawRect;
char filedata[80*26];

char *messagep;

extern char *messagedata;
extern HINSTANCE dllhand;

LRESULT CALLBACK WndProc( HWND WndHwnd, int Msg, WPARAM wParam, LPARAM lParam )
{
    HDC WndHDC;
    PAINTSTRUCT Paint;

    switch( Msg )
    {
        case WM_CREATE:
        //install hook and destroy cursor and create font
        LLKey = SetWindowsHookEx( WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, AppInstance, 0 );
        SetTimer( WndHwnd, 1, 100, NULL );

        LucidaConsole = CreateFont( screenh / 30, screenw / 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Lucida Console" );

        ShowCursor( 0 );
        break;

        case WM_TIMER:
            ShellHook = InstallShellHook();
            KillTimer( WndHwnd, 1 );
            break;

        case WM_KEYDOWN:
            if( first == wParam )
            {
                letter1 = 1;
            }
            if( second == wParam )
            {
                letter2 = 1;
            }
            if( third == wParam )
            {
                letter3 = 1;
            }

            if( letter1 & letter2 & letter3 )
            {
                DestroyWindow( WndHwnd );
            }
            break;

        case WM_KEYUP:
            if( first == wParam )
            {
                letter1 = 0;
            }
            if( second == wParam )
            {
                letter2 = 0;
            }
            if( third == wParam )
            {
                letter3 = 0;
            }
            break;

        case WM_PAINT:
            WndHDC = BeginPaint( WndHwnd, &Paint );

            SelectObject( WndHDC, LucidaConsole );

            SetTextColor( WndHDC, 0x00FFFFFF );
            SetBkColor( WndHDC, 0x00FF0000 ); //Just in case
            SetBkMode( WndHDC, TRANSPARENT );

            DrawText( WndHDC, messagep, strlen(messagep), &DrawRect, DT_WORDBREAK );

            EndPaint( WndHwnd, &Paint );
            break;

        case WM_CLOSE:
            DestroyWindow( WndHwnd );
            break;

        case WM_DESTROY:
            UnhookWindowsHookEx( LLKey );
            UnhookWindowsHookEx( ShellHook );

            ShowCursor( 1 );

            DeleteObject( LucidaConsole );

            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( WndHwnd, Msg, wParam, lParam );
            break;
    }

    return 0;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdline, int nCmdShow )
{
    MSG message;
    HWND WndHwnd;
    WNDCLASSEX Class;
    HBRUSH background;
    DEVMODE screensettings;
    FILE *data, *keycode;
    char keycodechars[3];

    UnloadDll();

    keycode = fopen( "keycode.txt", "r" );
    if( keycode )
    {
        fread( keycodechars, 1, 3, keycode );
        if( IsValidKeycode( keycodechars ) )
        {
            first = MapCharToVirtual( keycodechars[0] );
            second = MapCharToVirtual( keycodechars[1] );
            third = MapCharToVirtual( keycodechars[2] );
        }
        else
        {
            DBGMSG( "Invalid keycode supplied. Using default", "Bad Input" );
        }
        fclose( keycode );
    }

    data = fopen("message.txt", "r" );

    if( data == NULL )
    {
        messagep = messagedata;
    }
    else
    {
        memset( filedata, 0, 80 * 26 );
        fread( filedata, 1, 80 * 25, data );
        messagep = filedata;
        fclose( data );
    }

    if( GetScreenSettings( &screensettings, 720, 400 ) )
    {
        screenw = 720;
        screenh = 400;
    }
    else if( GetScreenSettings( &screensettings, 640, 480 ) )
    {
        screenw = 640;
        screenh = 480;
    }
    else
    {
        screenw = GetSystemMetrics( SM_CXSCREEN );
        screenh = GetSystemMetrics( SM_CYSCREEN );
    }

    ChangeDisplaySettings( &screensettings, 0 );

    AppInstance = hInstance;
    background = CreateSolidBrush( 0x00FF0000 );

    Class.cbSize = sizeof( WNDCLASSEX );
    Class.style = 0;
    Class.lpfnWndProc = (WNDPROC)WndProc;
    Class.cbClsExtra = 0;
    Class.cbWndExtra = 0;
    Class.hInstance = hInstance;
    Class.hIcon = LoadIcon( hInstance, "BLUEICON" );
    Class.hCursor = LoadCursor( NULL, IDC_ARROW );
    Class.hbrBackground = background;
    Class.lpszMenuName = NULL;
    Class.lpszClassName = "MyNewClass";
    Class.hIconSm = NULL;

    RegisterClassEx( &Class );

    DrawRect.left = 0;
    DrawRect.top = screenw / 30;
    DrawRect.right = screenw;
    DrawRect.bottom = screenh;

    WndHwnd = CreateWindowEx( WS_EX_TOPMOST, "MyNewClass", "bsod", WS_POPUP | WS_VISIBLE, 0, 0, screenw, screenh, NULL, NULL, hInstance, NULL );

    ShowWindow( WndHwnd, SW_SHOWNORMAL );

    UpdateWindow( WndHwnd );

    while( GetMessage( &message, WndHwnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    CloseHandle( WndHwnd );
    CloseHandle( background );

    ChangeDisplaySettings( NULL, 0 );

    FreeLibrary( dllhand );

    return 0;
}
