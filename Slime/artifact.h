#ifndef ARTIFACT_H
#define ARTIFACT_H

#include <windows.h>
#include <stdlib.h>
#include "ball.h"


struct artifact
{
    RECT coords;
    char *name;
    unsigned int col;
    unsigned int msg;
    struct artifact *next;
};


struct artifact *GetNextArtifact( struct artifact* art );
char *GetArtifactName( struct artifact *art );

void EraseArtifact( struct artifact *art, HDC dc, unsigned int backcol );
void DrawArtifact( struct artifact *art, HDC dc );

int CheckArtifactCollision( struct artifact *art, struct ball *iod );
int GetArtifactMessage( struct artifact *art );

struct artifact *CreateArtifact( char *name, RECT *coords, unsigned int col, unsigned int msg );

struct artifact *CreateNewArtifactAfter( struct artifact *before, char *name, RECT *coords, unsigned int col, unsigned int msgnum );

void SetNextArtifact( struct artifact *toset, struct artifact *newval );

void GetArtifactCoords( struct artifact *art, RECT *coords );

#endif
