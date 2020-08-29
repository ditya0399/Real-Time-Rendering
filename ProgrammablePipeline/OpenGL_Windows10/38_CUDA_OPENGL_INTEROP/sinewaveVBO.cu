__global__ void sinewave_vbo_kernel(float4 *pos,unsigned int width,unsigned int height,float time, uchar4 *colorPos)
{
	unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	// calculate uv coordinates
	float u = x / (float)width;
	float v = y / (float)height;
	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	// calculate simple sine wave pattern
	float freq = 4.0f;
	float w = sinf(u*freq + time) * cosf(v*freq + time) * 0.5f;

	// write output vertex
	pos[y*width + x] = make_float4(u, w, v, 1.0f);

	colorPos[y*width + x].w = 0;
	colorPos[y*width + x].x = 255.f *0.5*(1.f + sinf(w + x));
	colorPos[y*width + x].y = 255.f *0.5*(1.f + sinf(x)*cosf(y));
	colorPos[y*width + x].z = 255.f *0.5*(1.f + sinf(w + time / 10.f));


}


void launchCudaKernel(float4 *pos,unsigned int Mesh_Width,unsigned int Mesh_Height,float Time, uchar4 *colorPos)
{
    dim3 block(8,8,1);
    dim3 grid(Mesh_Width/block.x,Mesh_Height/block.y,1);
    sinewave_vbo_kernel<<<grid,block>>>(pos,Mesh_Width,Mesh_Height,Time,colorPos);

}