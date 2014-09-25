//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"

void RAD::release_iunknown_deleter(IUnknown* unknown)
{
    if (nullptr != unknown)
    {
        #ifdef DBG
        std::wcout << std::endl << L"Releasing: 0x" << unknown << std::endl;
        #endif
        unknown->Release();
    }
}
