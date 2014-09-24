//+--------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//---------------------------------------------------------------------------

#pragma once

HRESULT RADv2RegisterEffects(ID2D1Factory1* argFactory);

void RADv2UnregisterEffects(ID2D1Factory1* argFactory);


// Effect GUIDs
const GUID CLSID_RADV2CheckColorDistance = { 0xb434f2af, 0x88c4, 0x49bd, 0x9e, 0x1b, 0x0d, 0xac, 0x0a, 0x3b, 0x76, 0x4c };
const GUID CLSID_RADV2ColorDistanceToRgb = { 0xc8571cd3, 0x148e, 0x489d, 0xb9, 0xbd, 0x3c, 0x61, 0xac, 0x3b, 0xea, 0x26 };
const GUID CLSID_RADV2DebugImageData =     { 0xbe389829, 0x5401, 0x491d, 0x93, 0x6e, 0x31, 0xf6, 0x25, 0xe5, 0xa5, 0xa8 };
const GUID CLSID_RADV2Invert =             { 0x3ab2dbb8, 0x5008, 0x4981, 0x85, 0xc9, 0x80, 0xab, 0xe8, 0x59, 0x90, 0x0c };
const GUID CLSID_RADV2LabColorComparer =   { 0x463d591b, 0x54d1, 0x49f7, 0xb3, 0x4b, 0x95, 0xe0, 0x18, 0x1b, 0x6a, 0x52 };
const GUID CLSID_RADV2LabImageComparer =   { 0x2a1e9e4f, 0x8d87, 0x4cb2, 0xbc, 0xad, 0x30, 0xb1, 0x3d, 0x10, 0xc5, 0xfd };
const GUID CLSID_RADV2RgbColorComparer =   { 0xc4b258cb, 0x452d, 0x4979, 0x9e, 0x8a, 0x42, 0xc9, 0x1f, 0x2e, 0x12, 0xd7 };
const GUID CLSID_RADV2RgbImageComparer =   { 0x9e9ba999, 0x2a62, 0x4689, 0x9d, 0x34, 0x99, 0x9b, 0x0b, 0x2b, 0x5e, 0x62 };
const GUID CLSID_RADV2RgbToLab =           { 0xd75238bf, 0xd0d9, 0x4150, 0xab, 0xa0, 0xaa, 0xd0, 0x9a, 0x89, 0x3b, 0x89 };
const GUID CLSID_RADV2Tint =               { 0x0490eed4, 0xc1c1, 0x4787, 0x97, 0x02, 0x7e, 0x72, 0x79, 0xda, 0x81, 0xc7 };

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_CHECK_COLOR_DISTANCE_PROP
//
//  Synopsis:
//      The enumeration of the Check Color Distance effect properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_CHECK_COLOR_DISTANCE_PROP
{
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT      = 0,
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X    = 1,
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y    = 2,
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT   = 3,
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X = 4,
    RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y = 5,

    RADV2_CHECK_COLOR_DISTANCE_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_CHECK_COLOR_DISTANCE_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_TINT_PROP
//
//  Synopsis:
//      The enumeration of the Tint effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_TINT_PROP
{
    RADV2_TINT_PROP_RED   = 0,
    RADV2_TINT_PROP_GREEN = 1,
    RADV2_TINT_PROP_BLUE  = 2,
    RADV2_TINT_PROP_ALPHA = 3,

    D2D1_TINT_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_TINT_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_LAB_COLOR_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Lab Color Comparer effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_LAB_COLOR_COMPARER_PROP
{
    RADV2_LAB_COLOR_COMPARER_PROP_L = 0,
    RADV2_LAB_COLOR_COMPARER_PROP_A = 1,
    RADV2_LAB_COLOR_COMPARER_PROP_B = 2,

    RADV2_LAB_COLOR_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_LAB_COLOR_COMPARER_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_DEBUG_IMAGE_DATA_PROP
//
//  Synopsis:
//      The enumeration of the Debug Image Data effect's top level properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_DEBUG_IMAGE_DATA_PROP
{
    RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA = 0,
    RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH = 1,

    RADV2_DEBUG_IMAGE_DATA_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_DEBUG_IMAGE_DATA_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_RGB_IMAGE_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Rgb Image Comparer effect properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_RGB_IMAGE_COMPARER_PROP
{
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_COUNT      = 0,
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_X    = 1,
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_Y    = 2,
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_COUNT   = 3,
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X = 4,
    RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y = 5,

    RADV2_RGB_IMAGE_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_RGB_IMAGE_COMPARER_PROP;

//+-----------------------------------------------------------------------------
//
//  Enum:
//      RADV2_RGB_COLOR_COMPARER_PROP
//
//  Synopsis:
//      The enumeration of the Rgb Color Comparer effect properties.
//
//------------------------------------------------------------------------------
typedef enum RADV2_RGB_COLOR_COMPARER_PROP
{
    RADV2_RGB_COLOR_COMPARER_PROP_RED                    = 0,
    RADV2_RGB_COLOR_COMPARER_PROP_GREEN                  = 1,
    RADV2_RGB_COLOR_COMPARER_PROP_BLUE                   = 2,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_COUNT      = 3,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_X    = 4,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_Y    = 5,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_COUNT   = 6,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X = 7,
    RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y = 8,

    RADV2_RGB_COLOR_COMPARER_PROP_FORCE_DWORD = 0xFFFFFFFF

} RADV2_RGB_COLOR_COMPARER_PROP;