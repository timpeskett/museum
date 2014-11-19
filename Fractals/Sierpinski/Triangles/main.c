#include <windows.h>

#define WIDTH 640
#define HEIGHT 480


void DrawTriangle( HDC dc, int x1, int y1, int x2, int y2, int x3, int y3 )
{
    MoveToEx( dc, x1, y1, NULL );
    LineTo( dc, x2, y2 );
    LineTo( dc, x3, y3 );
    LineTo( dc, x1, y1 );
}


void DrawFractal( HDC dc, int x1, int y1, int x2, int y2, int x3, int y3, int rlev )
{
    int mx1, my1, mx2, my2, mx3, my3;

    mx1 = x1 + (x2 - x1) / 2;
    my1 = y1 + (y2 - y1) / 2;
    mx2 = x2 + (x3 - x2) / 2;
    my2 = y2 + (y3 - y2) / 2;
    mx3 = x3 + (x1 - x3) / 2;
    my3 = y3 + (y1 - y3) / 2;

    DrawTriangle( dc, mx1, my1, mx2, my2, mx3, my3 );

    if( rlev < 8 )
    {
        DrawFractal( dc, x1, y1, mx1, my1, mx3, my3, rlev + 1 );
        DrawFractal( dc, mx1, my1, x2, y2, mx2, my2, rlev + 1 );
        DrawFractal( dc, mx3, my3, mx2, my2, x3, y3, rlev + 1 );
    }

    return;
}


int i1, j1, i2, j2, i3, j3;
int *xp, *yp;

LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    HPEN pen, old;
    int nx, ny;

    switch( msg )
    {
        case WM_CREATE:
            i1 = WIDTH / 2;
            j1 = 20;
            i2 = 0;
            j2 = HEIGHT - 20;
            i3 = WIDTH;
            j3 = HEIGHT - 20;
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            pen = CreatePen( PS_SOLID, 0, 0xFF0000 );
            old = (HPEN)SelectObject( dc, pen );

            DrawTriangle( dc, i1, j1, i2, j2, i3, j3 );

            DrawFractal( dc, i1, j1, i2, j2, i3, j3, 1 );

            SelectObject( dc, old );

            DeleteObject( pen );

            EndPaint( wnd, &ps );
            break;
        case WM_MOUSEMOVE:
            if( wParam & MK_LBUTTON )
            {
                if( xp && yp )
                {
                    *xp = LOWORD( lParam );
                    *yp = HIWORD( lParam );
                    RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE );
                }
            }
            break;
        case WM_LBUTTONDOWN:
            nx = LOWORD(lParam);
            ny = HIWORD(lParam);

            if( abs( nx - i1 ) < 15 && abs( ny - j1 ) < 15 )
            {
                xp = &i1;
                yp = &j1;
            }
            if( abs( nx - i2 ) < 15 && abs( ny - j2 ) < 15 )
            {
                xp = &i2;
                yp = &j2;
            }
            if( abs( nx - i3 ) < 15 && abs( ny - j3 ) < 15 )
            {
                xp = &i3;
                yp = &j3;
            }
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

    wc.cbSize = sizeof( wc );
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.style = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "FractalClass";
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not create class", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "FractalClass", "Sierpinski Triangle", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + GetSystemMetrics( SM_CXBORDER ) * 2, HEIGHT + GetSystemMetrics( SM_CYBORDER ) + GetSystemMetrics( SM_CYCAPTION ), NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "FractalClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0 , 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "FractalClass", hInst );

    return 0;
}
