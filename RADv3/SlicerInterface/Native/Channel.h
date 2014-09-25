//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      An enumeration representing the stereo visual channels.
//
//------------------------------------------------------------------------------

#pragma once

namespace RAD
{
    enum class Channel : unsigned int
    {
        Left =         0x1,
        Right =        0x2,
        LeftAndRight = 0x3,
    };

    bool IsChannelLeft(Channel argChannel);

    bool IsChannelRight(Channel argChannel);

    bool IsChannelValid(Channel argChannel);
}
