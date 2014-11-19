#include <windows.h>
#include <string.h>
#include <stdio.h>

#define Error(x) MessageBox(NULL,x,"Error",MB_OK)
#define WIDTH 75
#define HEIGHT 85
#define VX -8
#define RUNNUMSPRITES 7
#define TURNNUMSPRITES 8
#define RUN 1
#define TURN 2

struct PosWnd
{
    HWND hwnd;
    int area;
};

BOOL CALLBACK EnumWndProc( HWND hwnd, LPARAM lParam )
{
    char buffer[100];
    struct PosWnd* pw;
    RECT wndrect;
    int area;

    GetWindowText( hwnd, buffer, 99 );

    if( !strcmp( buffer, "FolderView" ) )
    {
        GetClassName( hwnd, buffer, 99 );
        if( !strcmp( buffer, "SysListView32" ) )
        {
            pw = (struct PosWnd*)lParam;
            GetWindowRect( hwnd, &wndrect );
            area = (wndrect.bottom-wndrect.top) * (wndrect.right-wndrect.left);
            if( area > pw->area )
            {
                pw->area = area;
                pw->hwnd = hwnd;
            }
        }
    }

    return 1;
}


HWND GetDeskWindow( void )
{
    struct PosWnd DeskWnd;

    DeskWnd.hwnd = 0;
    DeskWnd.area = 0;

    EnumChildWindows( GetDesktopWindow(), EnumWndProc, (LPARAM)&DeskWnd );

    return DeskWnd.hwnd;
}


void DrawTransBitmap( HDC dest, int x, int y, int w, int h, HDC src, int sx, int sy )
{
    int masky;

    masky = sy + HEIGHT * 2;

    BitBlt( dest, x, y, w, h, src, sx, masky, SRCAND ); //Needed for transparency

    BitBlt( dest, x, y, w, h, src, sx, sy, SRCPAINT );
}


void ReplaceOld( HDC draw, HDC backup, RECT POPc )
{
    BitBlt( draw, POPc.left, POPc.top, POPc.right-POPc.left, POPc.bottom - POPc.top, backup, 0, 0, SRCCOPY );
}


void MoveToNew( RECT *POPc, int vx )
{
    POPc->left += vx;
    POPc->right += vx;
}

void BackupNew( HDC draw, HDC backup, RECT POPc )
{
    BitBlt( backup, 0, 0, POPc.right-POPc.left, POPc.bottom - POPc.top, draw, POPc.left, POPc.top, SRCCOPY );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdArgs, int nCmdShow )
{
    HBITMAP runbm, bu, turnbm;
    HDC compat, draw, backup;
    HWND wnd;
    RECT POPc, wndrect;
    int vx = VX;
    int spritenum = 0, numsprites = RUNNUMSPRITES;
    int mode = RUN;
    int turnrow = 0;

    wnd = GetDeskWindow();

    if( wnd == 0 )
    {
        Error("Could not get handle to desktop window");
        return 0;
    }

    draw = GetWindowDC( wnd );

    if( draw == NULL )
    {
        Error("Could not get Device Context");
        return 0;
    }

    compat = CreateCompatibleDC( draw );
    backup = CreateCompatibleDC( draw );

    runbm = LoadBitmap( hInstance, "RunBMP" );
    turnbm = LoadBitmap( hInstance, "TurnBMP" );

    SelectObject( compat, runbm );

    GetWindowRect( wnd, &wndrect );

    POPc.left = (wndrect.right - wndrect.left) / 2;
    POPc.right = POPc.left + WIDTH;
    POPc.top = wndrect.bottom - HEIGHT;
    POPc.bottom = wndrect.bottom;

    bu = CreateCompatibleBitmap( draw, WIDTH, HEIGHT );

    SelectObject( backup, bu );

    BackupNew( draw, backup, POPc );

    ReleaseDC( wnd, draw );

    for(;;)
    {
        InvalidateRect( wnd, &POPc, 1 );
        draw = GetWindowDC( wnd );
        MoveToNew( &POPc, vx );
        BackupNew( draw, backup, POPc );
        DrawTransBitmap( draw, POPc.left, POPc.top, WIDTH, HEIGHT, compat, spritenum * WIDTH, (mode == RUN) ? ((vx < 0) ? 0 * HEIGHT : 1 * HEIGHT) : (turnrow * HEIGHT) );
        ReleaseDC( wnd, draw );
        if( mode == RUN )
        {
            spritenum += (vx < 8) ? 1 : -1;
        }
        else
        {
            spritenum += (turnrow) ? -1 : 1;
        }
        if( spritenum > numsprites )
        {
            spritenum = 0;
        }
        else if( spritenum < 0 )
        {
            spritenum = numsprites;
        }
        if( (POPc.left < wndrect.left + 20 && vx < 8) || (POPc.right > wndrect.right - 20 && vx > -8) )
        {
            if( mode == RUN )
            {
                mode = TURN;
                SelectObject( compat, turnbm );
                numsprites = TURNNUMSPRITES;
                spritenum = (POPc.left < wndrect.left + 50) ? 0 : TURNNUMSPRITES; //same as below applies
            }
            if( POPc.left < wndrect.left + 50 ) //doesn't matter what it is, just has to be somewhere near the above value
            {
                vx += 2;
                turnrow = 0;
            }
            else
            {
                vx -= 2;
                turnrow = 1;
            }
        }
        else if( mode == TURN )
        {
            mode = RUN;
            SelectObject( compat, runbm );
            numsprites = RUNNUMSPRITES;
            spritenum = 0;
        }
        Sleep(80);
        if( GetAsyncKeyState( VK_MBUTTON ) & 1 )
        {
            InvalidateRect( wnd, &POPc, 1 );
            break;
        }
    }

    ReleaseDC( wnd, draw );

    DeleteObject( runbm );

    DeleteDC( compat );


    return 0;
}
