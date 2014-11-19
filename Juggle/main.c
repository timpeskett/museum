#include <windows.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 400
#define HEIGHT 500
#define PWIDTH 50
#define BDIAMETER 20
#define PHEIGHT 15
#define SPACE 30

#define TIMER_ID 10000
#define JM_RESET 10001
#define JM_LOSE 10002

struct fvel
{
    float vx;
    float vy;
    int active;
};

typedef struct fvel FVEL;

RECT paddle;
RECT ball[10];
FVEL ballp[10];
int colours[10] = { 0xFF, 0xFF00, 0xFF0000, 0xFFFF, 0xFF00FF, 0xFFFF00, 0xFFFFFF, 0x12, 0x1234, 0x123456 };
int numballs = 3;
int starting = 1, countdown = 0, reload;
float gravity = 9.8f / 30.0f;
float startvel = 350.0f / 30.0f;
int score = 0;

LRESULT CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    int i;
    int s;
    int j;
    HDC dc;
    PAINTSTRUCT ps;
    HBRUSH brush;
    char scorebuffer[20];
    RECT textrect;

    switch( msg )
    {
        case WM_CREATE:
            ShowCursor( 0 );
            SetTimer( wnd, TIMER_ID, 30, NULL );

            textrect.left = 0;
            textrect.right = WIDTH;
            textrect.top = 0;
            textrect.bottom = HEIGHT / 5;

            srand( time(0) );

            SendMessage( wnd, JM_RESET, 0, 0 );
            break;
        case JM_RESET:
            paddle.left = (WIDTH - PWIDTH) / 2;
            paddle.right = paddle.left + PWIDTH;
            paddle.bottom = HEIGHT - SPACE;
            paddle.top = paddle.bottom - PHEIGHT;

            starting = 1;
            reload = 2500 / numballs / 30;
            countdown = reload;

            for( i = 0; i < numballs; i++ )
            {
                ball[i].left = paddle.left + (PWIDTH - BDIAMETER) / 2;
                ball[i].right = ball[i].left + BDIAMETER;
                ball[i].bottom = paddle.top;
                ball[i].top = ball[i].bottom - BDIAMETER;

                ballp[i].vx = 0.0f;
                ballp[i].vy = 0.0f;
                ballp[i].active = 0;
            }
            break;
        case JM_LOSE:
            score = (numballs - 3) * 200;
            SendMessage( wnd, JM_RESET, 0, 0 );
            break;
        case WM_TIMER:
            if( wParam != TIMER_ID )
            {
                break;
            }
            if( starting )
            {
                if( !countdown )
                {
                    for( s = 0; s < numballs; s++ )
                    {
                        if( !ballp[s].active )
                        {
                            break;
                        }
                    }
                    if( s == numballs )
                    {
                        starting = 0;
                    }
                    else
                    {
                        ballp[s].active = 1;
                        ballp[s].vy = startvel;
                        ballp[s].vx = (float)(rand() % 15) - 15 / 2;
                    }
                    countdown = reload;
                }
                countdown--;
            }

            for( s = 0; s < numballs; s++ )
            {
                if( ballp[s].active )
                {
                    ball[s].top -= (int)ballp[s].vy;
                    ball[s].bottom = ball[s].top + BDIAMETER;
                    ball[s].left += (int)ballp[s].vx;
                    ball[s].right = ball[s].left + BDIAMETER;
                    ballp[s].vy -= gravity;
                    if( ball[s].bottom >= paddle.top)
                    {
                        if(  paddle.left <= ball[s].left + (BDIAMETER / 2) && paddle.right >= ball[s].right - (BDIAMETER / 2) )
                        {
                            ballp[s].vy = startvel;
                            ballp[s].vx = (ball[s].left - paddle.left + (BDIAMETER / 2) - (PWIDTH / 2)) / 2;
                            score += 10;
                            if( score >= (numballs - 2) * 200 )
                            {
                                if( numballs >= 10 )
                                {
                                    dc = GetDC( wnd );
                                    TextOut( dc, WIDTH / 3, HEIGHT / 2, "You Won!", 8 );
                                    ReleaseDC( wnd, dc );
                                    Sleep(2000);
                                    SendMessage( wnd, WM_CLOSE, 0, 0 );
                                }
                                else
                                {
                                    numballs++;
                                    SendMessage( wnd, JM_RESET, 0, 0 );
                                }
                            }
                        }
                        if( ball[s].bottom >= HEIGHT )
                        {
                            SendMessage( wnd, JM_LOSE, 0, 0 );
                            break;
                        }
                    }
                    if( ball[s].left <= (0 - (int)ballp[s].vx) || ball[s].right >= (WIDTH - (int)ballp[s].vx) )
                    {
                        ballp[s].vx = -(ballp[s].vx);
                    }
                    if( ball[s].top <= 0 )
                    {
                        ballp[s].vy = ((int)ballp[s].vy < 0 ) ? ballp[s].vy : -(ballp[s].vy);
                    }
                }
            }

            InvalidateRgn( wnd, NULL, 1 );
            break;
        case WM_MOUSEMOVE:
            paddle.left = LOWORD(lParam) - PWIDTH / 2;
            paddle.right = paddle.left + PWIDTH;
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            sprintf( scorebuffer, "Score: %d", score );
            TextOut( dc, 0, 0, scorebuffer, strlen( scorebuffer) );


            brush = CreateSolidBrush( 0 );

            SelectObject( dc, brush );

            RoundRect( dc, paddle.left, paddle.top, paddle.right, paddle.bottom, PWIDTH / 4, PHEIGHT );

            DeleteObject( brush );

            for( j = 0; j < numballs; j++ )
            {
                brush = CreateSolidBrush( colours[j] );
                SelectObject( dc, brush );
                Ellipse( dc, ball[j].left, ball[j].top, ball[j].right, ball[j].bottom );
                DeleteObject( brush );
            }

            EndPaint( wnd, &ps );
            break;
        case WM_DESTROY:
            KillTimer( wnd, TIMER_ID );
            ShowCursor( 1 );
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
    HWND wnd;
    WNDCLASSEX wc;
    MSG msg;
    HBRUSH backbrush;

    backbrush = CreateHatchBrush( HS_CROSS, 0x654321);

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( hInst, "MyIcon" );
    wc.hbrBackground = backbrush;
    wc.hInstance = hInst;
    wc.lpszClassName = "JugglerClass";
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.hIconSm = NULL;
    wc.style = 0;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not Register Window Class", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "JugglerClass", "Juggler", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH + GetSystemMetrics( SM_CXBORDER ) * 2, HEIGHT + GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYBORDER), NULL, NULL, hInst, NULL );

    if( wnd == NULL )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "JugglerClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "JugglerClass", hInst );
    DeleteObject( backbrush );

    return 0;
}
