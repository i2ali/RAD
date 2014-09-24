//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the pixel shader for the RgbToLab effect.
//
//------------------------------------------------------------------------------

#define D65_XyzWhitePoint_X 0.95047
#define D65_XyzWhitePoint_Y 1.0
#define D65_XyzWhitePoint_Z 1.08883

Texture2D<float4> InputTexture : register(t0);
SamplerState      InputSampler : register(s0);


// Computes Y/Yn^(1/3) with a limit case for small values of Y.
float f(float Y, float Yn)
{
    const float a = 0.008856452;
    float t = Y / Yn;

    if (t > a)
    {
        return pow(abs(t), 1.0 / 3.0);
    }
    else
    {
        return (841.0 / 108.0) * t + (16.0 / 116.0);
    }
}


float4 RgbToLab(
    float4 pos      : SV_POSITION,
    float4 posScene : SCENE_POSITION,
    float4 uv0      : TEXCOORD0
    ) : SV_Target
{
    float4 sample = InputTexture.Sample(InputSampler, uv0.xy);

    //
    // Convert from RGB to XYZ
    //

    if (sample.r <= 0.04045)
    {
        sample.r = sample.r / 12.92;
    }
    else
    {
        sample.r = pow((sample.r + 0.055) / 1.055, 2.4);
    }

    if (sample.g <= 0.04045)
    {
        sample.g = sample.g / 12.92;
    }
    else
    {
        sample.g = pow((sample.g + 0.055) / 1.055, 2.4);
    }

    if (sample.b <= 0.04045)
    {
        sample.b = sample.b / 12.92;
    }
    else
    {
        sample.b = pow((sample.b + 0.055) / 1.055, 2.4);
    }

    float x = (0.412453 * sample.r + 0.357580 * sample.g + 0.180423 * sample.b);
    float y = (0.212671 * sample.r + 0.715160 * sample.g + 0.072169 * sample.b);
    float z = (0.019334 * sample.r + 0.119193 * sample.g + 0.950227 * sample.b);

    //
    // Convert from XYZ to LAB
    //

    float f_x_xn = f(x, D65_XyzWhitePoint_X);
    float f_y_yn = f(y, D65_XyzWhitePoint_Y);
    float f_z_zn = f(z, D65_XyzWhitePoint_Z);

    return float4(
        116.0 * (f_y_yn) - 16.0,
        500.0 * (f_x_xn - f_y_yn),
        200.0 * (f_y_yn - f_z_zn),
        sample.a);
}
