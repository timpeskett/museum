#include "artifact.h"


struct artifact *GetNextArtifact( struct artifact* art )
{
    return art->next;
}


char *GetArtifactName( struct artifact *art )
{
    return art->name;
}


void EraseArtifact( struct artifact *art, HDC dc, unsigned int backcol )
{
    HBRUSH brush, oldb;
    HPEN newpen, oldp;

    brush = CreateSolidBrush( backcol );
    newpen = CreatePen( PS_SOLID, 0, backcol );

    oldb = SelectObject( dc, brush );
    oldp = SelectObject( dc, newpen );

    Rectangle( dc, art->coords.left, art->coords.top, art->coords.right, art->coords.bottom );

    SelectObject( dc, oldb );
    SelectObject( dc, oldp );

    DeleteObject( brush );
    DeleteObject( newpen );
}

void DrawArtifact( struct artifact *art, HDC dc )
{
    HBRUSH brush, oldb;
    HPEN pen, oldp;

    brush = CreateSolidBrush( art->col );
    pen = CreatePen( PS_SOLID, 0, art->col );

    oldb = SelectObject( dc, brush );
    oldp = SelectObject( dc, pen );

    Rectangle( dc, art->coords.left, art->coords.top, art->coords.right, art->coords.bottom );

    SelectObject( dc, oldb );
    SelectObject( dc, oldp );

    DeleteObject( brush );
    DeleteObject( pen );
}


int CheckArtifactCollision( struct artifact *art, struct ball *iod )
{
    RECT bp;
    int centx, centy;
    int mask[4] = {0,0,0,0};
    int i, sum = 0;

    GetBallCentre( iod, &centx, &centy );
    bp.left = GetBallX( iod );
    bp.top = GetBallY( iod );
    bp.right = bp.left + GetBallRadius( iod ) * 2;
    bp.bottom = bp.top + GetBallRadius( iod ) * 2;

    if( bp.right >= art->coords.left )
    {
        mask[0] = 1;
    }
    if( bp.left <= art->coords.right )
    {
        mask[2] = 1;
    }
    if( bp.top <= art->coords.bottom )
    {
        mask[3] = 1;
    }
    if( bp.bottom >= art->coords.top )
    {
        mask[1] = 1;
    }

    for( i = 0; i < 4; i++ )
    {
        sum += mask[i];
    }

    if( sum >= 4 )
    {
        return 1;
    }

    return 0;
}


int GetArtifactMessage( struct artifact *art )
{
    return art->msg;
}


struct artifact *CreateArtifact( char *name, RECT *coords, unsigned int col, unsigned int msg )
{
    struct artifact *art;

    art = malloc( sizeof( struct artifact ) );
    art->name = malloc( strlen( name ) + 1 );
    strcpy( art->name, name );
    memcpy( &(art->coords), coords, sizeof( RECT ) );
    art->col = col;
    art->msg = msg;
    art->next = NULL;

    return art;
}


struct artifact *CreateNewArtifactAfter( struct artifact *before, char *name, RECT *coords, unsigned int col, unsigned int msgnum )
{
    before->next = CreateArtifact( name, coords, col, msgnum );

    return before->next;
}


void SetNextArtifact( struct artifact *toset, struct artifact *newval )
{
    toset->next = newval;
}


void GetArtifactCoords( struct artifact *art, RECT *coords )
{
    memcpy( coords, &art->coords, sizeof( RECT ) );
}
