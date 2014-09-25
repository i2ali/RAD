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
// D2D Headers
//

#include <d2d1_1.h>

//
// Slicer Headers
//

#include "ComputeShaderModel.h"
#include "ISlicerFactory.h"

//
// Exports
//

HRESULT CreateSlicerFactory(
    RAD::ISlicerFactory** argSlicerFactory,
    RAD::ComputeShaderModel argComputeShaderModelRequired = RAD::ComputeShaderModel::None);