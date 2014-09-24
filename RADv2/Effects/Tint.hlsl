//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the Tint effect.
//
//------------------------------------------------------------------------------

Texture2D InputTexture    : register(t0);

SamplerState InputSampler : register(s0);

cbuffer TintParameters    : register(b0)
{
    float4 tint : packoffset(c0);   
};


float4 Tint(
    float4 pos      : SV_POSITION,
    float4 posScene : SCENE_POSITION,
    float4 uv0      : TEXCOORD0
    ) : SV_Target
{
    float4 sample = InputTexture.Sample(InputSampler, uv0.xy);

    return float4(
        (tint.r - sample.r) * tint.a + sample.r,
        (tint.g - sample.g) * tint.a + sample.g,
        (tint.b - sample.b) * tint.a + sample.b,
        1.0);
}
