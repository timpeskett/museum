#include <windows.h>
#include <stdio.h>
#include "CC_Desktop.h"
#include "CC_Patch.h"
#include "CC_Shapes.h"
#include "CC_Slider.h"

#define SLIDER_BLUE 10000

struct CC_Desktop desk;
POINT old;

LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    RECT bounds;
    int pid;

    switch( msg )
    {
        case WM_CREATE:
            GetClientRect( wnd, &bounds );

            BelieveDesktop( &desk, &bounds, 0xFFFFFF );
            AddPatch( &desk, 10, 10, 100, 0, CC_Hexagon, NULL );
            AddPatch( &desk, 50, 70, 100, 0xFF0000, CC_Hexagon, NULL );
            AddPatch( &desk, 90, 130, 100, 0xFF00, CC_Hexagon, NULL );
            AddPatch( &desk, 130, 190, 100, 0xFF, CC_Hexagon, NULL );
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );
            DrawDesktop( &desk, dc ); //implement double buffering
            EndPaint( wnd, &ps );
            break;
        case WM_LBUTTONDOWN:
            old.x = LOWORD( lParam );
            old.y = HIWORD( lParam );

            BringToFront( &desk, GetPatchFromCoord( &desk, old.x, old.y ) );
            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            break;
        case WM_MOUSEMOVE:
            if( wParam & MK_LBUTTON )
            {
                if( (pid = GetPatchFromCoord( &desk, old.x, old.y )) != -1 )
                {
                    MovePatch( &desk, pid, LOWORD(lParam) - old.x, HIWORD(lParam) - old.y );
                }
                old.x = LOWORD(lParam);
                old.y = HIWORD(lParam);
                RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            }
            break;
        case WM_DESTROY:
            DestroyDesktop( &desk );
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break; //I know it isn't necessary, just doesn't feel the same without it
    }

    return 0;
}


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX wc;
    HWND wnd;
    MSG msg;
    struct SliderData sd;

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszClassName = "MyTestClass";
    wc.lpszMenuName = NULL;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.hIconSm = NULL;
    wc.hInstance = hInst;


    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register class", "Error", MB_OK );
        return 0;
    }

    if( !InitSliderControl( hInst, GetSysColorBrush( COLOR_WINDOW ) ) )
    {
        MessageBox( NULL, "Could not create slider control", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "MyTestClass", "MyTestWindow", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "MyTestClass", hInst );
        return 0;
    }

    sd.low = 0;
    sd.high = 0xFF0000;
    sd.step = 0x020000;
    sd.sliderpos = 10;

    CreateWindowEx( 0, "CC_Slider", "Blue", WS_CHILD | WS_VISIBLE, 20, 20, 128, 30, wnd, (HMENU)SLIDER_BLUE, hInst, &sd );

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "MyTestClass", hInst );

    return msg.wParam;
}
