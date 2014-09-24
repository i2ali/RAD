//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Provides the includes and the main entry point for the slicer interface
//      library.  Call CreateSlicerFactory to get started.
//
//------------------------------------------------------------------------------

#pragma once

//
// These are the slicer headers
//

#include "ComputeShaderModel.h"
#include "IDwmSlicer.h"
#include "IGdiSlicer.h"
#include "ISlicerFactory.h"

//
// This function creates a slicer factory
//
HRESULT CreateSlicerFactory(RADv2::ISlicerFactory** argSlicerFactory, RADv2::ComputeShaderModel argComputeShaderModelRequired = RADv2::ComputeShaderModel::None);

//
// Helper for StartCapture
//

namespace RADv2
{
    inline RECT Rect(LONG left, LONG top, LONG right, LONG bottom)
    {
        RECT r;
        r.left = left;
        r.top = top;
        r.right = right;
        r.bottom = bottom;

        return r;
    }
}
