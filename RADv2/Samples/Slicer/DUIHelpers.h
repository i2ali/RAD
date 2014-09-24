#pragma once

using namespace DirectUI;

HRESULT DUI_SetElementBitmap(_Inout_ Element *pe, HBITMAP hBitmap, BYTE dBlendMode, UINT dBlendValue, bool bPreMultiplied);

HRESULT Convert32bppIWICBitmapSourceToHBITMAP(_In_ IWICBitmapSource *pBitmapSource, _COM_Outptr_ HBITMAP *phbmp);

HRESULT ConvertIWICBitmapSourcePixelFormat(_In_  IWICBitmapSource *pBitmapSource,
                                           _In_  WICPixelFormatGUID guidPixelFormatDestination,
                                           _In_  IWICImagingFactory *pImagingFactory,
                                           _COM_Outptr_ IWICBitmapSource **ppBitmapSourceConverted);


HRESULT ConvertIWICBitmapSourceTo32bppHBITMAP(_In_  IWICBitmapSource *pBitmapSource,
                                              _In_  IWICImagingFactory *pImagingFactory,
                                              _COM_Outptr_ HBITMAP *phbmp);
