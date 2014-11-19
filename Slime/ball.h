#ifndef BALL_H
#define BALL_H

#include <windows.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include <stdio.h>

struct ball
{
    float x, y, vx, vy;
    int r, d;
    HBRUSH col;
    RECT brect;
};

void CreateBall( struct ball *iod, int x, int y, int w, RECT *bound, unsigned int col );
void DeleteBall( struct ball *iod );

void EraseBall( struct ball *iod, HDC dc, unsigned int backcol );
void DrawBall( struct ball *iod, HDC dc, unsigned int backcol );
void UpdateBall( struct ball *iod, float grav, float maxvelocity );

void SetBallX( struct ball *iod, float newx );
int GetBallXVelocity( struct ball *iod );
int SetBallXVelocity( struct ball *iod, float newvx );

void SetBallY( struct ball *iod, float newy );
int GetBallYVelocity( struct ball *iod );
int SetBallYVelocity( struct ball *iod, float newvy );

int GetBallX( struct ball *iod );
int GetBallY( struct ball *iod );

int GetBallRadius( struct ball *iod );

int GetBallBearing( struct ball *iod );

void GetBallCentre( struct ball *iod, int *x, int *y );

int GetBallMagnitude( struct ball *iod );

#endif
