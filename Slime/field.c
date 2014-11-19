#include "field.h"


void CreateField( struct field *f, void (*FieldEvent)( struct artifact*, struct ball*, unsigned int ) )
{
    f->FieldEvent = FieldEvent;
    f->list = NULL;
}

void DeleteField( struct field *f )
{
    struct artifact *temp;

    while( f->list )
    {
        temp = GetNextArtifact( f->list );
        free( GetArtifactName( f->list ) );
        free( f->list );
        f->list = temp;
    }
}

void EraseField( struct field *f, HDC dc, unsigned int backcol )
{
    struct artifact *current = f->list;

    while( current )
    {
        EraseArtifact( current, dc, backcol );
        current = GetNextArtifact( current );
    }
}

void DrawField( struct field *f, HDC dc )
{
    struct artifact *current = f->list;

    while( current )
    {
        DrawArtifact( current, dc );
        current = GetNextArtifact( current );
    }
}

void CheckField( struct field *f, struct ball *iod )
{
    struct artifact *current = f->list;

    while( current )
    {
        if( CheckArtifactCollision( current, iod ) )
        {
            f->FieldEvent( current, iod, GetArtifactMessage( current ) );
        }
        current = GetNextArtifact( current );
    }
}

void AddArtifact( struct field *f, char *name, RECT *rect, unsigned int col, unsigned int msgnum )
{
    struct artifact *current = f->list;

    while( f->list )
    {
        if( !GetNextArtifact( current ) )
        {
            break;
        }
        current = GetNextArtifact( current );
    }

    if( !current )
    {
        f->list = CreateArtifact( name, rect, col, msgnum );
    }
    else
    {
        current = CreateNewArtifactAfter( current, name, rect, col, msgnum );
    }
}

int RemoveArtifact( struct field *f, char *name )
{
    struct artifact *current, *prev;

    current = GetArtifactByName( f, name );

    if( !current )
    {
        return 0;
    }

    prev = GetPreviousArtifact( f, current );

    if( !prev )
    {
        prev = f->list;
    }

    SetNextArtifact( prev, GetNextArtifact( current ) );

    free( GetArtifactName( current ) );
    free( current );

    return 1;
}

struct artifact *GetArtifactByName( struct field *f, char *name )
{
    struct artifact *current = f->list;

    while( current )
    {
        if( strcmp( GetArtifactName( current ), name ) == 0 )
        {
            return current;
        }
        current = GetNextArtifact( current );
    }

    return NULL;
}

struct artifact *GetPreviousArtifact( struct field *f, struct artifact *art )
{
    struct artifact *current = f->list;

    while( f->list )
    {
        if( GetNextArtifact( current ) == art )
        {
            return current;
        }
        current = GetNextArtifact( current );
    }

    return NULL;
}
