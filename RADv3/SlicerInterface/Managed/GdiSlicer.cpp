//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "GdiSlicer.h"

using namespace RAD::Managed;


// Constructor
GdiSlicer::GdiSlicer(
    RAD::IGdiSlicer* argGdiSlicer,
    RAD::ISlicerFactory* argSlicerFactory,
    HMODULE argSlicerInterfaceModule)
    : SlicerBase(argGdiSlicer, argSlicerFactory, argSlicerInterfaceModule)
{
}