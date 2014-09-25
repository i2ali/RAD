//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This is an interface for a slicer using the DWM.
//
//------------------------------------------------------------------------------

#pragma once

#include "ISlicer.h"

namespace RAD
{
    struct __declspec(uuid("{743731b7-f557-43b5-bd8f-95f3c38a9b25}")) IDwmSlicer;

    struct IDwmSlicer : public ISlicer
    {
    };
}
