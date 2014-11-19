#include <windows.h>
#include <stdio.h>
#include <string.h>

#define NUMWIDTH 30
#define NUMHEIGHT 30

#define SP_NUMCLICK 10000
#define SP_CREATEWINDOWS 10001

#define abs(x) ((x < 0)?-(x):(x))

#define CHILDID 10002

unsigned char matrix[4][4];


LRESULT CALLBACK NumProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    char string[2];
    HDC dc;
    PAINTSTRUCT ps;
    switch( msg )
    {
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            Rectangle( dc, 0, 0, NUMWIDTH, NUMHEIGHT );
            sprintf( string, "%d", GetDlgCtrlID( wnd ) );
            TextOut( dc, 1, 1, string, strlen( string ) );

            EndPaint( wnd, &ps );
            break;
        case WM_LBUTTONUP:
            SendMessage( GetParent( wnd ), SP_NUMCLICK, 0, (LPARAM)GetDlgCtrlID( wnd ) );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
    }
    return 0;
}

void GetEmptyRect( RECT *rect )
{
    int i, j;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            if( matrix[i][j] == 0 )
            {
                rect->left = i * NUMWIDTH;
                rect->right = rect->left + NUMWIDTH;
                rect->top = j * NUMHEIGHT;
                rect->bottom = rect->top + NUMHEIGHT;
            }
        }
    }
}

int IsNumAdjacentEmpty( int num )
{
    int x, y, i, j, a, b;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            if( matrix[i][j] == 0 )
            {
                a = i;
                b = j;
            }
            else if( matrix[i][j] == num )
            {
                x = i;
                y = j;
            }
        }
    }

    if( abs( x - a ) + abs( y - b ) < 2 )
    {
        return 1;
    }

    return 0;
}

void SwapWithEmpty( int num )
{
    int x, y, i, j, a, b;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            if( matrix[i][j] == 0 )
            {
                a = i;
                b = j;
            }
            else if( matrix[i][j] == num )
            {
                x = i;
                y = j;
            }
        }
    }

    matrix[a][b] = num;
    matrix[x][y] = 0;
}


LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    int i;
    RECT rect;

    switch( msg )
    {
        case WM_CREATE:
            SendMessage( wnd, SP_CREATEWINDOWS, 0, 0 );
            break;
        case SP_CREATEWINDOWS:
            for( i = 1; i < 16; i++ )
            {
                matrix[(i - 1) % 4][(i-1) / 4] = i;
                if( CreateWindowEx( 0, "NUMCLASS", " ", WS_VISIBLE | WS_CHILD, (i - 1) * NUMWIDTH % (NUMWIDTH * 4), (i - 1) * NUMWIDTH / (NUMWIDTH * 4) * NUMHEIGHT, NUMWIDTH, NUMHEIGHT, wnd, (HMENU)i, (HINSTANCE)GetWindowLong( wnd, GWL_HINSTANCE ), NULL ) == NULL )
                {
                    MessageBox( NULL, "Couldn't create window", "Error", MB_OK );
                }
            }
            matrix[3][3] = 0;
            break;
        case SP_NUMCLICK:
            if( IsNumAdjacentEmpty( lParam ) )
            {
                GetEmptyRect( &rect );
                SwapWithEmpty( lParam );
                SetWindowPos( GetDlgItem( wnd, lParam ), NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW );
            }
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
    }

    return 0;
}



int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int CmdShow )
{
    HWND wnd, desk;
    MSG msg;
    WNDCLASSEX wc, nc;

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.style = 0;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_BACKGROUND );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SliderClass";
    wc.hIconSm = NULL;
    wc.lpfnWndProc = (WNDPROC)WndProc;

    nc.cbSize = sizeof( WNDCLASSEX );
    nc.cbClsExtra = nc.cbWndExtra = 0;
    nc.hInstance = hInst;
    nc.style = 0;
    nc.hIcon = NULL;
    nc.hCursor = LoadCursor( NULL, IDC_ARROW );
    nc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    nc.lpszMenuName = NULL;
    nc.lpszClassName = "NUMCLASS";
    nc.hIconSm = NULL;
    nc.lpfnWndProc = (WNDPROC)NumProc;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register window class", "Error", MB_OK );
        return 0;
    }

    if( !RegisterClassEx( &nc ) )
    {
        MessageBox( NULL, "Could not register num class", "Error", MB_OK );
        UnregisterClass( "SliderClass", hInst );
        return 0;
    }

    desk = FindWindowEx( FindWindowEx( FindWindowEx( NULL, NULL, "Progman", "Program Manager" ), NULL, "SHELLDLL_DefView", NULL ), NULL, "SysListView32", "FolderView" );

    if( desk == NULL )
    {
        MessageBox( NULL, "Desk is NULL", "error", MB_OK );
    }

    wnd = CreateWindowEx( WS_EX_TOOLWINDOW, "SliderClass", "Slider", WS_CAPTION | WS_SYSMENU | ((desk)?WS_CHILD:0), CW_USEDEFAULT, CW_USEDEFAULT, NUMWIDTH * 4 + GetSystemMetrics( SM_CXBORDER ) * 2, NUMHEIGHT * 4 + GetSystemMetrics( SM_CYSMCAPTION ) + GetSystemMetrics( SM_CYBORDER ), (desk)?desk:NULL, (HMENU)((desk)?CHILDID:NULL), hInst, NULL );

    if( wnd == NULL )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "SliderClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "SliderClass", hInst );

    return 0;
}
