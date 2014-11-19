#ifndef CC_PATCH_H
#define CC_PATCH_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "CC_Shapes.h"

#define PI (3.1415926)
#define RAD (PI/180)

struct Patch
{
    RECT pos, oldpos;
    RECT bounds;
    unsigned int col;
    enum Shape shape;
    int pid;
    int width;

    struct Patch *next;
};

//needs to be returned so memory can be allocated
struct Patch * CreatePatch( int x, int y, int w, int pid, RECT *bounds, unsigned int col, enum Shape shape );
void DeletePatch( struct Patch *patch );

void SubjectPatchToLocomotiveForce( struct Patch *patch, int dx, int dy );

void ErasePatch( struct Patch *patch, HDC dc, unsigned int backcol );
void DrawPatch( struct Patch *patch, HDC dc );

void DrawShape( HDC dc, enum Shape shape, unsigned int col, RECT *pos, int width );

int GetPatchID( struct Patch *patch );

int IsPointInPatch( struct Patch *patch, int x, int y );

#endif
