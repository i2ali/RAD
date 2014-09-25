//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the ColorDistanceToRgb effect.
//
//------------------------------------------------------------------------------

Texture2D InputTexture    : register(t0);

SamplerState InputSampler : register(s0);

#define JND 2.3
#define WRN 0.5
#define ColorGood float3(0.0, 1.0, 0.0)
#define ColorBad float3(1.0, 0.0, 0.0)
#define ColorWarn float3(1.0, 1.0, 0.0)

float4 ColorDistanceToRgb(
    float4 pos      : SV_POSITION,
    float4 posScene : SCENE_POSITION,
    float4 uv0      : TEXCOORD0
    ) : SV_Target
{
    float4 sample = InputTexture.Sample(InputSampler, uv0.xy);

    if (sample.x < WRN)
    {
        return float4(ColorGood, 1.0);
    }
    else if (sample.x < JND)
    {
        return float4(ColorWarn, 1.0);
    }
    else
    {
        return float4(ColorBad, 1.0);
    }
}
