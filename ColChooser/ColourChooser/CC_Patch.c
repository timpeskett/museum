#include "CC_Patch.h"

/*struct Patch
{
    RECT pos;
    RECT bounds;
    unsigned int col;
    enum Shape shape;
    int pid;
    int width;

    struct Patch *next;
};*/

//needs to be returned so memory can be allocated
struct Patch * CreatePatch( int x, int y, int w, int pid, RECT *bounds, unsigned int col, enum Shape shape )
{
    struct Patch *newp;

    newp = malloc( sizeof( struct Patch ) );

    newp->pos.left = x;
    newp->pos.top = y;
    newp->pos.right = newp->pos.left + w;
    newp->pos.bottom = newp->pos.top + w;

    newp->width = w;

    newp->pid = pid;

    memcpy( &newp->oldpos, &newp->pos, sizeof( RECT ) );
    memcpy( &newp->bounds, bounds, sizeof( RECT ) );

    newp->col = col;
    newp->shape = shape;

    newp->next = NULL;

    return newp;
}


void DeletePatch( struct Patch *patch )
{
    free( patch );
}


//brilliant use of recursion, if I may say so myself :p
void SubjectPatchToLocomotiveForce( struct Patch *patch, int dx, int dy )
{
    patch->pos.left += dx;
    patch->pos.right += dx;
    patch->pos.top += dy;
    patch->pos.bottom += dy;

    if( patch->pos.bottom > patch->bounds.bottom )
    {
        SubjectPatchToLocomotiveForce( patch, 0, patch->bounds.bottom - patch->pos.bottom );
    }
    if( patch->pos.right > patch->bounds.right )
    {
        SubjectPatchToLocomotiveForce( patch, patch->bounds.right - patch->pos.right, 0 );
    }
    if( patch->pos.top < patch->bounds.top )
    {
        SubjectPatchToLocomotiveForce( patch, 0, patch->bounds.top - patch->pos.top );
    }
    if( patch->pos.left < patch->bounds.left )
    {
        SubjectPatchToLocomotiveForce( patch, patch->bounds.left - patch->pos.left, 0 );
    }
}

void ErasePatch( struct Patch *patch, HDC dc, unsigned int backcol )
{
    DrawShape( dc, patch->shape, backcol, &patch->oldpos, patch->width );
}

void DrawPatch( struct Patch *patch, HDC dc )
{
    DrawShape( dc, patch->shape, patch->col, &patch->pos, patch->width );

    memcpy( &patch->oldpos, &patch->pos, sizeof(RECT) );
}


void DrawShape( HDC dc, enum Shape shape, unsigned int col, RECT *pos, int width )
{
    HBRUSH colb, oldb;
    HPEN colp, oldp;
    POINT points[6];
    int leftd;

    colb = CreateSolidBrush( col );
    colp = CreatePen( PS_SOLID, 0, col );

    oldb = SelectObject( dc, colb );
    oldp = SelectObject( dc, colp );

    switch( shape )
    {
        case CC_Rectangle:
            Rectangle( dc, pos->left, pos->top, pos->right, pos->bottom );
            break;
        case CC_Circle:
            Ellipse( dc, pos->left, pos->top, pos->right, pos->bottom );
            break;
        case CC_Hexagon: //best to do side finding logic here
            points[0].x = pos->left;
            points[3].x = pos->right;
            points[1].y = points[2].y = pos->top;
            points[5].y = points[4].y = pos->bottom;

            leftd = width / 2 / tan( 60 * RAD );

            points[1].x = points[5].x = pos->left + leftd;
            points[2].x = points[4].x = pos->right - leftd;
            points[0].y = points[3].y = pos->top + width / 2;

            Polygon( dc, points, 6 );
            break;
        default:
            MessageBox(NULL,"Unknown shape", "Error Encountered", MB_OK );
            break;
    }

    SelectObject( dc, oldb );
    SelectObject( dc, oldp );

    DeleteObject( colb );
    DeleteObject( colp );
}

int GetPatchID( struct Patch *patch )
{
    return patch->pid;
}


int IsPointInPatch( struct Patch *patch, int x, int y )
{
    if( !(x >= patch->pos.left && x <= patch->pos.right) )
    {
        return 0;
    }
    if( !(y >= patch->pos.top && y <= patch->pos.bottom ) )
    {
        return 0;
    }
    return 1;
}
