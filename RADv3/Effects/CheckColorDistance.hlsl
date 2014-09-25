//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the compute shader for the CheckColorDistance effect.
//
//------------------------------------------------------------------------------

#define LAB_JND 2.3

Texture2D<float4>         ColorDistanceTexture : register(t0);
SamplerState              InputSampler         : register(s0);
RWStructuredBuffer<uint> Result;


// These are set by DImage
// Output contract for an analysis transform
cbuffer systemConstants : register(b0)
{
    float2 sceneToInput0X;
    float2 sceneToInput0Y;
};

cbuffer customConstants : register(b1)
{
    uint stride : packoffset(c0);
};

// Helper function provided by DImage
float2 ConvertInput0SceneToTexelSpace(float2 inputScenePosition)
{
    float2 ret;
    ret.x = inputScenePosition.x * sceneToInput0X[0] + sceneToInput0X[1];
    ret.y = inputScenePosition.y * sceneToInput0Y[0] + sceneToInput0Y[1];

    return ret;
}


[numthreads( 1, 1, 1 )]
void CheckColorDistance(
    uint3 globalThreadId         : SV_DispatchThreadID,
    uint3 localThreadId          : SV_GroupThreadID,
    uint  localThreadIdFlattened : SV_GroupIndex,
    uint3 threadGroupId          : SV_GroupID)
{
    uint x = threadGroupId.x;
    uint y = threadGroupId.y;

    uint offset = (y * stride) + x;

    float2 pos = ConvertInput0SceneToTexelSpace(float2(x + 0.5, y + 0.5));
    float4 inputPixel = ColorDistanceTexture.SampleLevel(InputSampler, pos, 0);

    if (inputPixel.x >= LAB_JND)
    {
        Result[offset] = 0;
    }
    else
    {
        Result[offset] = 1;
    }
}