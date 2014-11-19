#ifndef SLIMES_H
#define SLIMES_H

#include <windows.h>
#include <string.h>
#include "ball.h"

#define LEFT (1)
#define RIGHT (2)

#define abs(x) ((x<0)?(-(x)):(x))

struct slime
{
    float x, y, vx, vy; //position and velocity
    int w, h; //width and height
    int r, d; //radius and diameter - for convenience
    RECT brect; //boundary rectangle
    int dir;
    unsigned int brush; //brush
    unsigned int eyecolour;
    int score; //score
    char *name; //name
};

void CreateSlime( struct slime* marty, int x, int y, int w, RECT *bound, int dir, unsigned int col, char *name );
void DeleteSlime( struct slime* marty );

void EraseSlime( struct slime* marty, HDC dc, unsigned int col );
void DrawSlime( struct slime* marty, HDC dc, unsigned int backcol );
void UpdateSlime( struct slime* marty, float grav ); //updates all the slime's variables, such as x and y position.

//performs collision detection and changes variables accordingly. returns 1 if hit, 0 otherwise
//int BallSlimeCD( struct slime * marty, struct ball *iod );

//returns old x velocity
float GetSlimeXVelocity( struct slime *marty );
float SetSlimeXVelocity( struct slime *marty, float xvel );
//returns old y velocity
float GetSlimeYVelocity( struct slime *marty );
float SetSlimeYVelocity( struct slime *marty, float yvel );

int IsJumping( struct slime *marty );

int IsBallSlimeCollision( struct slime *marty, struct ball *iod ); //mainly for internal use
int GetBallSlimeDisplacement( struct slime *marty, struct ball *iod );

int GetSlimeAngleBall( struct slime *marty, struct ball *iod );

int CheckCollision( struct slime *marty, struct ball *iod, int (*action)(struct slime*, struct ball*) );
int DefCD( struct slime *marty, struct ball *iod );
int GetSlimeSideHit( struct slime *marty, struct ball *iod );

#endif
