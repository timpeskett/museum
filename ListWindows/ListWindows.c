#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <psapi.h>

#define TREE_ID 10001
#define REFRESH_ID 10002
#define HIGHLIGHT_ID 10003
#define STATIC_ID 10004
#define PULL_ID 10005
#define DESTROY_ID 10006
#define CLOSE_ID 10007
#define KILLPROC_ID 10008
#define EDITMEM_ID 10009
#define SHOW_ID 10010
#define HIDE_ID 10011
#define REDRAW_ID 10012

HWND current = NULL;

void PopulateList( HTREEITEM parent, HWND hwnd, HWND tree )
{
    TV_INSERTSTRUCT tvi;
    char title[MAX_PATH];
    char classname[MAX_PATH];
    char fullname[MAX_PATH];
    HWND next;
    HTREEITEM current;

    GetWindowText( hwnd, title, MAX_PATH );
    GetClassName( hwnd, classname, MAX_PATH );

    sprintf( fullname, "%s - {%s}", title, classname );

    tvi.hParent = parent;
    tvi.hInsertAfter = TVI_SORT;
    tvi.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvi.item.pszText = fullname;
    tvi.item.lParam = (LPARAM)hwnd;

    current = (HTREEITEM)SendMessage( tree, TVM_INSERTITEM, 0, (LPARAM)&tvi );

    next = GetWindow( hwnd, GW_CHILD );
    if( next )
    {
        PopulateList( current, next, tree );
    }
    next = GetWindow( hwnd, GW_HWNDNEXT );
    if( next )
    {
        PopulateList( parent, next, tree );
    }
    return;
}


void GetProcInfo( HWND wnd, HWND stat )
{
    HANDLE proc;
    char fulltext[MAX_PATH], pathname[MAX_PATH], parent[MAX_PATH];
    DWORD pid;

    memset( pathname, 0, MAX_PATH );

    GetWindowText( GetParent( wnd ), fulltext, MAX_PATH );
    GetClassName( GetParent( wnd ), pathname, MAX_PATH );
    sprintf( parent, "%s - {%s}", fulltext, pathname );

    memset( fulltext, 0, MAX_PATH );
    memset( pathname, 0, MAX_PATH );

    GetWindowThreadProcessId( wnd, &pid );
    proc = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid );
    GetModuleFileNameEx( proc, NULL, pathname, MAX_PATH );
    CloseHandle( proc );

    sprintf( fulltext, "Parent: %s\nPathName: %s\nVisibility: %s\n", parent, pathname, (!IsWindowVisible(wnd)) ? "Hidden" : "Visible" );

    SendMessage( stat, WM_SETTEXT, 0, (LPARAM)fulltext );
}


void EditProcMem( HWND hwnd, HWND stat )
{
    HANDLE proc;
    DWORD pid;
    DWORD retval;
    void* start;

    GetWindowThreadProcessId( hwnd, &pid );
    proc = OpenProcess( PROCESS_VM_WRITE | PROCESS_VM_OPERATION, 0, pid );
    start = VirtualAllocEx( proc, NULL, 4, MEM_COMMIT, PAGE_READWRITE );
    retval = WriteProcessMemory( proc, start, (void*)&pid, 4, &pid );
    if( !(pid && retval) )
    {
        SendMessage( stat, WM_SETTEXT, 0, (LPARAM)"Couldn't write process memory" );
    }
    else
    {
        SendMessage( stat, WM_SETTEXT, 0, (LPARAM)"process memory written" );
    }
    VirtualFreeEx( proc, start, 4, MEM_DECOMMIT );
    CloseHandle( proc );
}


void HighlightWindow( HWND hwnd, unsigned int bc )
{
    HBRUSH brush;
    RECT wndrect;
    HDC dc;

    dc = GetWindowDC( hwnd );
    GetWindowRect( hwnd, &wndrect );
    brush = CreateSolidBrush( bc );
    brush = (HBRUSH)SelectObject( dc, brush );
    Rectangle( dc, 0, 0, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top );
    brush = (HBRUSH)SelectObject( dc, brush );
    ReleaseDC( hwnd, dc );
    DeleteObject( brush );
}


void KillProcess( HWND wnd, HWND stat )
{
    HANDLE proc;
    DWORD pid;

    GetWindowThreadProcessId( wnd, &pid );
    proc = OpenProcess( PROCESS_TERMINATE, 0, pid );
    SendMessage( stat, WM_SETTEXT, 0, (LPARAM)(TerminateProcess( proc, 0 ) ? "Process Terminated" : "Could Not Terminate Process") );
    CloseHandle( proc );
}


LRESULT CALLBACK WindowProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_CREATE:
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
        case WM_COMMAND:
            switch( wParam & 0xFFFF )
            {
                case REFRESH_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        SendDlgItemMessage( wnd, TREE_ID, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT );
                        PopulateList( NULL, GetDesktopWindow(), GetDlgItem( wnd, TREE_ID ) );
                    }
                    break;
                case HIGHLIGHT_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            HighlightWindow( current, 0xFF0000 );
                        }
                    }
                    break;
                case PULL_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            SetForegroundWindow( current );
                        }
                    }
                    break;
                case DESTROY_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            SendMessage( current, WM_DESTROY, 0, 0 );
                        }
                    }
                    break;
                case CLOSE_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            SendMessage( current, WM_CLOSE, 0, 0 );
                        }
                    }
                    break;
                case KILLPROC_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            KillProcess( current, GetDlgItem( wnd, STATIC_ID ) );
                        }
                    }
                    break;
                case EDITMEM_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            EditProcMem( current, GetDlgItem( wnd, STATIC_ID ) );
                        }
                    }
                    break;
                case SHOW_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            ShowWindow( current, SW_SHOW );
                            UpdateWindow( current );
                        }
                    }
                    break;
                case HIDE_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            ShowWindow( current, SW_HIDE );
                            UpdateWindow( current );
                        }
                    }
                    break;
                case REDRAW_ID:
                    if( wParam >> 16 == BN_CLICKED )
                    {
                        if( current != NULL )
                        {
                            RedrawWindow( current, NULL, NULL, RDW_ERASE | RDW_INVALIDATE );
                            UpdateWindow( current );
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case WM_NOTIFY:
            if( wParam == TREE_ID )
            {
                if( ((NM_TREEVIEW*)lParam)->hdr.code == TVN_SELCHANGED )
                {
                    current = (HWND)((NM_TREEVIEW*)lParam)->itemNew.lParam;
                    GetProcInfo( current, GetDlgItem( wnd, STATIC_ID ));
                }
            }
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
    }

    return 0;
}


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdArgs, int CmdShow )
{
    HWND window, tree, refresh, stat, hl, pull, destroy, close, kp, epm, show, hide, redraw;
    MSG msg;
    WNDCLASSEX wc;

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = 0;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetSysColorBrush( COLOR_INACTIVEBORDER );
    wc.lpszMenuName = NULL;
    wc.hInstance = hInst;
    wc.lpszClassName = "WindowList";
    wc.hIconSm = NULL;
    wc.lpfnWndProc = (WNDPROC)WindowProc;

    if( !RegisterClassEx( &wc ))
    {
        MessageBox( NULL, "Could not register class", "Error", MB_OK );
        return 0;
    }

    window = CreateWindowEx( 0, "WindowList", "ListWindows", WS_SYSMENU | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInst, NULL );

    if( window == NULL )
    {
        MessageBox( NULL, "Could not create window","Error", MB_OK );
        UnregisterClass( "WindowList", hInst );
        return 0;
    }

    InitCommonControls();

    tree = CreateWindowEx( 0, "SysTreeView32", "Windows", WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 20, 100, 400, 400, window, (HMENU)TREE_ID, hInst, NULL );

    if( !tree )
    {
        MessageBox( NULL, "Could not create tree view", "error", MB_OK );
    }

    refresh = CreateWindowEx( 0, "BUTTON", "Refresh", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_NOTIFY | BS_TEXT, 10, 10, 100, 30, window, (HMENU)REFRESH_ID, hInst, NULL );
    hl = CreateWindowEx( 0, "BUTTON", "Highlight", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_NOTIFY | BS_TEXT, 450, 10, 100, 30, window, (HMENU)HIGHLIGHT_ID, hInst, NULL );
    stat = CreateWindowEx( 0, "STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_LEFT, 450, 300, 330, 250, window, (HMENU)STATIC_ID, hInst, NULL );
    pull = CreateWindowEx( 0, "BUTTON", "BrToFront", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 450, 45, 100, 30, window, (HMENU)PULL_ID, hInst, NULL );
    destroy = CreateWindowEx( 0, "BUTTON", "Destroy", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 450, 80, 100, 30, window, (HMENU)DESTROY_ID, hInst, NULL );
    close = CreateWindowEx( 0, "BUTTON", "Close", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 450, 115, 100, 30, window, (HMENU)CLOSE_ID, hInst, NULL );
    kp = CreateWindowEx( 0, "BUTTON", "KillProc", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 450, 150, 100, 30, window, (HMENU)KILLPROC_ID, hInst, NULL );
    epm = CreateWindowEx( 0, "BUTTON", "EditProcMem", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 450, 185, 100, 30, window, (HMENU)EDITMEM_ID, hInst, NULL );
    show = CreateWindowEx( 0, "BUTTON", "Show", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 560, 10, 100, 30, window, (HMENU)SHOW_ID, hInst, NULL );
    hide = CreateWindowEx( 0, "BUTTON", "Hide", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 560, 45, 100, 30, window, (HMENU)HIDE_ID, hInst, NULL );
    redraw = CreateWindowEx( 0, "BUTTON", "Redraw", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_NOTIFY | BS_TEXT, 560, 80, 100, 30, window, (HMENU)REDRAW_ID, hInst, NULL );

    ShowWindow( window, SW_SHOW );
    UpdateWindow( window );

    while( GetMessage( &msg, window, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterClass( "WindowList", hInst );

    return 0;
}
