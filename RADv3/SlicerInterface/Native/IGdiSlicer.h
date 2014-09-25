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

namespace RAD
{
    struct __declspec(uuid("{71c38f02-0d1f-42e6-a29c-3a1fb67b73d2}")) IGdiSlicer;

    struct IGdiSlicer : public ISlicer
    {
    };
}