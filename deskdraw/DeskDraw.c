/*A desktop draw POC*/

#include <windows.h>
#include "DeskDraw.h"

int CurrentSelected = MOUSESELECT;

HINSTANCE AppInstance;
int MidPressed = NOTPRESSED;

HBITMAP Pens;
HBITMAP PensSel;
HCURSOR SystemCursor, CpySystemCursor;


void DrawButtons( HWND WndHwnd )
{
    HDC whDC, NewDC;

    whDC = GetDC( WndHwnd );
    NewDC = CreateCompatibleDC( whDC );

    SelectObject( NewDC, Pens );

    if( !BitBlt( whDC, 0, 0, 150, 50, NewDC, 0, 0, SRCCOPY ))
    {
        MessageBox( NULL, "Failed to Blit src to dest", "Failed", MB_OK );
    }

    SelectObject( NewDC, PensSel );

    BitBlt( whDC, CurrentSelected * 50, 0, 50, 50, NewDC, CurrentSelected * 50, 0, SRCCOPY );

    DeleteDC( NewDC );
    DeleteDC( whDC );
}

LRESULT CALLBACK WndProc( HWND WndHwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    static HDC disp;
    SHORT MidKeyState = 0;
    POINT mousepos;
    int penstyle, width;
    COLORREF PenCol;

    switch( Msg )
    {
        case WM_CREATE:
            SetTimer( WndHwnd, 1, 80, NULL );
            disp = CreateDC( "DISPLAY", NULL, NULL, NULL );

            Pens = LoadBitmap( AppInstance, "PENS1" );
            PensSel = LoadBitmap( AppInstance, "PENSPUSHED2" );

            if( Pens == NULL || PensSel == NULL )
            {
                MessageBox( NULL, "Could not load Bitmaps", "Failed", MB_OK );
                DestroyWindow( WndHwnd );
            }

            SystemCursor = LoadCursor( NULL, IDC_ARROW );
            CpySystemCursor = CopyCursor( SystemCursor );
            DestroyCursor( SystemCursor );
            break;

        case WM_TIMER:
            if( CurrentSelected != MOUSESELECT )
            {
                if( !MidPressed )
                {
                    GetCursorPos( &mousepos );
                    MoveToEx( disp, mousepos.x, mousepos.y, NULL );
                    DrawButtons( WndHwnd );
                    UpdateWindow( WndHwnd );
                }
                if( (MidKeyState = GetAsyncKeyState( VK_MBUTTON )) != 0 )
                {
                    MidPressed = PRESSED;
                    if( MidKeyState & 1 )
                    {
                        //If Mid mouse pressed since last call
                        GetCursorPos( &mousepos );
                        MoveToEx( disp, mousepos.x, mousepos.y, NULL );
                    }

                    else
                    {
                        GetCursorPos( &mousepos );
                        LineTo( disp, mousepos.x, mousepos.y );
                    }
                }
            }
            break;

        case WM_LBUTTONDOWN:
            if( (lParam & 0xFFFF ) < 50 )
            {
                SystemCursor = CopyCursor( CpySystemCursor );
                SetSystemCursor( CpySystemCursor, OCR_NORMAL );
                CpySystemCursor = CopyCursor( SystemCursor );
                DestroyCursor( SystemCursor );

                PenCol = 0;
                CurrentSelected = MOUSESELECT;
            }
            else if( (lParam & 0xFFFF ) < 100 )
            {
                SetSystemCursor( LoadCursor( AppInstance, "BLUECUR" ), OCR_NORMAL );

                PenCol = (0xFF << 16);
                CurrentSelected = BLUESELECT;
            }
            else
            {
                SetSystemCursor( LoadCursor( AppInstance, "REDCUR" ), OCR_NORMAL );

                PenCol = 0xFF;
                CurrentSelected = REDSELECT;
            }

            penstyle = PS_SOLID;
            width = 2;

            SelectObject( disp, CreatePen( penstyle, width, PenCol ) );

            DrawButtons( WndHwnd );
            UpdateWindow( WndHwnd );
            break;

        case WM_MOVE:
            DrawButtons( WndHwnd );
            UpdateWindow( WndHwnd );
            break;

        case WM_CLOSE:
            DestroyWindow( WndHwnd );
            break;

        case WM_DESTROY:
            KillTimer( WndHwnd, 1 );
            DeleteDC( disp );
            DeleteObject( Pens );
            DeleteObject( PensSel );
            SetSystemCursor( CpySystemCursor, OCR_NORMAL );
            DestroyCursor( CpySystemCursor );
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( WndHwnd, Msg, wParam, lParam );
            break;
    }
    return 0;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX WndCls;
    HWND WndHwnd;
    MSG Msg;
    int captionh, borderw;

    AppInstance = hInstance;

    WndCls.cbSize = sizeof( WNDCLASSEX );
    WndCls.style = 0;
    WndCls.lpfnWndProc = WndProc;
    WndCls.cbClsExtra = 0;
    WndCls.cbWndExtra = 0;
    WndCls.hInstance = hInstance;
    WndCls.hIcon = LoadIcon( hInstance, "APPICON" );
    WndCls.hCursor = LoadCursor( NULL, IDC_ARROW );
    WndCls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WndCls.lpszMenuName = NULL;
    WndCls.lpszClassName = "NewClass";
    WndCls.hIconSm = NULL;

    if( !RegisterClassEx( &WndCls ) )
    {
        MessageBox( NULL, "Class registration failed", "Failed", MB_OK );
        return 0;
    }

    captionh = GetSystemMetrics( SM_CYCAPTION );
    borderw = GetSystemMetrics( SM_CXBORDER );

    WndHwnd = CreateWindowEx( WS_EX_TOPMOST, "NewClass", "ToolBox", WS_SYSMENU, 0, 0, borderw * 2 + 150, captionh + borderw * 2 + 50, NULL, NULL, hInstance, NULL );

    if( WndHwnd == NULL )
    {
        MessageBox( NULL, "Could not create window", "Failed", MB_OK );
        UnregisterClass( "NewClass", hInstance );
        return 0;
    }

    ShowWindow( WndHwnd, SW_SHOW );
    UpdateWindow( WndHwnd );

    while( GetMessage( &Msg, WndHwnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &Msg );
        DispatchMessage( &Msg );
    }
    UnregisterClass( "NewClass", hInstance );
    return 0;
}
