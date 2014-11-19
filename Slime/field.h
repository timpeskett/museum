#ifndef FIELD_H
#define FIELD_H

#include <windows.h>
#include "ball.h"
#include "artifact.h"

struct field
{
    struct artifact *list;
    void (*FieldEvent)( struct artifact*, struct ball*, unsigned int );
};

void CreateField( struct field *f, void (*FieldEvent)( struct artifact*, struct ball*, unsigned int ) );
void DeleteField( struct field *f );

void EraseField( struct field *f, HDC dc, unsigned int backcol );
void DrawField( struct field *f, HDC dc );

void CheckField( struct field *f, struct ball *iod );

void AddArtifact( struct field *f, char *name, RECT *rect, unsigned int col, unsigned int msgnum );
int RemoveArtifact( struct field *f, char *name );

struct artifact *GetArtifactByName( struct field *f, char *name );
struct artifact *GetPreviousArtifact( struct field *f, struct artifact *art );

#endif
