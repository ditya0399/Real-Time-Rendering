__global__ void sinewave_vbo_kernel(float4 *pos,unsigned int width,unsigned int height,float animTime)
{
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
    float u = x/(float)width;
    float v = y/(float)height;
    u = (u * 2.0) - 1.0;
    v = (v * 2.0) - 1.0;
    float frequency = 4.0;
    float w = sinf(frequency * u + animTime) * cosf(frequency * v + animTime) * 0.5;
    pos[y * width + x] = make_float4(u,w,v,1.0);

}


void launchCudaKernel(float4 *pos,unsigned int Mesh_Width,unsigned int Mesh_Height,float Time)
{
    dim3 block(8,8,1);
    dim3 grid(Mesh_Width/block.x,Mesh_Height/block.y);
    sinewave_vbo_kernel<<<grid,block>>>(pos,Mesh_Width,Mesh_Height,Time);

}