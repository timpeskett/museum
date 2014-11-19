#include <windows.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480
#define PI (3.1415926)
#define RAD (PI/180)
#define ANGLE 30
#define DIVISOR 2

void DrawTree( HDC dc, int x, int y, int len, int angle, int recnum )
{
    int x2, y2;
    int langle;

    MoveToEx( dc, x, y, NULL );

    langle = angle % 90;

    if( angle / 90 == 1 || angle / 90 == 3 )
    {
        langle = 90 - langle;
    }

    x2 = x + (int)(( angle < 180 ) ? len * sin( langle * RAD ) : -(len * sin( langle * RAD) ));
    y2 = y + (int)(( angle > 90 && angle < 270 ) ? len * cos( langle * RAD ) : -(len * cos( langle * RAD) ));


    LineTo( dc, x2, y2 );

    if( recnum < 8 )
    {
        DrawTree( dc, x2, y2, len / DIVISOR, (angle - ANGLE < 0) ? 360 + (angle - ANGLE) : angle - ANGLE, recnum + 1 );
        DrawTree( dc, x2, y2, len / DIVISOR, (angle - ANGLE / 2 < 0) ? 360 + (angle - ANGLE / 2) : angle - ANGLE / 2, recnum + 1 );
        DrawTree( dc, x2, y2, len / DIVISOR, (angle + ANGLE / 2 > 360) ? angle + ANGLE / 2 - 360 : angle + ANGLE / 2, recnum + 1 );
        DrawTree( dc, x2, y2, len / DIVISOR, angle, recnum + 1 );
        DrawTree( dc, x2, y2, len / DIVISOR, (angle + ANGLE > 360) ? angle + ANGLE - 360 : angle + ANGLE, recnum + 1 );
    }

    return;
}




LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    HPEN pen, old;

    switch( msg )
    {
        case WM_CREATE:
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            pen = CreatePen( PS_SOLID, 0, 0xFF0000 );
            old = (HPEN)SelectObject( dc, pen );

            DrawTree( dc, WIDTH / 2, HEIGHT, HEIGHT / DIVISOR, 0, 0 );

            SelectObject( dc, old );

            DeleteObject( pen );

            EndPaint( wnd, &ps );
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }
    return 0;
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX wc;
    HWND wnd;
    MSG msg;

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.cbWndExtra = wc.cbClsExtra = 0;
    wc.style = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TRClass";
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "could not register cloass", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "TRClass", "Tree Fractal", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + GetSystemMetrics( SM_CXBORDER) * 2, HEIGHT + GetSystemMetrics( SM_CYBORDER ) + GetSystemMetrics( SM_CYCAPTION ), NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "error", MB_OK );
        UnregisterClass( "TRClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "TRClass", hInst );

    return 0;
}
