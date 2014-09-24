//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the Invert effect.
//
//------------------------------------------------------------------------------

Texture2D InputTexture    : register(t0);

SamplerState InputSampler : register(s0);

float4 Invert(
    float4 pos      : SV_POSITION,
    float4 posScene : SCENE_POSITION,
    float4 uv0      : TEXCOORD0
    ) : SV_Target
{
    unorm float4 sample = InputTexture.Sample(InputSampler, uv0.xy);

    return float4(
        1.0 - sample.r,
        1.0 - sample.g,
        1.0 - sample.b,
        1.0);
}
