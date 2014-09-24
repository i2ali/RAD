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

namespace RADv2
{
    struct __declspec(uuid("{a51a9b7b-d531-477e-86c5-e2ed6653768f}")) IDwmSlicer;

    struct IDwmSlicer : public ISlicer
    {
    };
}
