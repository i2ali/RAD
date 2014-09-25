//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corp.  All rights reserved.
//
//  This tests the managed RAD library by using legacy System.Drawing classes.
//  This test binary will compile on ARM.
//
//------------------------------------------------------------------------------

// .NET Includes
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

// Sparta
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

// Interop Library
using RAD.Managed;


namespace RADv3.UnitTests.Managed.Legacy
{
    [TestClass]
    class Basic
    {
        /// <summary>
        /// </summary>
        [TestMethod]
        public void CreateSlicerFactory()
        {
            // This function creates a bitmap from a system bitmap
            Func<SystemBitmap, Bitmap> createBitmap = (SystemBitmap argSystemBitmap) =>
            {
                Bitmap bitmap = new Bitmap((int)argSystemBitmap.Width, (int)argSystemBitmap.Height);

                // Convert from RGBA to BGRA
                uint currentScanLine = 0;

                for (uint y = 0; y < argSystemBitmap.Height; ++y)
                {
                    uint currentPixel = currentScanLine;

                    for (uint x = 0; x < argSystemBitmap.Width; ++x)
                    {
                        bitmap.SetPixel(
                            (int)x,
                            (int)y,
                            Color.FromArgb(
                                argSystemBitmap.Data[currentPixel + 3],
                                argSystemBitmap.Data[currentPixel + 0],
                                argSystemBitmap.Data[currentPixel + 1],
                                argSystemBitmap.Data[currentPixel + 2]));

                        currentPixel += 4;
                    }

                    currentScanLine += argSystemBitmap.Stride;
                }

                return bitmap;
            };


            SlicerFactory slicerFactory = null;
            IGdiSlicer gdiSlicer = null;
            IDwmSlicer dwmSlicer = null;

            // Test creating a slicer factory
            int retval = SlicerFactory.Create(out slicerFactory, ComputeShaderModel.None);
            Verify.AreEqual(0, retval, String.Format("SlicerFactory.Create: {0:X08}", retval));
            Verify.IsNotNull(slicerFactory);

            // Put the SlicerFactory object in a "using" block so that we test
            // what happens when the SlicerFactory's resources are freed.
            using (slicerFactory)
            {
                // Test creating a GDI slicer
                retval = slicerFactory.CreateGdiSlicer(out gdiSlicer);

                Verify.AreEqual(0, retval, String.Format("CreateGdiSlicer: {0:X08}", retval));
                Verify.IsNotNull(gdiSlicer);

                // Test creating a DWM slicer
                retval = slicerFactory.CreateDwmSlicer(out dwmSlicer);

                Verify.AreEqual(0, retval, String.Format("CreateDwmSlicer: {0:X08}", retval));
                Verify.IsNotNull(dwmSlicer);
            }


            // Test EndCapture without StartCapture
            {
                SystemBitmap gdiSlicerBitmap = null;
                uint framesCaptured = 0;

                retval = gdiSlicer.EndCapture(ref framesCaptured, out gdiSlicerBitmap);
                Verify.AreNotEqual(0, retval, String.Format("gdiSlicer.EndCapture: {0:X08}", retval));
            }

            // Test using DWM and GDI slicer after the slcier factory has been
            // destroyed
            {
                retval = gdiSlicer.StartCapture(0, 0, 100, 100, 10, 0);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap gdiSlicerBitmap = null;
                uint framesCaptured = 0;

                retval = gdiSlicer.EndCapture(ref framesCaptured, out gdiSlicerBitmap);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(gdiSlicerBitmap);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(gdiSlicerBitmap).Save("gdiSlicerTestManaged.1.0.bmp", ImageFormat.Bmp);
            }

            {
                retval = gdiSlicer.StartCapture(0, 0, 100, 100, 10, 0);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap gdiSlicerBitmapLeft = null;
                SystemBitmap gdiSlicerBitmapRight = null;
                uint framesCaptured = 0;

                retval = gdiSlicer.EndCapture(ref framesCaptured, out gdiSlicerBitmapLeft, out gdiSlicerBitmapRight);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(gdiSlicerBitmapLeft);
                Verify.IsNull(gdiSlicerBitmapRight);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(gdiSlicerBitmapLeft).Save("gdiSlicerTestManaged.2.0.bmp", ImageFormat.Bmp);
            }

            {
                retval = gdiSlicer.StartCapture(0, 0, 100, 100, 10, 0, Channel.LeftAndRight);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap gdiSlicerBitmapLeft = null;
                SystemBitmap gdiSlicerBitmapRight = null;
                uint framesCaptured = 0;

                retval = gdiSlicer.EndCapture(ref framesCaptured, out gdiSlicerBitmapLeft, out gdiSlicerBitmapRight);
                Verify.AreEqual(0, retval, String.Format("gdiSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(gdiSlicerBitmapLeft);
                Verify.IsNotNull(gdiSlicerBitmapRight);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(gdiSlicerBitmapLeft).Save("gdiSlicerTestManaged.3.0.bmp", ImageFormat.Bmp);
                createBitmap(gdiSlicerBitmapRight).Save("gdiSlicerTestManaged.3.1.bmp", ImageFormat.Bmp);
            }


            // Test EndCapture without StartCapture
            {
                SystemBitmap dwmSlicerBitmap = null;
                uint framesCaptured = 0;

                retval = dwmSlicer.EndCapture(ref framesCaptured, out dwmSlicerBitmap);
                Verify.AreNotEqual(0, retval, String.Format("dwmSlicer.EndCapture: {0:X08}", retval));
            }

            // Test using DWM and DWM slicer after the slcier factory has been
            // destroyed
            {
                retval = dwmSlicer.StartCapture(0, 0, 100, 100, 10, 0);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap dwmSlicerBitmap = null;
                uint framesCaptured = 0;
                retval = dwmSlicer.EndCapture(ref framesCaptured, out dwmSlicerBitmap);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(dwmSlicerBitmap);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(dwmSlicerBitmap).Save("dwmSlicerTestManaged.1.0.bmp", ImageFormat.Bmp);
            }

            {
                retval = dwmSlicer.StartCapture(0, 0, 100, 100, 10, 0);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap dwmSlicerBitmapLeft = null;
                SystemBitmap dwmSlicerBitmapRight = null;
                uint framesCaptured = 0;

                retval = dwmSlicer.EndCapture(ref framesCaptured, out dwmSlicerBitmapLeft, out dwmSlicerBitmapRight);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(dwmSlicerBitmapLeft);
                Verify.IsNull(dwmSlicerBitmapRight);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(dwmSlicerBitmapLeft).Save("dwmSlicerTestManaged.2.0.bmp", ImageFormat.Bmp);
            }

            // Note: Windows 8 Bugs #11553 and Windows Blue Bugs #22133
            // Capture left and right channels
            {
                retval = dwmSlicer.StartCapture(0, 0, 100, 100, 10, 0, Channel.LeftAndRight);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.StartCapture: {0:X08}", retval));

                Thread.Sleep(3000);

                SystemBitmap dwmSlicerBitmapLeft = null;
                SystemBitmap dwmSlicerBitmapRight = null;
                uint framesCaptured = 0;

                retval = dwmSlicer.EndCapture(ref framesCaptured, out dwmSlicerBitmapLeft, out dwmSlicerBitmapRight);
                Verify.AreEqual(0, retval, String.Format("dwmSlicer.EndCapture: {0:X08}", retval));
                Verify.IsNotNull(dwmSlicerBitmapLeft);
                Verify.IsNotNull(dwmSlicerBitmapRight);

                Log.Comment(String.Format("Frames captured: {0}", framesCaptured));

                createBitmap(dwmSlicerBitmapLeft).Save("dwmSlicerTestManaged.3.0.bmp", ImageFormat.Bmp);
                createBitmap(dwmSlicerBitmapRight).Save("dwmSlicerTestManaged.3.1.bmp", ImageFormat.Bmp);
            }
        }
    }
}
