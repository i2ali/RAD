//+--------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//---------------------------------------------------------------------------

#pragma once

HRESULT RADRegisterEffects(ID2D1Factory1* argFactory);

void RADUnregisterEffects(ID2D1Factory1* argFactory);


// Effect GUIDs
DEFINE_GUID(CLSID_RADCheckColorDistance,    0xe849cf11, 0xbbb2, 0x4f14, 0xa3, 0x28, 0x43, 0x63, 0x36, 0x84, 0x13, 0x19);
DEFINE_GUID(CLSID_RADColorDistanceToRgb,    0x6c79edbb, 0x8c0f, 0x434d, 0x8e, 0xb9, 0x96, 0x55, 0x87, 0xda, 0xd9, 0xfe);
DEFINE_GUID(CLSID_RADDebugImageData,        0x02972ea8, 0x303d, 0x47e4, 0xb2, 0x1c, 0x59, 0x23, 0x7b, 0xdd, 0xfa, 0xbb);
DEFINE_GUID(CLSID_RADInvert,                0x1c9366a9, 0xbb1c, 0x4963, 0x90, 0x9a, 0x04, 0xcc, 0x02, 0xc1, 0x26, 0x3d);
DEFINE_GUID(CLSID_RADLabColorComparer,      0x538bc449, 0x72aa, 0x43d6, 0xa4, 0x8a, 0x33, 0x38, 0x9b, 0x42, 0x21, 0xfc);
DEFINE_GUID(CLSID_RADLabImageComparer,      0xdf6c741c, 0xec97, 0x4dc0, 0xbb, 0xcd, 0xa4, 0xc2, 0x6f, 0xcb, 0xe2, 0x5f);
DEFINE_GUID(CLSID_RADRgbColorComparer,      0x4f242eeb, 0x5793, 0x4dbf, 0xaa, 0xce, 0x7d, 0xf7, 0x33, 0xf6, 0x39, 0x1d);
DEFINE_GUID(CLSID_RADRgbImageComparer,      0xb2aa9d6c, 0x7bdf, 0x42da, 0x81, 0xc6, 0x01, 0xe0, 0x43, 0xb6, 0xee, 0xe3);
DEFINE_GUID(CLSID_RADRgbToLab,              0xbd752287, 0x06d6, 0x403a, 0xa6, 0x1a, 0x2c, 0x27, 0x5c, 0x40, 0x85, 0xc6);
DEFINE_GUID(CLSID_RADTint,                  0xe755101c, 0x0023, 0x4d22, 0x83, 0x5d, 0xf6, 0x81, 0x53, 0x66, 0x02, 0x3f);


//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_CHECK_COLOR_DISTANCE_PROP
//
//  Synopsis:
//      The enumeration of the Check Color Distance effect properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_CHECK_COLOR_DISTANCE_PROP
{
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT      = 0,
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X    = 1,
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y    = 2,
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT   = 3,
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X = 4,
    RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y = 5,

    RAD_CHECK_COLOR_DISTANCE_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_CHECK_COLOR_DISTANCE_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_TINT_PROP
//
//  Synopsis:
//      The enumeration of the Tint effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_TINT_PROP
{
    RAD_TINT_PROP_RED   = 0,
    RAD_TINT_PROP_GREEN = 1,
    RAD_TINT_PROP_BLUE  = 2,
    RAD_TINT_PROP_ALPHA = 3,

    D2D1_TINT_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_TINT_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_LAB_COLOR_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Lab Color Comparer effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_LAB_COLOR_COMPARER_PROP
{
    RAD_LAB_COLOR_COMPARER_PROP_L = 0,
    RAD_LAB_COLOR_COMPARER_PROP_A = 1,
    RAD_LAB_COLOR_COMPARER_PROP_B = 2,

    RAD_LAB_COLOR_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_LAB_COLOR_COMPARER_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_DEBUG_IMAGE_DATA_PROP
//
//  Synopsis:
//      The enumeration of the Debug Image Data effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_DEBUG_IMAGE_DATA_PROP
{
    RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA = 0,
    RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH = 1,

    RAD_DEBUG_IMAGE_DATA_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_DEBUG_IMAGE_DATA_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_RGB_IMAGE_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Rgb Image Comparer effect properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_RGB_IMAGE_COMPARER_PROP
{
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_COUNT      = 0,
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_X    = 1,
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_Y    = 2,
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_COUNT   = 3,
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X = 4,
    RAD_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y = 5,

    RAD_RGB_IMAGE_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_RGB_IMAGE_COMPARER_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RAD_RGB_COLOR_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Rgb Color Comparer effect properties.
//
//------------------------------------------------------------------------------
typedef enum RAD_RGB_COLOR_COMPARER_PROP
{
    RAD_RGB_COLOR_COMPARER_PROP_RED                    = 0,
    RAD_RGB_COLOR_COMPARER_PROP_GREEN                  = 1,
    RAD_RGB_COLOR_COMPARER_PROP_BLUE                   = 2,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_COUNT      = 3,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_X    = 4,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_Y    = 5,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_COUNT   = 6,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X = 7,
    RAD_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y = 8,

    RAD_RGB_COLOR_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RAD_RGB_COLOR_COMPARER_PROP;
