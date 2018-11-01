#pragma once

// this file contains the simulation parameters.


#include "main.h"

// the body structure
struct body {
	float x;    // the x position
	float y;    // the y position
	float vx;    // the x-axis velocity
	float vy;    // the y-axis velocity
	double m;    // the body mass
};



// epsilon as damping factor
#define eps (0.1)

// gravity factor
#define G  (1e-7)

// cursor weight
const float cursor_weight = (0.5*NUM_BODIES);

// reflection factor
// lose speed if collide to the boundary
#define collision_damping (0.6)

void freeCudaMem(void* p);
unsigned char* initCanvas();
struct body *initializeNBodyCuda(char method);
void rasterize(struct body *bodies, unsigned char *buffer);
void NBodyTimestepCuda(struct body *bodies, float rx, float ry, bool cursor);

