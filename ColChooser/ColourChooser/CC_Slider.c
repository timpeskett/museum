#include "CC_Slider.h"

int InitSliderControl( HINSTANCE hInst, HBRUSH bg )
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = 0;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = (WNDPROC)SliderProc;
    wc.hInstance = hInst;
    wc.hbrBackground = bg;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "CC_Slider";
    wc.hIconSm = NULL;

    return RegisterClassEx( &wc );
}


LRESULT CALLBACK SliderProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam )
{
    HDC dc;
    PAINTSTRUCT ps;
    RECT wndr;
    int i;
    unsigned int colval;
    struct SliderData *sd;

    switch( msg )
    {
        case WM_CREATE:
            sd = (struct SliderData*)((CREATESTRUCT*)lParam)->lpCreateParams;
            SetSliderData( wnd, sd );
            break;
        case WM_PAINT:
            dc = BeginPaint( wnd, &ps );

            GetClientRect( wnd, &wndr );
            sd = GetSliderData( wnd );

            colval = sd->low;

            for( i = wndr.left; i < wndr.right; i++ )
            {
                if( i - wndr.left == sd->sliderpos )
                {
                    DrawLine( dc, i, wndr.top, i, wndr.bottom, 0xFFFFFF );
                }
                else
                {
                    DrawLine( dc, i, wndr.top, i, wndr.bottom, colval );
                }
                colval += sd->step;
                if( colval > sd->high )
                {
                    colval = sd->high;
                }
            }

            EndPaint( wnd, &ps );
            break;
        case WM_LBUTTONDOWN:
            GetClientRect( wnd, &wndr );
            sd = GetSliderData( wnd );

            sd->sliderpos = LOWORD(lParam) - wndr.left;
            RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            break;
        case WM_MOUSEMOVE:
            if( wParam & MK_LBUTTON )
            {
                GetClientRect( wnd, &wndr );
                sd = GetSliderData( wnd );

                sd->sliderpos = LOWORD(lParam) - wndr.left;
                RedrawWindow( wnd, NULL, NULL, RDW_INVALIDATE );
            }
            break;
        default:
            return DefWindowProc( wnd, msg, wParam, lParam );
    }

    return 0;
}


void DrawLine( HDC dc, int x1, int y1, int x2, int y2, unsigned int col )
{
    HPEN pen, oldp;
    POINT prevp;

    pen = CreatePen( PS_SOLID, 0, col );
    oldp = SelectObject( dc, pen );

    MoveToEx( dc, x1, y1, &prevp );
    LineTo( dc, x2, y2 );

    MoveToEx( dc, prevp.x, prevp.y, NULL );

    SelectObject( dc, oldp );
    DeleteObject( pen );
}


struct SliderData* GetSliderData( HWND wnd )
{
    return (struct SliderData*)GetWndUD( wnd );
}

void SetSliderData( HWND wnd, struct SliderData* sd )
{
    SetWndUD( wnd, (int)sd );
}

void DestroySliderControl( HINSTANCE hInst )
{
    UnregisterClass( "CC_Slider", hInst );
}
