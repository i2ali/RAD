//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines the compute shader for the RgbImageComparer effect.
//
//------------------------------------------------------------------------------

#define LAB_JND 2.3

#define D65_XyzWhitePoint_X 0.95047
#define D65_XyzWhitePoint_Y 1.0
#define D65_XyzWhitePoint_Z 1.08883

#define K_L 1.0
#define K_C 0.045
#define K_H 0.015

Texture2D<float4>        InputTexture1 : register(t0);
SamplerState             InputSampler1 : register(s0);
Texture2D<float4>        InputTexture2 : register(t1);
SamplerState             InputSampler2 : register(s1);

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
    int4   resultRect : packoffset(c0);
};


// Helper function provided by DImage
float2 ConvertInput0SceneToTexelSpace(float2 inputScenePosition)
{
    float2 ret;
    ret.x = inputScenePosition.x * sceneToInput0X[0] + sceneToInput0X[1];
    ret.y = inputScenePosition.y * sceneToInput0Y[0] + sceneToInput0Y[1];

    return ret;
}


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


float3 RgbToLab(float4 argRgbColor)
{
    //
    // Convert from RGB to XYZ
    //

    if (argRgbColor.r <= 0.04045)
    {
        argRgbColor.r = argRgbColor.r / 12.92;
    }
    else
    {
        argRgbColor.r = pow((argRgbColor.r + 0.055) / 1.055, 2.4);
    }

    if (argRgbColor.g <= 0.04045)
    {
        argRgbColor.g = argRgbColor.g / 12.92;
    }
    else
    {
        argRgbColor.g = pow((argRgbColor.g + 0.055) / 1.055, 2.4);
    }

    if (argRgbColor.b <= 0.04045)
    {
        argRgbColor.b = argRgbColor.b / 12.92;
    }
    else
    {
        argRgbColor.b = pow((argRgbColor.b + 0.055) / 1.055, 2.4);
    }

    float x = (0.412453 * argRgbColor.r + 0.357580 * argRgbColor.g + 0.180423 * argRgbColor.b);
    float y = (0.212671 * argRgbColor.r + 0.715160 * argRgbColor.g + 0.072169 * argRgbColor.b);
    float z = (0.019334 * argRgbColor.r + 0.119193 * argRgbColor.g + 0.950227 * argRgbColor.b);

    //
    // Convert from XYZ to LAB
    //

    float f_x_xn = f(x, D65_XyzWhitePoint_X);
    float f_y_yn = f(y, D65_XyzWhitePoint_Y);
    float f_z_zn = f(z, D65_XyzWhitePoint_Z);

    return float3(
        116.0 * (f_y_yn) - 16.0,
        500.0 * (f_x_xn - f_y_yn),
        200.0 * (f_y_yn - f_z_zn));
}


float Distance(float3 argLabColor1, float3 argLabColor2)
{
    //
    // The LAB color must be converted to LCH to be compared
    //

    float Cab1 = sqrt(argLabColor1.y * argLabColor1.y + argLabColor1.z * argLabColor1.z);
    float Cab2 = sqrt(argLabColor2.y * argLabColor2.y + argLabColor2.z * argLabColor2.z);
    float deltaC = Cab1 - Cab2;

    float deltaL = argLabColor1.x - argLabColor2.x;
    float deltaA = argLabColor1.y - argLabColor2.y;
    float deltaB = argLabColor1.z - argLabColor2.z;

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

    return distance;
}


[numthreads( 1, 1, 1 )]
void RgbImageComparer(
    uint3 globalThreadId         : SV_DispatchThreadID, 
    uint3 localThreadId          : SV_GroupThreadID, 
    uint  localThreadIdFlattened : SV_GroupIndex, 
    uint3 threadGroupId          : SV_GroupID)
{
    uint matchCount = 0;
    uint matchFirstX = 0;
    uint matchFirstY = 0;
    uint mismatchCount = 0;
    uint mismatchFirstX = 0;
    uint mismatchFirstY = 0;

    for (int y = resultRect[1]; y < resultRect[3]; y++)
    {
        for (int x = resultRect[0]; x < resultRect[2]; x++)
        {
            float2 pos = ConvertInput0SceneToTexelSpace(float2(x+0.5, y+0.5));
            float4 inputPixel1 = InputTexture1.SampleLevel(InputSampler1, pos, 0);
            float4 inputPixel2 = InputTexture2.SampleLevel(InputSampler2, pos, 0);

            float distance = Distance(RgbToLab(inputPixel1), RgbToLab(inputPixel2));

            if (distance >= LAB_JND)
            {
                if (0 == mismatchCount)
                {
                    mismatchFirstX = x;
                    mismatchFirstY = y;
                }

                mismatchCount++;
            }
            else
            {
                if (0 == matchCount)
                {
                    matchFirstX = x;
                    matchFirstY = y;
                }

                matchCount++;
            }
        }
    }

    Result[0] = matchCount;
    Result[1] = matchFirstX;
    Result[2] = matchFirstY;
    Result[3] = mismatchCount;
    Result[4] = mismatchFirstX;
    Result[5] = mismatchFirstY;
}
