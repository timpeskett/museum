#include "ball.h"

/*
struct ball
{
    float x, y, vx, vy;
    int r, d;
    HBRUSH col;
    RECT brect
};
*/

void CreateBall( struct ball *iod, int x, int y, int w, RECT *bound, unsigned int col )
{
    iod->x = x;
    iod->y = y;
    iod->d = w;
    iod->col = CreateSolidBrush( col );

    iod->r = w / 2;
    memcpy( (void*)&(iod->brect), (void*)bound, sizeof( RECT ) );
}

void DeleteBall( struct ball *iod )
{
    DeleteObject( iod->col );
}


void EraseBall( struct ball *iod, HDC dc, unsigned int backcol )
{
    RECT ballrect;
    HBRUSH old, back;
    HPEN backp, oldp;

    ballrect.left = iod->x;
    ballrect.top = iod->y;
    ballrect.right = ballrect.left + iod->d;
    ballrect.bottom = ballrect.top + iod->d;

    back = CreateSolidBrush( backcol );
    backp = CreatePen( PS_SOLID, 0, backcol );

    old = SelectObject( dc, back );
    oldp = SelectObject( dc, backp );

    Rectangle( dc, ballrect.left, ballrect.top, ballrect.right, ballrect.bottom );

    SelectObject( dc, old );
    SelectObject( dc, oldp );

    DeleteObject( back );
    DeleteObject( backp );
}

void DrawBall( struct ball *iod, HDC dc, unsigned int backcol )
{
    HPEN old, back;
    HBRUSH oldb;
    RECT circle;

    circle.left = iod->x;
    circle.top = iod->y;
    circle.right = circle.left + iod->d;
    circle.bottom = circle.top + iod->d;

    back = CreatePen( PS_SOLID, 0, backcol );

    old = SelectObject( dc, back );
    oldb = SelectObject( dc, iod->col );

    Ellipse( dc, circle.left, circle.top, circle.right, circle.bottom );

    SelectObject( dc, old );
    SelectObject( dc, oldb );

    DeleteObject( back );
}


void UpdateBall( struct ball *iod, float grav, float maxvelocity )
{
    int newx, newy;
    float newvx, newvy;

    newx = iod->x + iod->r; //should get us to the centre of the circle
    newy = iod->y + iod->r;//"""
    newx += iod->vx;
    newy -= iod->vy;

    if( newx + iod->r >= iod->brect.right || newx - iod->r <= iod->brect.left ) //tests whether outer part of circle is outside bounds
    {
        iod->vx = -(iod->vx);
        iod->x = (newx + iod->r >= iod->brect.right) ? iod->brect.right - 1 - iod->d : iod->brect.left + 1;
    }
    else
    {
        iod->x += iod->vx;
    }

    if( newy + iod->r > iod->brect.bottom )
    {
        iod->vy = -(iod->vy);
        iod->y = iod->brect.bottom - iod->d;
    }
    else
    {
        iod->y -= iod->vy;
    }
    iod->vy -= grav / 30.0f;

    if( maxvelocity >= 1.0f && GetBallMagnitude( iod ) > maxvelocity )
    {
        newvx = GetBallXVelocity( iod );
        newvy = GetBallYVelocity( iod );
        newvx = newvx / GetBallMagnitude( iod ) * maxvelocity;
        newvy = newvy / GetBallMagnitude( iod ) * maxvelocity;
        SetBallXVelocity( iod, newvx );
        SetBallYVelocity( iod, newvy );
    }
}


int GetBallXVelocity( struct ball *iod )
{
    return iod->vx;
}

int SetBallXVelocity( struct ball *iod, float newvx )
{
    float old;

    old = iod->vx;
    iod->vx = newvx;

    return old;
}

int GetBallYVelocity( struct ball *iod )
{
    return iod->vy;
}

int SetBallYVelocity( struct ball *iod, float newvy )
{
    float old;

    old = iod->vy;
    iod->vy = newvy;

    return old;
}

int GetBallX( struct ball *iod )
{
    return iod->x;
}

int GetBallY( struct ball *iod )
{
    return iod->y;
}

int GetBallRadius( struct ball *iod )
{
    return iod->r;
}


int GetBallBearing( struct ball *iod )
{
    int tohorz;
    int xsect, ysect;

    if( iod->vx == 0.0f )
    {
        tohorz = (iod->vy > 0 ) ? 180 : 0;
    }
    else
    {
        tohorz = DEGREES(atan( iod->vy / iod->vx ));
    }

    xsect = (iod->vx < 0) ? -1 : 1;
    ysect = (iod->vy < 0) ? -1 : 1;

    tohorz = ConvToTrue( abs(tohorz), xsect, ysect );

    return tohorz;
}


void GetBallCentre( struct ball *iod, int *x, int *y )
{
    *x = iod->x + iod->r;
    *y = iod->y + iod->r;
}


int GetBallMagnitude( struct ball *iod )
{
    return sqrt( pow( iod->vx, 2 ) + pow( iod->vy, 2 ) );
}


void SetBallX( struct ball *iod, float newx )
{
    iod->x = newx;
}

void SetBallY( struct ball *iod, float newy )
{
    iod->y = newy;
}
