//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"

#include "CheckColorDistance.h"
#include "ColorDistanceToRgb.h"
#include "DebugImageData.h"
#include "Invert.h"
#include "LabColorComparer.h"
#include "LabImageComparer.h"
#include "RgbColorComparer.h"
#include "RgbImageComparer.h"
#include "RgbToLab.h"
#include "Tint.h"


HRESULT RADv2RegisterEffects(ID2D1Factory1* argFactory)
{
    HRESULT hResult;

    IFRNULL(argFactory);

    IFR(CheckColorDistance::Register(argFactory));
    IFR(ColorDistanceToRgb::Register(argFactory));
    IFR(DebugImageData::Register(argFactory));
    IFR(Invert::Register(argFactory));
    IFR(LabColorComparer::Register(argFactory));
    IFR(LabImageComparer::Register(argFactory));
    IFR(RgbColorComparer::Register(argFactory));
    IFR(RgbImageComparer::Register(argFactory));
    IFR(RgbToLab::Register(argFactory));
    IFR(Tint::Register(argFactory));

    return hResult;
}


void RADv2UnregisterEffects(ID2D1Factory1* argFactory)
{
    if (nullptr != argFactory)
    {
        // Ignore the return values
        argFactory->UnregisterEffect(CLSID_RADV2CheckColorDistance);
        argFactory->UnregisterEffect(CLSID_RADV2ColorDistanceToRgb);
        argFactory->UnregisterEffect(CLSID_RADV2DebugImageData);
        argFactory->UnregisterEffect(CLSID_RADV2Invert);
        argFactory->UnregisterEffect(CLSID_RADV2LabColorComparer);
        argFactory->UnregisterEffect(CLSID_RADV2LabImageComparer);
        argFactory->UnregisterEffect(CLSID_RADV2RgbColorComparer);
        argFactory->UnregisterEffect(CLSID_RADV2RgbImageComparer);
        argFactory->UnregisterEffect(CLSID_RADV2RgbToLab);
        argFactory->UnregisterEffect(CLSID_RADV2Tint);
    }
}