#include <windows.h>
#include <stdio.h>
#include "slimes.h"
#include "ball.h"
#include "field.h"

#define WIDTH 640
#define HEIGHT 480

#define TIMER_ID 10000

#define GSM GetSystemMetrics

//debugging code, this should be rewritten once debugged

struct slime jorje, fure;
struct ball tennis;
struct field mcg;


void FieldHit( struct artifact *art, struct ball *iod, unsigned int msg )
{
    float oldvx, oldvy;
    RECT coords;
    int centx, centy;

    switch( msg )
    {
        case 1:
            printf("Ground Hit!\n");
            break;
        case 2:
            printf("Net hit!\n" );
            GetArtifactCoords( art, &coords );
            GetBallCentre( iod, &centx, &centy );
            if( centy < coords.top )
            {
                oldvy = GetBallYVelocity( iod );
                SetBallYVelocity( iod, -(oldvy) );
            }
            else
            {
                oldvx = GetBallXVelocity( iod );
                SetBallXVelocity( iod, -(oldvx) );
                if( oldvx > 0 )
                {
                    SetBallX( iod, coords.left - GetBallRadius( iod ) * 2 );
                }
                else
                {
                    SetBallX( iod, coords.right );
                }
            }
            break;
        default:
            break;
    }
}


LRESULT CALLBACK SlimeProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    RECT fuckyou;

    switch( msg )
    {
        case WM_CREATE:

            GetClientRect( wnd, &fuckyou );

            fuckyou.right /= 2;

            fuckyou.bottom -= 10;

            CreateSlime( &jorje, WIDTH / 4 - 40, HEIGHT - 40, 80, &fuckyou, RIGHT, 0xFF0000, "Jorje" );

            fuckyou.left = fuckyou.right;
            fuckyou.right *= 2;

            CreateSlime( &fure, WIDTH / 4 * 3 - 40, HEIGHT - 40, 80, &fuckyou, LEFT, 0xFF00, "Fure" );

            fuckyou.left = 0;
            fuckyou.bottom += 10;

            CreateBall( &tennis, 10, HEIGHT / 2, 25, &fuckyou, 0 );

            CreateField( &mcg, FieldHit );

            fuckyou.top = fuckyou.bottom - 10;

            AddArtifact( &mcg, "Ground", &fuckyou, 0, 1 );

            fuckyou.left = fuckyou.right / 2 - 5;
            fuckyou.right = fuckyou.left + 10;
            fuckyou.top -= 60;

            AddArtifact( &mcg, "Net", &fuckyou, 0, 2 );

            //SetBallXVelocity( &tennis, 5.0f );

            SetTimer( wnd, TIMER_ID, 30, NULL );
            break;
        case WM_TIMER:
            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            EraseSlime( &jorje, dc, 0xFFFFFF );
            EraseSlime( &fure, dc, 0xFFFFFF );
            EraseBall( &tennis, dc, 0xFFFFFF );

            UpdateSlime( &jorje, 9.98f * 3 );
            UpdateSlime( &fure, 9.98f * 3 );
            UpdateBall( &tennis, 9.98f * 3, 20.0f);

            CheckCollision( &jorje, &tennis, DefCD );
            CheckCollision( &fure, &tennis, DefCD );
            CheckField( &mcg, &tennis );

            DrawSlime( &jorje, dc, 0xFFFFFF );
            DrawSlime( &fure, dc, 0xFFFFFF );
            DrawBall( &tennis, dc, 0xFFFFFF );
            DrawField( &mcg, dc );

            EndPaint( wnd, &ps );
            break;
        case WM_KEYDOWN:
            switch( wParam )
            {
                case VK_LEFT:
                    SetSlimeXVelocity( &fure, -5.0f );
                    break;
                case VK_RIGHT:
                    SetSlimeXVelocity( &fure, 5.0f );
                    break;
                case VK_UP:
                    if( IsJumping( &fure ) == 0 )
                    {
                        SetSlimeYVelocity( &fure, 14.0f );
                    }
                    break;
                case VK_NUMPAD1:
                    SetSlimeXVelocity( &jorje, -5.0f );
                    break;
                case VK_NUMPAD3:
                    SetSlimeXVelocity( &jorje, 5.0f );
                    break;
                case VK_NUMPAD5:
                    if( IsJumping( &jorje ) == 0 )
                    {
                        SetSlimeYVelocity( &jorje, 14.0f );
                    }
                    break;
                default:
                    break;
            }
            break;
        case WM_KEYUP:
            switch( wParam )
            {
                case VK_LEFT:
                    if( GetSlimeXVelocity( &fure ) < 0 )
                    {
                        SetSlimeXVelocity( &fure, 0 );
                    }
                    break;
                case VK_RIGHT:
                    if( GetSlimeXVelocity( &fure ) > 0 )
                    {
                        SetSlimeXVelocity( &fure, 0 );
                    }
                    break;
                case VK_NUMPAD1:
                    if( GetSlimeXVelocity( &jorje ) < 0 )
                    {
                        SetSlimeXVelocity( &jorje, 0 );
                    }
                    break;
                case VK_NUMPAD3:
                    if( GetSlimeXVelocity( &jorje ) > 0 )
                    {
                        SetSlimeXVelocity( &jorje, 0 );
                    }
                    break;
                default:
                    break;
            }
            break;
        case WM_DESTROY:
            KillTimer( wnd, TIMER_ID );
            DeleteSlime( &jorje );
            DeleteSlime( &fure );
            DeleteBall( &tennis );
            DeleteField( &mcg );
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
    wc.lpfnWndProc = (WNDPROC)SlimeProc;
    wc.style = 0;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.hInstance = hInst;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SlimeClass";
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register class", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "SlimeClass", "Volleyball", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + GSM(SM_CXBORDER) * 2, HEIGHT + GSM(SM_CYCAPTION) + GSM(SM_CYBORDER), NULL, NULL, hInst, NULL );

    if( !wnd )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "SlimeClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "SlimeClass", hInst );

    return 0;
}
