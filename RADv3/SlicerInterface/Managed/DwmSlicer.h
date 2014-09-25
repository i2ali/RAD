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

#include "IDwmSlicer.h"
#include "SlicerBase.h"
#include "SlicerFactory.h"

namespace RAD { namespace Managed
{
    private ref class DwmSlicer : public IDwmSlicer, public SlicerBase
    {
    // Constructor
    public:
        DwmSlicer(
            RAD::IDwmSlicer* argDwmSlicer,
            RAD::ISlicerFactory* argSlicerFactory,
            HMODULE argSlicerInterfaceModule);

    };
} }