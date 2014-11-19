#include <windows.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 160
#define HEIGHT 100
#define PI 3.1415926
#define RAD (PI/180)

#define abs(x) (((x)<0)?-(x):(x))


char farray[10][10] ={\
{1,1,1,1,1,1,1,1,1,1},\
{1,0,0,0,0,0,0,0,0,1},\
{1,0,0,1,0,0,1,0,0,1},\
{1,0,0,0,0,0,0,0,0,1},\
{1,0,0,1,0,0,1,0,0,1},\
{1,0,0,0,0,0,0,0,0,1},\
{1,0,0,1,0,0,1,0,0,1},\
{1,0,0,0,0,0,0,0,0,1},\
{1,0,0,1,0,0,1,0,0,1},\
{1,1,1,1,1,1,1,1,1,1}};


struct Field
{
    char **map;
    int tx, ty;
    int twidth;
};

struct Player
{
    float x, y;
    int dir;
};

struct Camera
{
    int ppd;
    int fov;
    int hfov;
};

struct Player p1;
struct Field field;
struct Camera cam;


int IsWall( struct Field* f, int x, int y )
{
    if( *(*(f->map+y)+x) == 1 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int castHorzRay( struct Field *field, struct Player *player, int angle, int (*isWall)(struct Field*,int,int) )
{
    int x, y;
    float xrem;
    int cangle, hit, i;

    x = (int)player->x; //cast player pos to int
    y = (int)player->y;
    hit = 0;

     if( angle > 360 )
    {
        angle -= 360;
    }

    cangle = (int)angle;


    if( cangle == 90 || cangle == 270 )
    {
        return -1;
    }

    for( i = 0; i < cangle / 90; i++ )
    {
        angle -= 90;
    }

    if( cangle / 90 == 0 || cangle / 90 == 2 )
    {
        angle = 90.0f - angle;
    }

    while( x > 0 && y > 0 && x < field->tx * field->twidth && y < field->ty * field->twidth )
    {
        if( isWall( field, x / field->twidth, y / field->twidth ) )
        {
            hit = 1;
            break;
        }
        xrem = field->twidth - x % field->twidth;
        x += (cangle < 180) ? xrem : -(xrem);
        y += (cangle > 270 || cangle < 90) ? -(xrem * tan( angle * RAD )) : xrem * tan(angle * RAD);
    }

    if( !hit )
    {
        return -1;
    }

    x = abs(x - player->x);
    y = abs(y - player->y);

    return (int)sqrt( x * x + y * y );
}


void DrawLine( HDC dc, int x1, int y1, int x2, int y2, unsigned int col )
{
    POINT old;
    HPEN pold, pnew;

    pnew = CreatePen( PS_SOLID, 0, col );
    pold = SelectObject( dc, pnew );

    MoveToEx( dc, x1, y1, &old );
    LineTo( dc, x2, y2 );

    MoveToEx( dc, old.x, old.y, NULL );
    SelectObject( dc, pold );

    DeleteObject( pnew );
}


int castVertRay( struct Field *field, struct Player *player, int angle, int (*isWall)(struct Field*,int,int) )
{
    int x, y;
    float yrem;
    int cangle, hit, i;

    x = (int)player->x; //cast player pos to int
    y = (int)player->y;
    hit = 0;

    if( angle > 360 )
    {
        angle -= 360;
    }

    cangle = angle;


    if( cangle == 90 || cangle == 270 )
    {
        return -1;
    }

    for( i = 0; i < cangle / 90; i++ )
    {
        angle -= 90;
    }

    if( cangle / 90 == 0 || cangle / 90 == 2 )
    {
        angle = 90.0f - angle;
    }

    while( x > 0 && y > 0 && x < field->tx * field->twidth && y < field->ty * field->twidth )
    {
        if( isWall( field, x / field->twidth, y / field->twidth ) )
        {
            hit = 1;
            break;
        }
        yrem = field->twidth - y % field->twidth;
        x += (cangle < 180) ? yrem / tan( angle * RAD ) : -(yrem / tan(angle * RAD));
        y += (cangle > 270 || cangle < 90) ? -yrem : yrem;
    }

    if( !hit )
    {
        return -1;
    }

    x = abs(x - player->x);
    y = abs(y - player->y);
    return (int)sqrt( x * x + y * y );
}

void copyArray( char **ar1, char ar2[10][10] )
{
    int i, j;

    for( i = 0; i < 10; i++ )
    {
        for( j = 0; j < 10; j++ )
        {
            *(*(ar1 + j) + i) = ar2[i][j];
        }
    }
}

int projheight;

LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    int vmag, hmag, lheight, locang;
    int i;
    unsigned int col, scanline;
    int curdeg;
    float dx, dy;

    switch( msg )
    {
        case WM_CREATE:
            field.map = (char**)malloc( 10 * sizeof(char*) );
            for( i = 0; i < 10; i++ )
            {
                *(field.map + i) = malloc( 10 * sizeof(char) );
            }
            copyArray( field.map, farray );
            field.tx = field.ty = 10;
            field.twidth = 10;

            p1.x = p1.y = field.twidth + 1;
            p1.dir = 135;

            cam.fov = 60;
            cam.ppd = WIDTH / cam.fov;
            cam.fov = WIDTH / cam.ppd;
            cam.hfov = cam.fov / 2;

            projheight = field.twidth * 2 * (WIDTH / 2 * tan( cam.hfov * RAD ) );
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );
            scanline = 0;
            for( curdeg = p1.dir - cam.hfov; curdeg < p1.dir + cam.hfov; ++curdeg )
            {
                vmag = castVertRay( &field, &p1, curdeg, IsWall );
                hmag = castHorzRay( &field, &p1, curdeg, IsWall );

                printf("Vertmag: %d\nHorzmag: %d\n", vmag, hmag );

                if( vmag == -1 && hmag == -1 )
                {
                    continue;
                }
                else if( vmag == -1 || hmag == -1 )
                {
                    col = (vmag == -1) ? 0xFF00 : 0xFF;
                    vmag = (vmag == -1) ? hmag : vmag;
                }
                else
                {
                    col = (vmag > hmag) ? 0xFF00 : 0xFF;
                    vmag = (vmag > hmag) ? hmag : vmag;
                }

                lheight = projheight / vmag;

                for( i = 0; i < cam.ppd; i++ )
                {
                    DrawLine( dc, scanline + i, (HEIGHT - lheight) / 2, scanline + i, (HEIGHT + lheight) / 2, col );
                }
                scanline += cam.ppd;
            }
            EndPaint( wnd, &ps );
            break;
        case WM_KEYDOWN:
            switch( wParam )
            {
                case VK_LEFT:
                    p1.dir -= 3;
                    if( p1.dir < 3 )
                    {
                        p1.dir = 360;
                    }
                    printf("Angle:%d\n", p1.dir );
                    break;
                case VK_RIGHT:
                    p1.dir += 3;
                    if( p1.dir > 357 )
                    {
                        p1.dir = 0;
                    }
                    break;
                case VK_UP:
                    if( p1.dir / 90 == 0 || p1.dir / 90 == 2 )
                    {
                        locang = 90 - (p1.dir % 90);
                    }
                    else
                    {
                        locang = p1.dir % 90;
                    }
                    dx = (p1.dir < 180) ? cos( locang * RAD ) : -(cos( locang * RAD));
                    dy = (p1.dir > 270 || p1.dir < 90) ? -(sin( locang * RAD )) : sin( locang * RAD );
                    if( !IsWall( &field, (p1.x + dx) / field.twidth, (p1.y + dy) / field.twidth))
                    {
                        p1.x += dx;
                        p1.y += dy;
                    }
                    break;
                default:
                    break;
            }
            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE );
            break;
        case WM_DESTROY:
            free( field.map );
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }
    return 0;
}


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int CmdShow )
{
    MSG msg;
    WNDCLASSEX wc;
    HWND wnd;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = hInst;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.style = 0;
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszClassName = "RayCastClass";
    wc.lpszMenuName = NULL;
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register window class", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "RayCastClass", "RayCast", WS_SYSMENU | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + 2 * GetSystemMetrics( SM_CXBORDER ), HEIGHT + GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYBORDER ), NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "error", MB_OK );
        UnregisterClass( "RayCastClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "RayCastClass", hInst );

    return 0;
}
