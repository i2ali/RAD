//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "SystemBitmap.h"

using namespace RAD::Managed;

//
// Constructor
//

SystemBitmap::SystemBitmap(
    array<System::Byte>^ argData,
    unsigned int argWidth,
    unsigned int argHeight,
    unsigned int argStride)
    : mData(argData), mWidth(argWidth), mHeight(argHeight), mStride(argStride)
{
}


//
// Public Properties
//

array<System::Byte>^ SystemBitmap::Data::get(void)
{
    return this->mData;
}


unsigned int SystemBitmap::Width::get(void)
{
    return this->mWidth;
}


unsigned int SystemBitmap::Height::get(void)
{
    return this->mHeight;
}


unsigned int SystemBitmap::Stride::get(void)
{
    return this->mStride;
}