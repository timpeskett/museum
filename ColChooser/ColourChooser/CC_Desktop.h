#ifndef CC_DESKTOP_H
#define CC_DESKTOP_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "CC_Patch.h"
#include "CC_Shapes.h"

struct CC_Desktop
{
    struct Patch *pll;
    RECT bounds;
    unsigned int backcol;
};

void BelieveDesktop( struct CC_Desktop *cd, RECT *b, unsigned int backcol ); //already a windows function called CreateDesktop
void DestroyDesktop( struct CC_Desktop *cd );

//specify null for bounds if desktop bounds are desired
int AddPatch( struct CC_Desktop *cd, int x, int y, int w, unsigned int col, enum Shape shape, RECT *bounds );

//DOESN'T FREE MEMORY
struct Patch* RemovePatch( struct CC_Desktop *cd, int pid );

void BringToFront( struct CC_Desktop *cd, int pid );

int GetPatchFromCoord( struct CC_Desktop *cd, int x, int y );

void MovePatch( struct CC_Desktop *cd, int pid, int deltax, int deltay );

void DrawDesktop( struct CC_Desktop *cd, HDC dc );

int GetMaxPID( struct CC_Desktop *cd );

struct Patch *GetPatch( struct CC_Desktop *cd, int pid );

#endif
