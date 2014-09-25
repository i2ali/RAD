//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the Lab Color Comparer effect.
//
//------------------------------------------------------------------------------

#define K_L 1.0
#define K_C 0.045
#define K_H 0.015

struct PSInput
{
    float4 pos      : SV_POSITION;
    float4 posScene : SCENE_POSITION;
    float4 uv0      : TEXCOORD0;
    float4 uv1      : TEXCOORD1;
};

cbuffer LabColorComparerParameters : register(b0)
{
    float4 inputColor : packoffset(c0);   
};

Texture2D    InputTexture : register(t0);
SamplerState InputSampler : register(s0);


float4 LabColorComparer(PSInput input) : SV_Target
{
    float4 sample = InputTexture.Sample(InputSampler, input.uv0.xy);

    //
    // The LAB color must be converted to LCH to be compared
    //

    float Cab1 = sqrt(dot(sample.yz, sample.yz));
    float Cab2 = sqrt(dot(inputColor.yz, inputColor.yz));
    float deltaC = Cab1 - Cab2;

    float deltaL = sample.x - inputColor.x;
    float deltaA = sample.y - inputColor.y;
    float deltaB = sample.z - inputColor.z;

    float p = deltaA * deltaA + deltaB * deltaB;
    float n = deltaC * deltaC;

    float deltaH = (p - n) > 0.0 ? sqrt(p - n) : 0.0;

    float divisorL = K_L;
    float divisorC = 1.0 + K_C * Cab1;
    float divisorH = 1.0 + K_H * Cab1;

    float distance = sqrt(
        (deltaL * deltaL) / (divisorL * divisorL) +
        (deltaC * deltaC) / (divisorC * divisorC) +
        (deltaH * deltaH) / (divisorH * divisorH));

    return float4(distance, 0.0, 0.0, 1.0);
}
