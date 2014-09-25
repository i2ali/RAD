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


HRESULT RADRegisterEffects(ID2D1Factory1* argFactory)
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


void RADUnregisterEffects(ID2D1Factory1* argFactory)
{
    if (nullptr != argFactory)
    {
        // Ignore the return values
        argFactory->UnregisterEffect(CLSID_RADCheckColorDistance);
        argFactory->UnregisterEffect(CLSID_RADColorDistanceToRgb);
        argFactory->UnregisterEffect(CLSID_RADDebugImageData);
        argFactory->UnregisterEffect(CLSID_RADInvert);
        argFactory->UnregisterEffect(CLSID_RADLabColorComparer);
        argFactory->UnregisterEffect(CLSID_RADLabImageComparer);
        argFactory->UnregisterEffect(CLSID_RADRgbColorComparer);
        argFactory->UnregisterEffect(CLSID_RADRgbImageComparer);
        argFactory->UnregisterEffect(CLSID_RADRgbToLab);
        argFactory->UnregisterEffect(CLSID_RADTint);
    }
}
