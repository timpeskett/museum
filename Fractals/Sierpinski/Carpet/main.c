#include <windows.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 700

char *wnd_name = "Sierpinski's Carpet";


void DrawSCFractal( HDC dc, int x1, int y1, int x2, int y2, int lev )
{
    int rx, ry;
    int rw, rh;
    int i, j;
    int wid, hei;

    wid = x2 - x1;
    hei = y2 - y1;

    rw = wid / 3;
    rh = hei / 3;
    rx = x1 + (wid - rw) / 2;
    ry = y1 + (hei - rh) / 2;

    Rectangle( dc, rx, ry, rx + rw, ry + rh );

    if( lev < 8 )
    {
        for( i = 0; i < 3; i++ )
        {
            for( j = 0; j < 3; j++ )
            {
                if( !(j == 1 && i == 1) )
                {
                    DrawSCFractal( dc, x1 + rw * i, y1 + rh * j, x1 + rw * i + rw, y1 + rh * j + rh, lev + 1 );
                }
            }
        }
    }

    return;
}



LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    HBRUSH br, old;

    switch( msg )
    {
        case WM_CREATE:
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            br = CreateSolidBrush( 0 );

            old = (HBRUSH)SelectObject( dc, br );

            DrawSCFractal( dc, 0, 0, WIDTH, HEIGHT, 1 );

            SelectObject( dc, old );

            DeleteObject( br );

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

int WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX wc;
    HWND wnd;
    MSG msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.style = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "FractalClass";
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register class", "errpr", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "FractalClass", wnd_name, WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + GetSystemMetrics( SM_CXBORDER ) * 2, GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYBORDER ) + HEIGHT, NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "FractalClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "FractalClass", hInst );

    return msg.wParam;
}
