//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This is an interface for a slicer using GDI.
//
//------------------------------------------------------------------------------

#pragma once

#include "ISlicer.h"

namespace RADv2
{
    struct __declspec(uuid("{04ccd931-e84b-4cbc-860b-c0d2dc143e80}")) IGdiSlicer;

    struct IGdiSlicer : public ISlicer
    {
    };
}