#include <windows.h>
#include <string.h>

#define CIRCLE 1
#define CROSS 2

#define PLAYERMOVE 10000
#define AIMOVE 10001

#define GAME 0
#define END 1

#define absol(x) ((x<0)?-(x):(x))

int board[3][3];
HPEN blackpen;
int state = 0;
int WIDTH = 500, HEIGHT = 500;

void DrawCell( HWND wnd, int cellx, int celly )
{
    HDC dc;
    int x, y;

    x = cellx * (WIDTH / 3) + 10;
    y = celly * (HEIGHT / 3) + 10;

    dc = GetDC( wnd );

    SelectObject( dc, blackpen );
    SelectObject( dc, GetSysColorBrush( COLOR_WINDOW ) );

    if( board[cellx][celly] == CIRCLE )
    {
        Ellipse( dc, x, y, (cellx + 1) * (WIDTH / 3) - 10, (celly + 1) * (HEIGHT / 3) - 10 );
    }
    else if( board[cellx][celly] == CROSS )
    {
        MoveToEx( dc, x, y, NULL );
        LineTo( dc, (cellx + 1) * (WIDTH / 3) - 10, (celly + 1) * (HEIGHT / 3) - 10 );
        MoveToEx( dc, (cellx + 1) * (WIDTH / 3) - 10, y, NULL );
        LineTo( dc, x, (celly + 1) * (HEIGHT / 3) - 10 );
    }

    ReleaseDC( wnd, dc );
}


int TestLines( int sign, int x, int y, int level, int dirx, int diry )
{
    if( board[x][y] != sign )
    {
        return 0;
    }

    switch( level )
    {
        case 1:
            dirx = (x == 1) ? 0 : -(x - 1);
            if( TestLines( sign, x, y, level + 1, dirx, diry ))
            {
                return 1;
            }
            diry = (y == 1) ? 0 : -(y - 1);
            if( TestLines( sign, x, y, level + 1, dirx, diry ))
            {
                return 1;
            }
            dirx = 0;
            if( TestLines( sign, x, y, level + 1, dirx, diry ))
            {
                return 1;
            }
            return 0;
            break;
        case 2:
            x += dirx;
            y += diry;
            if( !(dirx | diry) )
            {
                return 0;
            }
            if( board[x][y] != sign )
            {
                return 0;
            }
            if( TestLines( sign, x, y, level + 1, dirx, diry ))
            {
                return 1;
            }
            return 0;
            break;
        case 3:
            x += dirx;
            y += diry;

            if( board[x][y] == sign )
            {
                return 1;
            }
            else
            {
                return 0;
            }
            break;
        default:
            MessageBox( NULL, "Something went wrong in recursive linechecking function", "Error", MB_OK );
            break;
    }

    return 0;
}



int HasWon( int sign )
{
    int i, j;

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( i == 1 && j == 1 )
            {
                continue;
            }
            if( board[i][j] == sign )
            {
                if( TestLines( sign, i, j, 1, 0, 0 ) )
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}


int HasPlayerWon( void )
{
    return HasWon( CROSS );
}

int HasAIWon( void )
{
    return HasWon( CIRCLE );
}


int BlockMove( int *cellx, int *celly )
{
    int i, j;

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( !board[i][j] )
            {
                board[i][j] = CROSS;
                if( HasPlayerWon() )
                {
                    board[i][j] = 0;
                    *cellx = i;
                    *celly = j;
                    return 1;
                }
                board[i][j] = 0;
            }
        }
    }
    return 0;
}


int IsSignAdjacent( int x, int y, int sign )
{
    int i, j;
    int num = 0;

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( board[i][j] == sign )
            {
                if( i == x && j == y )
                {
                    continue;
                }
                if( absol(x - i) < 2 && absol(y - j) < 2 )
                {
                     num++;
                }
            }
        }
    }
    return num;
}


void DecideMove( int *cellx, int *celly )
{
    int i, j;
    int num, max = 0;

    if( !board[1][1] ) //go middle first
    {
        *cellx = *celly = 1;
        return;
    }

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( board[i][j] == 0 )
            {
                if( (num = IsSignAdjacent( i, j, CROSS )) )
                {
                    if( num > max )
                    {
                        max = num;
                        *cellx = i;
                        *celly = j;
                    }
                }
            }
        }
    }
    if( max )
    {
        return;
    }

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( board[i][j] == 0 )
            {
                *cellx = i;
                *celly = j;
                return;
            }
        }
    }
}


int FinishMove( int *cellx, int *celly )
{
    int i, j;

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( board[i][j] == 0 )
            {
                board[i][j] = CIRCLE;
                if( HasAIWon() )
                {
                    *cellx = i;
                    *celly = j;
                    board[i][j] = 0;
                    return 1;
                }
                board[i][j] = 0;
            }
        }
    }
    return 0;
}


void GetNextMove( int *cellx, int *celly )
{
    if( FinishMove( cellx, celly ) )
    {
        return;
    }
    else if( BlockMove( cellx, celly ) )
    {
        return;
    }
    else
    {
        DecideMove( cellx, celly );
    }
}


void DrawGrid( HWND wnd )
{
    HDC dc;

    dc = GetDC( wnd );

    SelectObject( dc, blackpen );

    MoveToEx( dc, WIDTH / 3, 0, NULL );
    LineTo( dc, WIDTH / 3, HEIGHT );
    MoveToEx( dc, WIDTH / 3 * 2, 0, NULL );
    LineTo( dc, WIDTH / 3 * 2, HEIGHT );

    MoveToEx( dc, 0, HEIGHT / 3, NULL );
    LineTo( dc, WIDTH, HEIGHT / 3 );
    MoveToEx( dc, 0, HEIGHT / 3 * 2, NULL );
    LineTo( dc, WIDTH, HEIGHT / 3 * 2 );

    ReleaseDC( wnd, dc );
}


int BoardIsFull( void )
{
    int i, j;

    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            if( !board[i][j] )
            {
                return 0;
            }
        }
    }
    return 1;
}


LPARAM CALLBACK WndProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    int cellx, celly;
    PAINTSTRUCT ps;

    switch( msg )
    {
        case WM_CREATE:
            memset( board, 0, 9 * 4 );
            blackpen = CreatePen( PS_SOLID, 10, 0 );
            DrawGrid( wnd );
            break;
        case WM_PAINT:
            BeginPaint( wnd, &ps );
            DrawGrid( wnd );
            for( cellx = 0; cellx < 3; cellx++ )
            {
                for( celly = 0; celly < 3; celly++ )
                {
                    DrawCell( wnd, cellx, celly );
                }
            }
            EndPaint( wnd, &ps );
            break;
        case WM_DESTROY:
            DeleteObject( blackpen );
            PostQuitMessage( 0 );
            break;
        case WM_LBUTTONDOWN:
            if( state == END )
            {
                memset( board, 0, 9 * 4 );
                InvalidateRgn( wnd, NULL, 1 );
                state = GAME;
                break;
            }

            cellx = (lParam & 0xFFFF) / (WIDTH / 3);
            celly = (lParam >> 16) / (HEIGHT / 3);

            if(board[cellx][celly])
            {
                break;
            }

            SendMessage( wnd, PLAYERMOVE, 0, (cellx & 0xFFFF) | (celly << 16) );

            if( HasPlayerWon() )
            {
                MessageBox( NULL, "you have won", "Msg", MB_OK );
                state = END;
                //DisplayMessage( wnd, "You Have Won!" );
                break;
            }
            else if( BoardIsFull() )
            {
                MessageBox(NULL, "Draw", "Msg", MB_OK );
                state = END;
                break;
            }

            SendMessage( wnd, AIMOVE, 0, 0 );

            if( HasAIWon() )
            {
                MessageBox( NULL, "you lost", "msg", MB_OK );
                //DisplayMessage( wnd, "You Have Lost!" );
                state = END;
                break;
            }
            else if( BoardIsFull() )
            {
                MessageBox( NULL, "Draw", "Msg", MB_OK );
                state = END;
                break;
            }
            break;
        case PLAYERMOVE:
            cellx = lParam & 0xFFFF;
            celly = lParam >> 16;
            board[cellx][celly] = CROSS;
            DrawCell( wnd, cellx, celly );
            break;
        case AIMOVE:
            GetNextMove( &cellx, &celly );
            board[cellx][celly] = CIRCLE;
            DrawCell( wnd, cellx, celly );
            break;
        case WM_SIZE:
            WIDTH = lParam & 0xFFFF;
            HEIGHT = lParam >> 16;

            InvalidateRgn( wnd, NULL, 1 );
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

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_CROSS );
    wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TTTClass";
    wc.hIconSm = NULL;

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register class", "Error", MB_OK );
        return 0;
    }

    wnd = CreateWindowEx( 0, "TTTClass", "Tic Tac Toe", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, NULL, NULL, hInst, NULL );

    if( wnd == NULL )
    {
        MessageBox( NULL, "Could not create window", "Error", MB_OK );
        UnregisterClass( "TTTClass", hInst );
        return 0;
    }

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "TTTClass", hInst );

    return 0;
}
