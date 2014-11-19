/* Colour Chooser Application. Created for Ben Szabadics by Tim Peskett.*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CC_Desktop.h"
#include "CC_Shapes.h"

#define GSM GetSystemMetrics
#define BORDERX GSM( SM_CXBORDER )
#define BORDERY GSM( SM_CYBORDER )
#define CAPTIONY GSM( SM_CYCAPTION )
#define WIDTH 640
#define HEIGHT 480

#define CC_SETBOUNDS 10000
#define CC_SETPALETTESIZE 10001
#define CC_CREATEPALETTE 10002
#define CC_CREATESELECTION 10003
#define CC_SETSELECTIONSIZE 10004

struct CC_Desktop pal, sel;

extern int numcols;

LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    RECT cr, ncr;
    int i, minpid;

    switch( msg )
    {
        case WM_CREATE:
            memset( &cr, 0, sizeof( RECT ) );
            BelieveDesktop( &pal, &cr, 0 );
            BelieveDesktop( &sel, &cr, 0 );

            SendMessage( wnd, CC_SETBOUNDS, 0, 0 );
            SendMessage( wnd, CC_CREATEPALETTE, 0, 0 );
            SendMessage( wnd, CC_SETPALETTESIZE, 0, 0 );
            SendMessage( wnd, CC_CREATESELECTION, 0, 0 );
            SendMessage( wnd, CC_SETSELECTIONSIZE, 0, 0 );

            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE );
            break;
        case CC_SETBOUNDS:
            GetClientRect( wnd, &cr );

            ncr.left = cr.left + (cr.right - cr.left) / 3;
            ncr.right = cr.right;
            ncr.top = cr.top;
            ncr.bottom = cr.bottom;

            ChangeBounds( &pal, &ncr );

            ncr.right = ncr.left;
            ncr.left = cr.left;

            ChangeBounds( &sel, &ncr );
            break;
        case CC_CREATEPALETTE:
            srand( time(0) );
            for( i = 0; i < numcols; i++ )
            {
                AddPatch( &pal, 0, 0, 0, 0, ((rand() & 0xFF) << 16) | ((rand() & 0xFF) << 8) | (rand() & 0xFF), CC_Rectangle, NULL ); //replace with some predetermined method
            }
            break;
        case CC_SETPALETTESIZE:
            memcpy( &cr, GetDesktopBounds( &pal ), sizeof( RECT ) );

            minpid = GetMinPID( &pal );

            for( i = minpid; i <= GetMaxPID( &pal ); i++ )
            {
                if( !((i - minpid) / 8) )
                {
                    ncr.left = cr.left + (cr.right - cr.left) / 8 * (i - minpid);
                    ncr.top = cr.top;
                    ncr.bottom = ncr.top + (cr.bottom - cr.top) / 2;
                }
                else
                {
                    ncr.left = cr.left + (cr.right - cr.left) / 8 * (i - minpid - 8);
                    ncr.top = cr.top + (cr.bottom - cr.top) / 2;
                    ncr.bottom = cr.bottom;
                }

                ncr.right = ncr.left + (cr.right - cr.left) / 8;

                SetPatchPos( &pal, i, ncr.left, ncr.top, ncr.right - ncr.left, ncr.bottom - ncr.top );
            }
            break;
        case CC_CREATESELECTION:
            AddPatch( &sel, 0, 0, 0, 0, 0xABABAB, CC_Rectangle, NULL ); //Don't ask
            AddPatch( &sel, 0, 0, 0, 0, 0xABABAB, CC_Rectangle, NULL );
            AddPatch( &sel, 0, 0, 0, 0, 0xABABAB, CC_Rectangle, NULL );
            break;
        case CC_SETSELECTIONSIZE:
            memcpy( &cr, GetDesktopBounds( &sel ), sizeof( RECT ) );

            ncr.left = cr.left + (cr.right - cr.left) / 2 - (cr.right - cr.left) / 4;
            ncr.right = ncr.left + (cr.right - cr.left) / 2;

            minpid = GetMinPID( &sel );

            for( i = minpid; i <= GetMaxPID( &sel ); i++ )
            {
                ncr.top = cr.top + (cr.bottom - cr.top) / 5 * (i - minpid + 1);
                ncr.bottom = ncr.top + (cr.bottom - cr.top) / 5;
                SetPatchPos( &sel, i, ncr.left, ncr.top, ncr.right - ncr.left, ncr.bottom - ncr.top );
            }
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );
            DrawDesktop( &pal, dc );
            DrawDesktop( &sel, dc );
            EndPaint( wnd, &ps );
            break;
        case WM_LBUTTONDOWN:
            minpid = GetPatchFromCoord( &pal, LOWORD( lParam ), HIWORD( lParam ) );

            if( minpid != -1 )
            {
                SetPatchCol( &sel, GetFirstPatchPID( &sel ), GetPatchCol( &pal, minpid ) );
                BringToFront( &sel, GetFirstPatchPID( &sel ) );
            }
            dc = GetDC( wnd );
            DrawDesktop( &sel, dc );
            ReleaseDC( wnd, dc );
            break;
        case WM_SIZE:
            if( !(wParam & SIZE_MAXIMIZED) )
            {
                break;
            }
        case WM_EXITSIZEMOVE:
            SendMessage( wnd, CC_SETBOUNDS, 0, 0 );
            SendMessage( wnd, CC_SETPALETTESIZE, 0, 0 );
            SendMessage( wnd, CC_SETSELECTIONSIZE, 0, 0 );
            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            break;
        case WM_DESTROY:
            DestroyDesktop( &sel );
            DestroyDesktop( &pal );
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }
    return 0;
}


/*TO an uneducated person, this function may look copied. It is noteworthy that this function is the
skeleton windows code and is present in almost all Win32 GUI projects,looking almost the same as this.
This was handwritten though and as such there are subtle differences between this and similar code.*/
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX wc;
    HWND wnd;
    MSG msg;
    HBRUSH bg;

    bg = CreateSolidBrush( 0 );

    if( !bg )
    {
        bg = GetSysColorBrush( COLOR_DESKTOP );
    }

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = 0;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = bg;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SzabaClass";
    wc.hIconSm = NULL;
    wc.hInstance = hInst;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register class","Error", MB_OK );
        DeleteObject( bg );
        return 0;
    }

    wnd = CreateWindowEx( 0, "SzabaClass", "Bonza Barry's Bonus Bargain Bogan Barn", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + 2 * BORDERX, HEIGHT + CAPTIONY + BORDERY, NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "error", MB_OK );
        UnregisterClass( "SzabaClass", hInst );
        DeleteObject( bg );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "SzabaClass", hInst );
    DeleteObject( bg );

    return msg.wParam;
}
