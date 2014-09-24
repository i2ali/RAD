//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the LabImageComparer effect.
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

Texture2D<float4> InputTexture0 : register(t0);
SamplerState      InputSampler0 : register(s0);

Texture2D<float4> InputTexture1 : register(t1);
SamplerState      InputSampler1 : register(s1);


float4 LabImageComparer(PSInput input) : SV_Target
{
    float4 sample0 = InputTexture0.Sample(InputSampler0, input.uv0.xy);
    float4 sample1 = InputTexture1.Sample(InputSampler1, input.uv1.xy);

    //
    // The LAB color must be converted to LCH to be compared
    //

    float Cab1 = sqrt(sample0.y * sample0.y + sample0.z * sample0.z);
    float Cab2 = sqrt(sample1.y * sample1.y + sample1.z * sample1.z);
    float deltaC = Cab1 - Cab2;

    float deltaL = sample0.x - sample1.x;
    float deltaA = sample0.y - sample1.y;
    float deltaB = sample0.z - sample1.z;

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
