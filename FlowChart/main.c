#include <windows.h>
#include <stdio.h>

#define FCM_PRINT 9998
#define FCM_CREATEWINDOW 9999

#define CHILDWINDOW 10000
#define DECISION 10001
#define INOUT 10002
#define PROCESS 10003
#define TERMINAL 10004
#define PRINT 10005

#define TIMER_ID 10010

#define dbg MessageBox( NULL, "message", "Error", MB_OK )


int boxid = 11000;

struct box
{
    HWND hwnd;
    int type;
    char *text;
    RECT pos;
    unsigned int brushcol;
};

struct box Boxes[1000];
POINT oldcoords;


void SetWindowType( HWND wnd, int type )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            break;
        }
        if( Boxes[i].hwnd == 0 )
        {
            break;
        }
    }
    Boxes[i].hwnd = wnd;
    Boxes[i].type = type;
}

void SetWindowString( HWND wnd, char *string )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            break;
        }
        if( Boxes[i].hwnd == 0 )
        {
            break;
        }
    }
    Boxes[i].hwnd = wnd;
    if( Boxes[i].text != NULL )
    {
        free( Boxes[i].text );
        Boxes[i].text = NULL;
    }
    if( string != NULL )
    {
        Boxes[i].text = (char*)malloc( strlen( string ) + 1 );
        strcpy( Boxes[i].text, string );
    }
}

void DeleteAll( void )
{
    int i;

    for( i = 0; Boxes[i].hwnd; i++ )
    {
        Boxes[i].hwnd = 0;
        Boxes[i].type = 0;
        if( Boxes[i].text != NULL )
        {
            free( Boxes[i].text );
        }
    }
}

struct box* GetBox( HWND wnd )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            return (Boxes + i);
        }
    }
    return NULL;
}

int GetWindowType( HWND wnd )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            return Boxes[i].type;
        }
    }
    return 0;
}

unsigned int GetWindowBrushColour( HWND wnd )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            return Boxes[i].brushcol;
        }
    }
    return 0;
}


void SetWindowBrushColour( HWND wnd, unsigned int brushcol )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            Boxes[i].brushcol = brushcol;
            break;
        }
    }
}

char* GetWindowString( HWND wnd )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == wnd )
        {
            return Boxes[i].text;
        }
    }
    return NULL;
}


void DrawDecisionBox( HDC dc, RECT *pos )
{
    MoveToEx( dc, 0, (pos->bottom - pos->top) / 2, NULL );
    LineTo( dc, (pos->right - pos->left) / 2, 0 );
    LineTo( dc, pos->right - pos->left, (pos->bottom - pos->top) / 2 );
    LineTo( dc, (pos->right - pos->left) / 2, pos->bottom - pos->top - 1 );
    LineTo( dc, 0, (pos->bottom - pos->top ) / 2 );
}

void DrawIOBox( HDC dc, RECT *pos )
{
    MoveToEx( dc, 0, pos->bottom - pos->top - 1, NULL );
    LineTo( dc, (pos->right - pos->left) / 6, 0 );
    LineTo( dc, pos->right - pos->left, 0 );
    LineTo( dc, ((pos->right - pos->left) / 6) * 5, pos->bottom - pos->top - 1 );
    LineTo( dc, 0, pos->bottom - pos->top - 1 );
}

void DrawProcessBox( HDC dc, RECT *pos )
{
    Rectangle( dc, 0, 0, pos->right - pos->left, pos->bottom - pos->top );
}

void DrawTerminalBox( HDC dc, RECT *pos )
{
    RoundRect( dc, 0, 0, pos->right - pos->left, pos->bottom - pos->top, (pos->right - pos->left) / 5, pos->bottom - pos->top );
}

LRESULT CALLBACK BoxWindowProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    int type;
    struct box* mybox;
    HDC dc;
    PAINTSTRUCT ps;
    POINT mouse;
    char *text;
    char buffer[1000];
    SIZE textsize;
    HPEN pen;

    switch( msg )
    {
        case WM_CREATE:
            type = (int)((CREATESTRUCT*)lParam)->lpCreateParams;
            SetWindowType( wnd, type );
            mybox = GetBox( wnd );

            mybox->pos.left = ((CREATESTRUCT*)lParam)->x;
            mybox->pos.top = ((CREATESTRUCT*)lParam)->y;
            mybox->pos.right = mybox->pos.left + ((CREATESTRUCT*)lParam)->cx;
            mybox->pos.bottom = mybox->pos.top + ((CREATESTRUCT*)lParam)->cy;

            SetFocus( wnd );
            break;
        case WM_SETFOCUS:
            SetWindowBrushColour( wnd, 0xFF );
            InvalidateRgn( wnd, NULL, 1 );
            break;
        case WM_KILLFOCUS:
            SetWindowBrushColour( wnd, 0 );
            InvalidateRgn( wnd, NULL, 1 );
            break;
        case WM_PAINT:
            if( GetWindowString( wnd ) )
            {
                dc = GetDC( wnd );
                GetTextExtentPoint32( dc, GetWindowString( wnd ), strlen( GetWindowString( wnd )), &textsize );
                mybox = GetBox( wnd );
                mybox->pos.right = mybox->pos.left + textsize.cx + 50;
                mybox->pos.bottom = mybox->pos.top + textsize.cy + 50;
                SetWindowPos( wnd, NULL, 0, 0, textsize.cx + 50, textsize.cy + 50, SWP_NOZORDER | SWP_NOMOVE );
                ReleaseDC( wnd, dc );
            }

            dc = BeginPaint( wnd, &ps );
            mybox = GetBox( wnd );
            pen = CreatePen( PS_SOLID, 0, GetWindowBrushColour( wnd ) );
            SelectObject( dc, pen );
            switch( GetWindowType( wnd ) )
            {
                case DECISION:
                    DrawDecisionBox( dc, &(mybox->pos) );
                    if( GetWindowString( wnd ) )
                                        TextOut( dc, ((mybox->pos.right - mybox->pos.left) - textsize.cx) / 2, ((mybox->pos.bottom - mybox->pos.top) - textsize.cy) / 2, GetWindowString( wnd ), strlen( GetWindowString(wnd) ) );
                    break;
                case INOUT:
                    DrawIOBox( dc, &(mybox->pos) );
                    if( GetWindowString( wnd ) )
                                        TextOut( dc, ((mybox->pos.right - mybox->pos.left) - textsize.cx) / 2, ((mybox->pos.bottom - mybox->pos.top) - textsize.cy) / 2, GetWindowString( wnd ), strlen( GetWindowString(wnd) ) );
                    break;
                case PROCESS:
                    DrawProcessBox( dc, &(mybox->pos) );
                    if( GetWindowString( wnd ) )
                                        TextOut( dc, ((mybox->pos.right - mybox->pos.left) - textsize.cx) / 2, ((mybox->pos.bottom - mybox->pos.top) - textsize.cy) / 2, GetWindowString( wnd ), strlen( GetWindowString(wnd) ) );
                    break;
                case TERMINAL:
                    DrawTerminalBox( dc, &(mybox->pos) );
                    if( GetWindowString( wnd ) )
                                        TextOut( dc, ((mybox->pos.right - mybox->pos.left) - textsize.cx) / 2, ((mybox->pos.bottom - mybox->pos.top) - textsize.cy) / 2, GetWindowString( wnd ), strlen( GetWindowString(wnd) ) );
                    break;
                default:
                    MessageBox( NULL, "Unrecognised Type", "Error", MB_OK );
                    break;
            }

            DeleteObject( pen );

            EndPaint( wnd, &ps );
            break;
        case WM_LBUTTONDOWN:
            oldcoords.x = LOWORD(lParam);
            oldcoords.y = HIWORD(lParam);

            SetCapture(wnd);

            SetFocus( wnd );
            break;
        case WM_LBUTTONUP:
            ReleaseCapture();
            InvalidateRgn( wnd, NULL, 1 );
            break;
        case WM_MOUSEMOVE:
            if( wParam & MK_LBUTTON )
            {
                mouse.x = LOWORD(lParam);
                mouse.y = HIWORD(lParam);
                ClientToScreen( wnd, &mouse );
                ScreenToClient( GetParent(wnd), &mouse );
                if( mouse.x - oldcoords.x > 0 && mouse.x - oldcoords.x < 590 && mouse.y - oldcoords.y > 0 && mouse.y - oldcoords.y < 540)
                {
                    SetWindowPos( wnd, 0, mouse.x - oldcoords.x, mouse.y - oldcoords.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
                }
            }
            break;
        case WM_KEYDOWN:
            if( wParam == VK_DELETE )
            {
                DestroyWindow( wnd );
            }
            break;
        case WM_CHAR:
            text = GetWindowString( wnd );
            if( wParam == '\b' )
            {
                if( text != NULL )
                {
                    if( strlen( text ) <= 1 )
                    {
                        SetWindowString( wnd, NULL ); //frees memory for me <3
                    }
                    else
                    {
                        text[strlen(text)-1] = '\0';
                    }
                    InvalidateRgn( wnd, NULL, 1 );
                }
                break;
            }
            else if( wParam == '\n' )
            {
                break;
            }
            else if( GetWindowString( wnd ) )
            {
                sprintf( buffer, "%s%c", text, wParam );
            }
            else
            {
                sprintf(buffer, "%c", wParam );
            }
            SetWindowString( wnd, buffer );
            InvalidateRgn( wnd, NULL, 1 );
            break;
        case WM_DESTROY:
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }

    return 0;
}


void DrawBoxesToDC( int (*DrawCallback)(HWND, HDC), HDC dc )
{
    int i;

    for( i = 0; i < 1000; i++ )
    {
        if( Boxes[i].hwnd == 0 || !DrawCallback( Boxes[i].hwnd, dc ) )
        {
            break;
        }
    }
}


int BoxesEnum( HWND box, HDC dc )
{
    RECT wndrect;
    HDC boxdc;

    if( IsWindow( box ) )
    {
        GetWindowRect( box, &wndrect );
        boxdc = GetDC( box );
        BitBlt( dc, wndrect.left, wndrect.top, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top, boxdc, 0, 0, SRCCOPY );
        ReleaseDC( box, boxdc );
    }
    return 1;
}


LRESULT CALLBACK ChildWindowProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    PRINTDLG pd;
    HDC cmp;
    HBITMAP toprint;
    RECT wndrect;
    DOCINFO di;

    switch( msg )
    {
        case WM_CREATE:
            break;
        case FCM_CREATEWINDOW:
            CreateWindowEx( 0, "BoxClass", "BoxWnd", WS_VISIBLE | WS_CHILD, 200, 200, 100, 50, wnd, (HMENU)boxid++, (HINSTANCE)GetWindowLong( wnd, GWL_HINSTANCE), (void*)lParam );
            break;
        case FCM_PRINT:
            pd.lStructSize = sizeof(PRINTDLG);
            pd.hwndOwner = wnd;
            pd.hDevMode = NULL;
            pd.hDevNames = NULL;
            pd.Flags = PD_ALLPAGES | PD_NOPAGENUMS | PD_RETURNDC | PD_NOSELECTION;
            pd.nFromPage = 0;
            pd.nToPage = 0;
            pd.nMinPage = 0;
            pd.nMaxPage = 0;
            pd.nCopies = 1;
            pd.lpfnPrintHook = NULL;
            pd.lpfnSetupHook = NULL;
            pd.lpPrintTemplateName = NULL;
            pd.lpSetupTemplateName = NULL;
            pd.hPrintTemplate = NULL;
            pd.hSetupTemplate = NULL;

            if( PrintDlg( &pd ) )
            {
                if( pd.hDC == NULL )
                {
                    break;
                }
                cmp = CreateCompatibleDC( pd.hDC );
                GetWindowRect( wnd, &wndrect );
                toprint = CreateCompatibleBitmap( pd.hDC, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top );
                SelectObject( cmp, toprint );
                DrawBoxesToDC( BoxesEnum, cmp );
                di.cbSize = sizeof( DOCINFO );
                StartDoc( pd.hDC, &di );
                StartPage( pd.hDC );
                BitBlt( pd.hDC, 0, 0, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top, cmp, 0, 0, SRCCOPY );
                EndPage( pd.hDC );
                EndDoc( pd.hDC );
                DeleteObject( toprint );
                DeleteDC( cmp );
            }
            break;
        case WM_LBUTTONDOWN:
            SetFocus( wnd );
            break;
        case WM_DESTROY:
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }

    return 0;
}


LRESULT CALLBACK ParentWindowProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_CREATE:
            memset( Boxes, 0, 1000 * sizeof( struct box ) );
            break;
        case WM_COMMAND:
            if( LOWORD(wParam) == PRINT )
            {
                if( HIWORD(wParam) == BN_CLICKED )
                {
                    SendDlgItemMessage( wnd, CHILDWINDOW, FCM_PRINT, 0, 0 );
                }
            }
            else if( HIWORD(wParam) == BN_CLICKED )
            {
                SendDlgItemMessage( wnd, CHILDWINDOW, FCM_CREATEWINDOW, 0, (LPARAM)LOWORD(wParam) );
            }
            break;
        case WM_LBUTTONDOWN:
            SetFocus( wnd );
            break;
        case WM_CLOSE:
            DestroyWindow( wnd );
            break;
        case WM_DESTROY:
            DeleteAll();
            PostQuitMessage( 0 );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }
    return 0;
}

HBITMAP bitmap;

LRESULT CALLBACK SplashProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc, cdc;
    PAINTSTRUCT ps;

    switch( msg )
    {
        case WM_CREATE:
            SetTimer( wnd, TIMER_ID, 3000, NULL );
            bitmap = LoadBitmap( (HINSTANCE)GetWindowLong( wnd, GWL_HINSTANCE ), "SPLASH" );
            SetForegroundWindow( wnd );
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );
            cdc = CreateCompatibleDC( dc );
            SelectObject( cdc, bitmap );
            BitBlt( dc, 0, 0, 500, 275, cdc, 0, 0, SRCCOPY );
            DeleteDC( cdc );
            EndPaint( wnd, &ps );
            break;
        case WM_KEYDOWN:
        case WM_TIMER:
            KillTimer( wnd, TIMER_ID );
            DestroyWindow( wnd );
            break;
        case WM_DESTROY:
            DeleteObject( bitmap );
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
            break;
    }
    return 0;
}



int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int nCmdShow )
{
    WNDCLASSEX wc, swc, bwc, spl;
    HWND wnd, splash;
    MSG msg;
    int scrx, scry;

    scrx = GetSystemMetrics( SM_CXSCREEN );
    scry = GetSystemMetrics( SM_CYSCREEN );

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = (WNDPROC)ParentWindowProc;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.style = 0;
    wc.hbrBackground = GetSysColorBrush( COLOR_INACTIVEBORDER );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "MainWindowClass";
    wc.hIconSm = NULL;
    wc.hInstance = hInst;

    swc.cbSize = sizeof( WNDCLASSEX );
    swc.cbClsExtra = swc.cbWndExtra = 0;
    swc.lpfnWndProc = (WNDPROC)ChildWindowProc;
    swc.hIcon = NULL;
    swc.hCursor = LoadCursor( NULL, IDC_ARROW );
    swc.style = 0;
    swc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    swc.lpszMenuName = NULL;
    swc.lpszClassName = "ChildWindowClass";
    swc.hIconSm = NULL;
    swc.hInstance = hInst;

    bwc.cbSize = sizeof( WNDCLASSEX );
    bwc.cbClsExtra = bwc.cbWndExtra = 0;
    bwc.lpfnWndProc = (WNDPROC)BoxWindowProc;
    bwc.hIcon = NULL;
    bwc.hCursor = LoadCursor( NULL, IDC_ARROW );
    bwc.style = 0;
    bwc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    bwc.lpszMenuName = NULL;
    bwc.lpszClassName = "BoxClass";
    bwc.hIconSm = NULL;
    bwc.hInstance = hInst;

    spl.cbSize = sizeof( WNDCLASSEX );
    spl.cbClsExtra = spl.cbWndExtra = 0;
    spl.lpfnWndProc = (WNDPROC)SplashProc;
    spl.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    spl.hCursor = LoadCursor( NULL, IDC_CROSS );
    spl.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
    spl.lpszMenuName = NULL;
    spl.lpszClassName = "Splash";
    spl.hIconSm = NULL;
    spl.hInstance = hInst;
    spl.style = 0;

    if( RegisterClassEx( &spl ) )
    {
        splash = CreateWindowEx( 0, "Splash", "FlowChart", WS_POPUP, (scrx - 500) / 2, (scry - 275) / 2, 500, 275, NULL, NULL, hInst, NULL );
        ShowWindow( splash, SW_SHOW );
        UpdateWindow( splash );
        while( GetMessage( &msg, splash, 0, 0 ) > 0 )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        UnregisterClass( "Splash", hInst );
    }

    if( !RegisterClassEx( &wc ) )
    {
        MessageBox( NULL, "Could not register main window class", "Error", MB_OK );
        return 0;
    }

    if( !RegisterClassEx( &swc ) )
    {
        MessageBox( NULL, "Could not register child window class", "Error", MB_OK );
        UnregisterClass( "MainWindowClass", hInst );
        return 0;
    }

    if( !RegisterClassEx( &bwc ) )
    {
        MessageBox( NULL, "Could not register button window class", "Error", MB_OK );
        UnregisterClass( "MainWindowClass", hInst );
        UnregisterClass( "ChildWindowClass", hInst );
        return 0;
    }

    wnd = CreateWindowEx( 0, "MainWindowClass", "FlowChart", WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInst, NULL );

    if( wnd == NULL )
    {
        MessageBox( NULL, "Could not create main window", "Error", MB_OK );
        return 0;
    }

    CreateWindowEx( 0, "ChildWindowClass", "FCWnd", WS_CHILD | WS_VISIBLE, 10, 10, 600, 550, wnd, (HMENU)CHILDWINDOW, hInst, NULL );
    CreateWindowEx( 0, "BUTTON", "Terminal", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | BS_CENTER, 620, 30, 80, 25, wnd, (HMENU)TERMINAL, hInst, NULL );
    CreateWindowEx( 0, "BUTTON", "I/O", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | BS_CENTER, 620, 70, 80, 25, wnd, (HMENU)INOUT, hInst, NULL );
    CreateWindowEx( 0, "BUTTON", "Process", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | BS_CENTER, 620, 110, 80, 25, wnd, (HMENU)PROCESS, hInst, NULL );
    CreateWindowEx( 0, "BUTTON", "Decision", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | BS_CENTER, 620, 150, 80, 25, wnd, (HMENU)DECISION, hInst, NULL );
    //CreateWindowEx( 0, "BUTTON", "Print", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | BS_CENTER, 620, 400, 80, 25, wnd, (HMENU)PRINT, hInst, NULL );

    ShowWindow( wnd, SW_SHOW );
    UpdateWindow( wnd );

    while( GetMessage( &msg, wnd, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "MainWindowClass", hInst );
    UnregisterClass( "ChildWindowClass", hInst );
    UnregisterClass( "BoxClass", hInst );

    return 0;
}
