#include <cuda.h>
#include <cuda_runtime.h>
#include <iostream>
#include "main.h"
#include "NBody.cuh"
#include <algorithm>
#include <math.h>
#include <iostream>

#define BLOCKSIZE (128)

using namespace std;


/*cuda shared memory*/
template<class T>
struct SharedMemory
{
	__device__ inline operator       T *()
	{
		extern __shared__ int __smem[];
		return (T *)__smem;
	}

	__device__ inline operator const T *() const
	{
		extern __shared__ int __smem[];
		return (T *)__smem;
	}
};

double getRandom(double min, double max)
{
	float r = (float)rand() / RAND_MAX;
	return r * (max - min) + min;
}

struct body *initializeNBodyCuda(char method)
{
	/**
	initialize the bodies, then copy to the CUDA device memory
	return the device pointer so that it can be reused in the NBodyTimestepCuda function.
	*/

	/*Using CUDA Unified Memory to automatically handle memory operation*/
	body *bodies;
	const double PI = 3.14159265;
	cudaMallocManaged((void **) &bodies, sizeof(body) * (NUM_BODIES+1));/*using bodies[NUM_BODIES] to store cursor body*/
	if(method == '0')
	{
		cout<<"Initialized by random position and mass"<<endl;
		for (auto i = 0; i < NUM_BODIES+1; i++)
		{
			bodies[i].m = getRandom(0,1.0);
			bodies[i].x = getRandom(-1,1);
			bodies[i].y = getRandom(-1,1);
			bodies[i].vx = 0;
			bodies[i].vy = 0;
		}
	}
	else
	{
		cout<<"Initialized by random velocity and mass"<<endl;
		for (auto i = 0; i < NUM_BODIES+1; i++)
		{
			bodies[i].m = getRandom(0,1.0);
			bodies[i].x = 0;
			bodies[i].y = 0;
			auto radius = getRandom(0,0.05);
			auto theta = getRandom(0,2*PI);
			bodies[i].vx = radius*cos(theta);
			bodies[i].vy = radius*sin(theta);
		}
	}
	return bodies;
}

unsigned char* initCanvas()
{
	/*Using CUDA Unified Memory to automatically handle memory operation, such that we can parallel rasterasize function*/
	unsigned char* buffer;
	cudaMallocManaged((void **) &buffer, sizeof(unsigned char) * SCREEN_WIDTH * SCREEN_HEIGHT * 3);
	return buffer;
}

__global__
void rasterizeKernel(unsigned char* buffer, body* bodies)
{
	/*rasterize kernel*/
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if (index >= NUM_BODIES)
	{
		return;
	}
	int x = (int) lround(SCREEN_WIDTH * ((bodies[index].x + 1) / 2.0));
	int y = (int) lround(SCREEN_HEIGHT * ((bodies[index].y + 1) / 2.0));
	buffer[x * SCREEN_WIDTH * 3 + y * 3 + 0] = 0XFF;
	buffer[x * SCREEN_WIDTH * 3 + y * 3 + 1] = 0XFE;
	buffer[x * SCREEN_WIDTH * 3 + y * 3 + 2] = 0XE5;
}

__global__
void trailKernel(unsigned char* buffer)
{
	/*trail effection kernel*/
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if (index >= SCREEN_WIDTH * SCREEN_HEIGHT * 3)
	{
		return;
	}
	buffer[index] = (unsigned char)(buffer[index] * 0.8);
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
	/*trail effection*/
	dim3 blockSize(BLOCKSIZE);
	dim3 gridSize((SCREEN_WIDTH * SCREEN_HEIGHT * 3 + blockSize.x - 1) / blockSize.x);
	trailKernel<<< gridSize, blockSize>>>(buffer);
	cudaDeviceSynchronize();
	/*rasterize*/
	dim3 gridSize2((NUM_BODIES + blockSize.x - 1) / blockSize.x);
	rasterizeKernel<<< gridSize2, blockSize>>>(buffer,bodies);
	cudaDeviceSynchronize();
}


void freeCudaMem(void* p)
{
	/*ferr malloced memory mannually*/
	cudaFree(p);
}

__device__
float2 bodyBodyInteraction(body selfBody, body body2, float2 acc)
{
	/*calculate accelaration between 2 bodies*/
	float2 r;
	r.x = body2.x - selfBody.x;
	r.y = body2.y - selfBody.y;
	float disSquare = r.x * r.x + r.y * r.y + eps * eps;
	float tmp = G * body2.m / rsqrt(disSquare * disSquare * disSquare);
	acc.x += r.x * tmp;
	acc.y += r.y * tmp;
	return acc;
}


__device__
float2 computeBodyAccel(body selfBody, body* bodies, int numTiles)
{
	/*compute the body's accelaration under all the other bodies' effection*/
	body *sharedBodies = SharedMemory<body>();
	float2 acc; acc.x =0; acc.y=0;
	for (int tile = 0; tile < numTiles; tile++)
	{
		sharedBodies[threadIdx.x] = bodies[tile * blockDim.x + threadIdx.x];
		__syncthreads();
		#pragma unroll 128
		for (unsigned int counter = 0; counter < blockDim.x; counter++)
		{
			acc = bodyBodyInteraction(selfBody, sharedBodies[counter], acc);
		}
		__syncthreads();
	}
	return acc;
}


__global__ void
integrateBodies(body* bodies, float deltaTime,int numTiles)
{
	/*N-Body kernel*/
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if (index >= NUM_BODIES)
	{
		return;
	}
	bodies[index].x += bodies[index].vx *deltaTime;
	bodies[index].y += bodies[index].vy *deltaTime;

	body b1 = bodies[index];
	float2 accel = computeBodyAccel(b1,bodies,numTiles);
	bodies[index].vx += accel.x * deltaTime;
	bodies[index].vy += accel.y * deltaTime;
	// solve boundary problem
	if (bodies[index].x > 1 || bodies[index].x < -1)
	{
		bodies[index].vx *= (-collision_damping);
		bodies[index].x = (bodies[index].x > 0 ? 1.0 : -1.0);
	}
	if (bodies[index].y > 1 || bodies[index].y < -1)
	{
		bodies[index].vy *= (-collision_damping);
		bodies[index].y = (bodies[index].y > 0 ? 1.0 : -1.0);
	}
}



void NBodyTimestepCuda(struct body *bodies, float rx, float ry, bool cursor)
{
	/**
	Compute a time step on the CUDA device.
	TODO: correctly manage the device memory, compute the time step with proper block/threads

	\param bodies A collection of bodies (located on the device).
	\param rx position x of the cursor.
	\param ry position y of the cursor.
	\param cursor Enable the mouse interaction if true (adding a weight = cursor_weight body in the computation).
	*/
	float timeStep = 1;
	/*cursor body*/
	bodies[NUM_BODIES].m = cursor_weight * (float) cursor; // convert bool condition to scale multiply to avoid branch selection
	bodies[NUM_BODIES].x = rx;
	bodies[NUM_BODIES].y = ry;
	bodies[NUM_BODIES].vx = 0;
	bodies[NUM_BODIES].vy = 0;
	/*N-Body cuda kernel exec*/
	int numTiles = (NUM_BODIES + BLOCKSIZE) / BLOCKSIZE;/*add 1 to store cursor body*/
	dim3 blockSize(BLOCKSIZE);
	dim3 gridSize(numTiles);
	int sharedMemSize = BLOCKSIZE * sizeof(body);
	integrateBodies<<< gridSize, blockSize, sharedMemSize >>>(bodies,timeStep,numTiles);
	cudaDeviceSynchronize();
}
