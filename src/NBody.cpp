#include "main.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "NBody.h"
#include <math.h>



double getRandom(double min, double max)
{
	double r = (double)rand() / RAND_MAX;
	return r*(max - min) + min;
}

void rasterize(struct body* bodies, unsigned char* buffer)
{
	/**
	rasterize the bodies from x,y: (-1,-1) to (1,1) according to some kind of formula

	Note: You can change the code for better visualization
	As the following code can be parallelized, you can optimize this routine with CUDA.

	\param bodies A collection of bodies (located on the device).
	\param buffer the RGB buffer for screen display (located on the host).
	*/

	// clear the canvas
	memset(buffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT * 3 * sizeof(unsigned char));

	//TODO: copy the device memory to the host, and draw points on the canvas

	// Following is a sample of drawing a nice picture to the buffer.
	// You will know the index for each pixel.
	// The pixel value is from 0-255 so the data type is in unsigned char.

	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			// the R channel
			buffer[x * SCREEN_WIDTH * 3 + y * 3 + 0] = (unsigned char)(x + y) | (unsigned char)(x - y);				
			// the G channel
			buffer[x * SCREEN_WIDTH * 3 + y * 3 + 1] = (unsigned char)(sqrt((x-SCREEN_WIDTH/2)*(x - SCREEN_WIDTH / 2) + (y - SCREEN_HEIGHT / 2)*(y - SCREEN_HEIGHT / 2)) * 1.5);	
			// the B channel
			buffer[x * SCREEN_WIDTH * 3 + y * 3 + 2] = (unsigned char)((x % 255) | (y % 255) );				 
		}
	}

	

}

struct body* initializeNBodyCuda()
{
	/**
	initialize the bodies, then copy to the CUDA device memory
	return the device pointer so that it can be reused in the NBodyTimestepCuda function.
	*/

	// initialize the position and velocity
	// you can implement own initial conditions to form a sprial/ellipse galaxy, have fun.
	
	return NULL;
}


void NBodyTimestepCuda(struct body* bodies, double rx, double ry, bool cursor)
{
	/**
	Compute a time step on the CUDA device. 
	TODO: correctly manage the device memory, compute the time step with proper block/threads 

	\param bodies A collection of bodies (located on the device).
	\param rx position x of the cursor.
	\param ry position y of the cursor.
	\param cursor Enable the mouse interaction if true (adding a weight = cursor_weight body in the computation).
	*/


}