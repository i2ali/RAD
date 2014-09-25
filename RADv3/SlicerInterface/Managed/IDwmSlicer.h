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

namespace RAD { namespace Managed
{
    public interface class IDwmSlicer : public ISlicer
    {
    };
} }