/*Part of The Colour Chooser Library created by Tim Peskett*/

#include "CC_Desktop.h"


/*struct CC_Desktop
{
    struct CC_Patch *pll;
    RECT bounds;
    unsigned int backcol;
};*/


void BelieveDesktop( struct CC_Desktop *cd, RECT *b, unsigned int backcol )
{
    cd->pll = NULL;

    memcpy( &cd->bounds, b, sizeof( RECT ) );

    cd->backcol = backcol;
}

void DestroyDesktop( struct CC_Desktop *cd )
{
    struct Patch *current = cd->pll;

    while( current )
    {
        current = current->next;
        DeletePatch( cd->pll );
        cd->pll = current;
    }
}

//specify null for bounds if desktop bounds are desired. //probably won't work when tested, pay special debugging attention
int AddPatch( struct CC_Desktop *cd, int x, int y, int w, int h, unsigned int col, enum Shape shape, RECT *bounds )
{
    RECT *b;
    struct Patch *current = cd->pll;
    int pid;

    b = bounds ? bounds : &cd->bounds;

    pid = GetMaxPID( cd ) + 1;

    if( !current )
    {
        current = cd->pll = CreatePatch( x, y, w, h, pid, b, col, shape );
    }
    else
    {
        while( current->next )
        {
            current = current->next;
        }
        current->next = CreatePatch( x, y, w, h, pid, b, col, shape );
    }

    return pid;
}

//1 = success, 0 = no dice. couldn't really avoid writing it like this, otherwise would have had to write other one time functions
//DOESN'T FREE MEMORY
struct Patch *RemovePatch( struct CC_Desktop *cd, int pid )
{
    struct Patch* current = cd->pll, *temp;

    if( current )
    {
            if( GetPatchID( current) == pid )
            {
                temp = current;
                cd->pll = current->next;
                return temp;
            }
            while( current->next )
            {
                if( GetPatchID( current->next ) == pid )
                {
                    temp = current->next;
                    current->next = current->next->next;
                    return temp;
                }
                current = current->next;
            }
    }

    return NULL;
}

//returns a patch ID. -1 on error
int GetPatchFromCoord( struct CC_Desktop *cd, int x, int y )
{
    struct Patch *current = cd->pll;
    int pid = -1;

    while( current )
    {
        if( IsPointInPatch( current, x, y ) )
        {
            pid = GetPatchID( current );
        }
        current = current->next;
    }

    return pid;
}

void MovePatch( struct CC_Desktop *cd, int pid, int deltax, int deltay )
{
    struct Patch *current = cd->pll;

    while( current )
    {
        if( GetPatchID( current ) == pid )
        {
            SubjectPatchToLocomotiveForce( current, deltax, deltay );
        }
        current = current->next;
    }
}

void DrawDesktop( struct CC_Desktop *cd, HDC dc )
{
    struct Patch *current = cd->pll;

    while( current )
    {
        ErasePatch( current, dc, cd->backcol );
        DrawPatch( current, dc );
        current = current->next;
    }
}


int GetMaxPID( struct CC_Desktop *cd )
{
    struct Patch *current = cd->pll;
    int max = 0, pid = 0;

    while( current )
    {
        if( (pid = GetPatchID( current )) > max )
        {
            max = pid;
        }
        current = current->next;
    }

    return max;
}


struct Patch *GetPatch( struct CC_Desktop *cd, int pid )
{
    struct Patch *current = cd->pll;

    while( current )
    {
        if( GetPatchID( current ) == pid )
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


void BringToFront( struct CC_Desktop *cd, int pid )
{
    struct Patch *desired, *current = cd->pll;

    desired = GetPatch( cd, pid );

    if( desired )
    {
        while( current )
        {
            if( current->next == NULL )
            {
                current->next = desired;
                break;
            }
            current = current->next;
        }
        RemovePatch( cd, pid );
        desired->next = NULL;
    }
}


void ChangeBounds( struct CC_Desktop *cd, RECT *bounds )
{
    memcpy( &cd->bounds, bounds, sizeof( RECT ));
}


void SetPatchPos( struct CC_Desktop *cd, int pid, int x, int y, int w, int h )
{
    struct Patch *pt;

    pt = GetPatch( cd, pid );

    pt->pos.left = x;
    pt->pos.top = y;
    pt->pos.right = pt->pos.left + w;
    pt->pos.bottom = pt->pos.top + h;

    pt->width = w;
}


int GetMinPID( struct CC_Desktop *cd )
{
    struct Patch *current = cd->pll;
    int min = 10, pid = 0;

    while( current )
    {
        if( (pid = GetPatchID( current )) < min )
        {
            min = pid;
        }
        current = current->next;
    }

    return min;
}

RECT *GetDesktopBounds( struct CC_Desktop *cd )
{
    return &cd->bounds;
}


unsigned int GetPatchCol( struct CC_Desktop *cd, int pid )
{
    return GetColour( GetPatch( cd, pid ) );
}

void SetPatchCol( struct CC_Desktop *cd, int pid, unsigned int col )
{
    SetColour( GetPatch( cd, pid ), col );
}


int GetFirstPatchPID( struct CC_Desktop *cd )
{
    return GetPatchID( cd->pll );
}
