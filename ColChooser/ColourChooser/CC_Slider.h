#ifndef CC_SLIDER
#define CC_SLIDER

#include <windows.h>

#define SetWndUD(wnd,data) SetWindowLong(wnd,GWL_USERDATA,data)
#define GetWndUD(wnd) GetWindowLong(wnd,GWL_USERDATA)

struct SliderData
{
    unsigned int low, high, step;
    int sliderpos;
};

//must be called before registering a slider window
int InitSliderControl( HINSTANCE hInst, HBRUSH bg );

LRESULT CALLBACK SliderProc( HWND wnd, DWORD msg, WPARAM wParam, LPARAM lParam );

struct SliderData* GetSliderData( HWND wnd );
void SetSliderData( HWND wnd, struct SliderData* sd );

void DestroySliderControl( HINSTANCE hInst );

void DrawLine( HDC dc, int x1, int y1, int x2, int y2, unsigned int col );

#endif
