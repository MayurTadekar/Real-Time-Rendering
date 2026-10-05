

#include	<cuda.h>
#include	<stdio.h>

extern	FILE* gpFile;

__global__ void SineWave_Kernel(float4* pPosition, 
								unsigned int width,
								unsigned int height,
								float time)
{
	unsigned short x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned short y = blockIdx.y * blockDim.y + threadIdx.y;

	float u = x / (float)width;
	float v = y / (float)height;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;
	float frequency = 4.0f;
	float w = sinf(u * frequency + time ) * cosf(v * frequency + time) * 0.5f;

	pPosition[y * width + x] = make_float4(u, w, v, 1.0f);
}

void launchSineWaveFromCUDAKernel(	float4* pPosition,
									unsigned int gMeshWidth,
									unsigned int gMeshHeight,
									float time)
{
	dim3 block = dim3(8, 8, 1);
	dim3 grid = dim3( ceil(gMeshWidth / block.x), ceil(gMeshHeight / block.y), 1);

	SineWave_Kernel <<<grid, block>>> (pPosition, gMeshWidth, gMeshHeight, time);

}
