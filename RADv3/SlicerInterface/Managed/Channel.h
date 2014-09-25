#pragma once

#include "Stdafx.h"

namespace RAD { namespace Managed
{
    [Serializable]
    [Flags]
    public enum class Channel : unsigned int
    {
        Left =         0x1,
        Right =        0x2,
        LeftAndRight = 0x3,
    };
} }