

#ifndef PARTICLES_KERNEL_H
#define PARTICLES_KERNEL_H


#include "include/vector_types.h"
typedef unsigned int uint;

struct SimParams
{
    float3 gravity;
    float globalDamping;
    float noiseFreq;
    float noiseAmp;
    float3 cursorPos;

    float time;
    float3 noiseSpeed;
};

struct float4x4
{
    float m[16];
};

#endif
