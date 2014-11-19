#include "slimes.h"

/* reference purposes ---------------
struct slime
{
    float x, y, vx, vy; //position and velocity
    int w, h; //width and height
    int r, d; //radius and diameter - for convenience
    RECT brect; //boundary rectangle
    int dir;
    HBRUSH brush; //brush
    int score; //score
    char *name; //name
};
*/


void CreateSlime( struct slime* marty, int x, int y, int w, RECT *bound, int dir, unsigned int col, char *name )
{
    marty->x = (float)x;
    marty->y = (float)y;
    marty->w = w;

    memcpy( (void*)&(marty->brect), (void*)bound, sizeof( RECT ) );

    marty->dir = dir;
    marty->brush = col;
    marty->name = name;

    marty->score = 0;
    marty->r = marty->h = marty->w / 2;
    marty->d = marty->w;
    marty->vx = marty->vy = 0;
}


void DeleteSlime( struct slime *marty )
{
    return;
}


void DrawSlime( struct slime* marty, HDC dc, unsigned int backcol )
{
    RECT boundrect;
    HBRUSH old, brush;
    int halfy;
    HPEN back, oldp;

    boundrect.left = marty->x;
    boundrect.top = marty->y;
    boundrect.right = boundrect.left + marty->w;
    boundrect.bottom = boundrect.top + marty->h * 2; // * 2 is needed for arc function.

    halfy = (boundrect.bottom - boundrect.top) / 2;

    brush = CreateSolidBrush( marty->brush );
    back = CreatePen( PS_SOLID, 0, backcol );

    oldp = SelectObject( dc, back );
    old = SelectObject( dc, brush );

    Chord( dc, boundrect.left, boundrect.top, boundrect.right, boundrect.bottom, boundrect.right, boundrect.top + halfy, boundrect.left, boundrect.top + halfy );

    SelectObject( dc, old );
    SelectObject( dc, oldp );

    DeleteObject( brush );
    DeleteObject( back );
}


void EraseSlime( struct slime* marty, HDC dc, unsigned int col )
{
    RECT boundrect;
    HBRUSH newb, oldb;
    HPEN back, oldp;

    boundrect.left = marty->x;
    boundrect.top = marty->y;
    boundrect.right = boundrect.left + marty->w;
    boundrect.bottom = boundrect.top + marty->h;

    newb = CreateSolidBrush( col );
    back = CreatePen( PS_SOLID, 0, col );

    oldb = SelectObject( dc, newb );
    oldp = SelectObject( dc, back );

    Rectangle( dc, boundrect.left, boundrect.top, boundrect.right, boundrect.bottom );

    SelectObject( dc, oldb );
    SelectObject( dc, oldp );

    DeleteObject( back );
    DeleteObject( newb );
}


float GetSlimeXVelocity( struct slime *marty )
{
    return marty->vx;
}


float SetSlimeXVelocity( struct slime *marty, float xvel )
{
    float old;

    old = marty->vx;
    marty->vx = xvel;

    return old;
}


float GetSlimeYVelocity( struct slime *marty )
{
    return marty->vy;
}


float SetSlimeYVelocity( struct slime *marty, float yvel )
{
    float old;

    old = marty->vy;
    marty->vy = yvel;

    return old;
}

//assumes framerate of 30
void UpdateSlime( struct slime* marty, float grav )
{
    float newx, newy;

    newx = marty->x + marty->r + marty->vx;
    newy = marty->y + marty->r - marty->vy;

    if( newx - marty->r > marty->brect.left && newx + marty->r < marty->brect.right )
    {
        marty->x = newx - marty->r;
    }

    if( newy > marty->brect.bottom )
    {
        marty->y = marty->brect.bottom - marty->h;
        marty->vy = 0;
    }
    else if( newy - marty->r < marty->brect.top )
    {
        marty->vy -= grav / 30;
    }
    else
    {
        marty->y = newy - marty->r;
        marty->vy -= grav / 30;
    }
}

int IsJumping( struct slime *marty )
{
    if( marty->y + marty->h < marty->brect.bottom )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int GetBallSlimeDisplacement( struct slime *marty, struct ball *iod )
{
    int slimecentx, slimecenty;
    int ballcentx, ballcenty;

    slimecentx = marty->x + marty->r;
    slimecenty = marty->y + marty->r;
    GetBallCentre( iod, &ballcentx, &ballcenty );

    if( GetBallY( iod ) + GetBallRadius( iod ) / 2 > marty->y + marty->r )
    {
       return 0;
    }

    return sqrt( pow( slimecentx - ballcentx, 2 ) + pow( slimecenty - ballcenty, 2 ) );
}


int IsBallSlimeCollision( struct slime *marty, struct ball* iod )
{
    int disp;

    if( (disp = GetBallSlimeDisplacement( marty, iod )) != 0 && disp <= GetBallRadius( iod ) + marty->r )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int GetSlimeAngleBall( struct slime *marty, struct ball *iod )
{
    double xcom, ycom;
    int xpos, ypos;

    GetBallCentre( iod, &xpos, &ypos );

    xcom = abs((marty->x + marty->r) - xpos);
    ycom = abs((marty->y + marty->r) - ypos);

    if( ycom == 0 )
    {
        return 0;
    }

    return DEGREES(atan( xcom / ycom ));
}

//checks if there is a collision and calls action. Supply DefCD if none created
int CheckCollision( struct slime *marty, struct ball *iod, int (*action)(struct slime*, struct ball*) )
{
    if( IsBallSlimeCollision( marty, iod ) )
    {
        return action( marty, iod );
    }
    else
    {
        return -1;
    }
}

//Default Collision Detection routine
int DefCD( struct slime *marty, struct ball *iod )
{
    int trueangle, localang, truelocal;
    int xsect, ysect, side;
    int finalb;
    float newvx, newvy;
    int newx, newy, ang;

    trueangle = GetBallBearing( iod );
    localang = GetSlimeAngleBall( marty, iod );

    side = GetSlimeSideHit( marty, iod );

    switch( side )
    {
        case LEFT:
            xsect = -1;
            ysect = 1;
            truelocal = ConvToTrue( 90 - localang, xsect, ysect );
            break;
        case RIGHT:
            xsect = 1;
            ysect = 1;
            truelocal = ConvToTrue( 90 - localang, xsect, ysect );
            break;
        case 0:
            truelocal = 180;
            break;
        default:
            return -1;
            break;
    }

    truelocal = oppang( truelocal );

    finalb = truelocal + (truelocal - trueangle);
    finalb = oppang( finalb );

    printf("Final Bearing of ball (true bearing): %d\n", finalb );

    finalb = TrueToConv( finalb, &xsect, &ysect );

    newvx = GetBallMagnitude( iod ) * cos( RAD(finalb) );
    newvy = GetBallMagnitude( iod ) * sin( RAD(finalb) );

    if( Magnitude( newvx, newvy ) < Magnitude( marty->vx, marty->vy ) )
    {
        newvx += newvx / Magnitude( newvx, newvy ) * Magnitude( marty->vx, marty->vy );
        newvy += newvy / Magnitude( newvx, newvy ) * Magnitude( marty->vx, marty->vy );
    }

    newvx = (xsect == 1) ? newvx : -(newvx);
    newvy = (ysect == 1) ? newvy : -(newvy);

    ang = TrueToConv( truelocal, &xsect, &ysect );
    newx = (marty->r + GetBallRadius( iod )) * cos( RAD(ang) );
    newy = (marty->r + GetBallRadius( iod )) * sin( RAD(ang) );
    newy = marty->y + marty->r - newy - GetBallRadius( iod );
    if( side == LEFT )
    {
        newx = marty->x + marty->r - newx - GetBallRadius( iod );
    }
    else
    {
        newx = marty->x + marty->r + newx - GetBallRadius( iod );
    }

    SetBallX( iod, newx );
    SetBallY( iod, newy );

    if( GetBallY( iod ) >= marty->y + marty->r )
    {
        SetBallX( iod, (side == LEFT) ? marty->x - GetBallRadius( iod ) * 2 : marty->x + marty->w );
    }

    SetBallXVelocity( iod, newvx );
    SetBallYVelocity( iod, newvy );

    return 1;
}


int GetSlimeSideHit( struct slime *marty, struct ball *iod )
{
    int xpos, ypos;

    GetBallCentre( iod, &xpos, &ypos );

    if( xpos < marty->x + marty->r )
    {
        return LEFT;
    }
    else if( xpos > marty->x + marty->r )
    {
        return RIGHT;
    }
    else
    {
        return 0;
    }
}

