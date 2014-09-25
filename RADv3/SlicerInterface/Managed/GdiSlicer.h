//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      The DwmSlicer uses the SlicerControl method to capture slices of the
//      screen.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

#include "IGdiSlicer.h"
#include "SlicerBase.h"
#include "SlicerFactory.h"

namespace RAD { namespace Managed
{
    private ref class GdiSlicer : public IGdiSlicer, public SlicerBase
    {
    // Constructor
    public:
        GdiSlicer(
            RAD::IGdiSlicer* argGdiSlicer,
            RAD::ISlicerFactory* argSlicerFactory,
            HMODULE argSlicerInterfaceModule);

    };
} }