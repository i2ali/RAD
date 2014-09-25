//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicer.h"

using namespace RAD::Managed;


// Constructor
DwmSlicer::DwmSlicer(
    RAD::IDwmSlicer* argDwmSlicer,
    RAD::ISlicerFactory* argSlicerFactory,
    HMODULE argSlicerInterfaceModule)
    : SlicerBase(argDwmSlicer, argSlicerFactory, argSlicerInterfaceModule)
{
}