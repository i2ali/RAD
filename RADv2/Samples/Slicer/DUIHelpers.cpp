#include "precomp.h"
#include "DUIHelpers.h"

using namespace DirectUI;

#define BITS_PER_BYTE 8

HRESULT DUI_SetElementBitmap(_Inout_ Element *pe, HBITMAP hBitmap, BYTE dBlendMode, UINT dBlendValue, bool bPreMultiplied)
{
    HRESULT hr;

    if (hBitmap)
    {
        Value *pv = Value::CreateGraphic(hBitmap, dBlendMode, dBlendValue, false, false, bPreMultiplied);
        if (pv)
        {
            hr = pe->SetValue(Element::ContentProp, PI_Local, pv);
            pv->Release();
        }
        else
        {
            ::DeleteObject(hBitmap);
            hr = E_OUTOFMEMORY;
        }
    }
    else
    {
        // treat nullptr bitmap like we want to remove it.
        hr = pe->RemoveLocalValue(Element::ContentProp);
    }

    return hr;
}

HRESULT Convert32bppIWICBitmapSourceToHBITMAP(_In_ IWICBitmapSource *pBitmapSource, _COM_Outptr_ HBITMAP *phbmp)
{
    *phbmp = nullptr;

    UINT nWidth, nHeight;
    HRESULT hr = pBitmapSource->GetSize(&nWidth, &nHeight);
    if (SUCCEEDED(hr))
    {
        // This is set to 4 since the source is 32 bpp
        static const WORD cbPixel = 4;

        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = nWidth;
        bmi.bmiHeader.biHeight = -static_cast<long>(nHeight);
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = BITS_PER_BYTE * cbPixel;
        bmi.bmiHeader.biCompression = BI_RGB;

        BYTE* pBits = NULL;
        HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void **>(&pBits), NULL, 0);

        if (hbmp != NULL)
        {
            WICRect rect = {0, 0, nWidth, nHeight};
            // If cbPixel is changed to something other than a multiple of 4, then the stride will
            // need to be calculated differently
            C_ASSERT(cbPixel % 4 == 0);

            // Convert the pixels and store them in the HBITMAP.  Note: the name of the function is a little
            // misleading - we're not doing any extraneous copying here.  CopyPixels is actually converting the
            // image into the given buffer.
            BEGIN_REQUIRES_WIN8MQ_CODE_FIX(26007, "bug:729233")
            hr = pBitmapSource->CopyPixels(&rect, (nWidth * cbPixel), (nWidth * nHeight * cbPixel), pBits);
            END_REQUIRES_WIN8MQ_CODE_FIX

            if (SUCCEEDED(hr))
            {
                *phbmp = hbmp;
            }
            else
            {
                DeleteObject(hbmp);
            }
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

// Converts a BitmapSource to the desired pixel format
HRESULT ConvertIWICBitmapSourcePixelFormat(_In_  IWICBitmapSource *pBitmapSource,
                                           _In_  WICPixelFormatGUID guidPixelFormatDestination,
                                           _In_  IWICImagingFactory *pImagingFactory,
                                           _COM_Outptr_ IWICBitmapSource **ppBitmapSourceConverted)
{
    *ppBitmapSourceConverted = nullptr;
    
    WICPixelFormatGUID guidPixelFormatSource;
    HRESULT hr = pBitmapSource->GetPixelFormat(&guidPixelFormatSource);

    // Check if conversion is necessary
    if (guidPixelFormatSource != guidPixelFormatDestination)
    {
        CComPtr<IWICFormatConverter> spFormatConverter;
        hr = pImagingFactory->CreateFormatConverter(&spFormatConverter);

        if (SUCCEEDED(hr))
        {
            // Create the appropriate pixel format converter
            hr = spFormatConverter->Initialize(pBitmapSource, guidPixelFormatDestination, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);

            if (SUCCEEDED(hr))
            {
                *ppBitmapSourceConverted = spFormatConverter.Detach();
            }
        }
    }
    else
    {
        // No conversion necessary.  Just copy the old object over to the out param.
        *ppBitmapSourceConverted = pBitmapSource;
        (*ppBitmapSourceConverted)->AddRef();
    }

    return hr;
}

HRESULT ConvertIWICBitmapSourceTo32bppHBITMAP(_In_  IWICBitmapSource *pBitmapSource,
                                              _In_  IWICImagingFactory *pImagingFactory,
                                              _COM_Outptr_ HBITMAP *phbmp)
{
    *phbmp = nullptr;
    
    CComPtr<IWICBitmapSource> spBitmapSourceConverted;
    HRESULT hr = ConvertIWICBitmapSourcePixelFormat(pBitmapSource, GUID_WICPixelFormat32bppBGRA, pImagingFactory, &spBitmapSourceConverted);
    if (SUCCEEDED(hr))
    {
        hr = Convert32bppIWICBitmapSourceToHBITMAP(spBitmapSourceConverted, phbmp);
    }
    return hr;
}
