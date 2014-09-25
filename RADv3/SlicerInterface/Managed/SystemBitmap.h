//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

namespace RAD { namespace Managed
{
    public ref class SystemBitmap
    {
    // Constructor
    internal:
        SystemBitmap(
            array<System::Byte>^ argData,
            unsigned int argWidth,
            unsigned int argHeight,
            unsigned int argStride);


    // Public Properties
    public:
        property array<System::Byte>^ Data
        {
            array<System::Byte>^ get(void);
        }

        property unsigned int Width
        {
            unsigned int get(void);
        }

        property unsigned int Height
        {
            unsigned int get(void);
        }

        property unsigned int Stride
        {
            unsigned int get(void);
        }


    // Private Data Members
    private:
        array<System::Byte>^ mData;
        unsigned int         mWidth;
        unsigned int         mHeight;
        unsigned int         mStride;

    };
} }