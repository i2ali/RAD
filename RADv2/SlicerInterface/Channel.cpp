//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "Channel.h"

bool RADv2::IsChannelLeft(Channel argChannel)
{
    return (Channel::Left == static_cast<Channel>((static_cast<unsigned int>(Channel::Left) & static_cast<unsigned int>(argChannel))));
}

bool RADv2::IsChannelRight(Channel argChannel)
{
    return (Channel::Right == static_cast<Channel>((static_cast<unsigned int>(Channel::Right) & static_cast<unsigned int>(argChannel))));
}

bool RADv2::IsChannelValid(Channel argChannel)
{
    return (3 >= static_cast<UINT>(argChannel) && static_cast<UINT>(argChannel) > 0);
}