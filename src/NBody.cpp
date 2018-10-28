#include "main.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "NBody.h"
#include <math.h>
#include <cmath>

#include <iostream>

using namespace std;


double getRandom(double min, double max)
{
	double r = (double) rand() / RAND_MAX;
	return r * (max - min) + min;
}

void rasterize(struct body *bodies, unsigned char *buffer)
{
	/**
	rasterize the bodies from x,y: (-1,-1) to (1,1) according to some kind of formula

	Note: You can change the code for better visualization
	As the following code can be parallelized, you can optimize this routine with CUDA.

	\param bodies A collection of bodies (located on the device).
	\param buffer the RGB buffer for screen display (located on the host).
	*/

	// clear the canvas
	memset(buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 3 * sizeof(unsigned char));

	//TODO: copy the device memory to the host, and draw points on the canvas

	for (auto i = 0; i < NUM_BODIES; i++)
	{
		// make sure body->x and y is between [-1,1]
//		if (bodies[i].x <= -1 || bodies[i].x >= 1 || bodies[i].y <= -1 || bodies[i].y >= 1)
//		{
//			continue;
//		}
		int x = (int) lround(SCREEN_WIDTH * ((bodies[i].x + 1) / 2.0));
		int y = (int) lround(SCREEN_HEIGHT * ((bodies[i].y + 1) / 2.0));
		// cout << x << ',' << y << endl;
		buffer[x * SCREEN_WIDTH * 3 + y * 3 + 0] = 0XFF;
		buffer[x * SCREEN_WIDTH * 3 + y * 3 + 1] = 0XFF;
		buffer[x * SCREEN_WIDTH * 3 + y * 3 + 2] = 0XFF;
	}


}

struct body *initializeNBodyCuda()
{
	/**
	initialize the bodies, then copy to the CUDA device memory
	return the device pointer so that it can be reused in the NBodyTimestepCuda function.
	*/

	// initialize the position and velocity
	// you can implement own initial conditions to form a sprial/ellipse galaxy, have fun.

	body *bodies = new body[NUM_BODIES];
	for (auto i = 0; i < NUM_BODIES; i++)
	{
		bodies[i].m = 1;
		bodies[i].vx = 0;
		bodies[i].vy = 0;
		bodies[i].x = getRandom(-1., 1.);
		bodies[i].y = getRandom(-1., 1.);
	}
	return bodies;
}


struct vector2d
{
	double x;
	double y;
};

double vector2dNorm(vector2d v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

vector2d gravitationalAcceleration(body *selfBody, body *body2)
{
	vector2d r = {body2->x - selfBody->x, body2->y - selfBody->y};
	double tmp = G * (body2->m) / pow((pow(vector2dNorm(r), 2) + pow(eps, 2)), 3 / 2.0);
	vector2d acc = {r.x * tmp, r.y * tmp};
	return acc;
}

void NBodyTimestepCuda(struct body *bodies, double rx, double ry, bool cursor)
{
	/**
	Compute a time step on the CUDA device. 
	TODO: correctly manage the device memory, compute the time step with proper block/threads 

	\param bodies A collection of bodies (located on the device).
	\param rx position x of the cursor.
	\param ry position y of the cursor.
	\param cursor Enable the mouse interaction if true (adding a weight = cursor_weight body in the computation).
	*/
	double timeStep = 1;
	body cursorBody;
	cursorBody.m =
			cursor_weight * (double) cursor; // convert bool condition to scale multiply to avoid branch selection
	cursorBody.x = rx;
	cursorBody.y = ry;
	cursorBody.vx = cursorBody.vy = 0;
	for (auto i = 0; i < NUM_BODIES; i++)
	{
		bodies[i].x += bodies[i].vx * timeStep;
		bodies[i].y += bodies[i].vy * timeStep;
		vector2d a = {0, 0};
		for (auto j = 0; j < NUM_BODIES; j++)
		{
			if (j == i)
			{
				continue;
			}
			vector2d aj = gravitationalAcceleration(&bodies[i], &bodies[j]);
			a.x += aj.x;
			a.y += aj.y;
		}
		vector2d aj = gravitationalAcceleration(&bodies[i], &cursorBody);
		a.x += aj.x;
		a.y += aj.y;
		bodies[i].vx += a.x * timeStep;
		bodies[i].vy += a.y * timeStep;
		// solve boundary problem
		if (bodies[i].x > 1 || bodies[i].x < -1)
		{
			bodies[i].x = bodies[i].x > 0 ? 1.0 : -1.0;
			bodies[i].vx *= -collision_damping;
		}
		if (bodies[i].y > 1 || bodies[i].y < -1)
		{
			bodies[i].y = bodies[i].y > 0 ? 1.0 : -1.0;
			bodies[i].vy *= -collision_damping;
		}
	}
}