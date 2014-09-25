//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corp.  All rights reserved.
//
//  This tests the managed RAD library by using Presentation Framework classes.
//  This test binary will NOT compile on ARM.
//
//------------------------------------------------------------------------------

// .NET Includes
using System;
using System.IO;
using System.Threading;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

// Sparta
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

// Interop Library
using RAD.Managed;


namespace RADv3.UnitTests.Managed
{
    [TestClass]
    class Basic
    {
        /// <summary>
        /// </summary>
        [TestMethod]
        public void CreateSlicerFactory()
        {
            // This function saves a bitmap source
            Action<String, BitmapSource> saveBitmapSource = (String argFileName, BitmapSource argBitmapSource) =>
            {
                FileStream stream = new FileStream(argFileName, FileMode.Create);
                BmpBitmapEncoder encoder = new BmpBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(argBitmapSource));
                encoder.Save(stream);
                stream.Flush();
                stream.Close();
            };

            // This function creates a bitmap source from a system bitmap
            Func<SystemBitmap, BitmapSource> createBitmapSource = (SystemBitmap argSystemBitmap) =>
            {
                // Convert from RGBA to BGRA
                uint currentScanLine = 0;

                for (uint y = 0; y < argSystemBitmap.Height; ++y)
                {
                    uint currentPixel = currentScanLine;

                    for (uint x = 0; x < argSystemBitmap.Width; ++x)
                    {
                        byte tempRed = argSystemBitmap.Data[currentPixel + 0];
                        argSystemBitmap.Data[currentPixel + 0] = argSystemBitmap.Data[currentPixel + 2];
                        argSystemBitmap.Data[currentPixel + 2] = tempRed;

                        currentPixel += 4;
                    }

                    currentScanLine += argSystemBitmap.Stride;
                }


                return BitmapSource.Create(
                    (int)argSystemBitmap.Width,
                    (int)argSystemBitmap.Height,
                    96,
                    96,
                    PixelFormats.Bgra32,
                    null,
                    argSystemBitmap.Data,
                    (int)argSystemBitmap.Stride);
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

                saveBitmapSource("gdiSlicerTestManaged.1.0.bmp", createBitmapSource(gdiSlicerBitmap));
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

                saveBitmapSource("gdiSlicerTestManaged.2.0.bmp", createBitmapSource(gdiSlicerBitmapLeft));
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

                saveBitmapSource("gdiSlicerTestManaged.3.0.bmp", createBitmapSource(gdiSlicerBitmapLeft));
                saveBitmapSource("gdiSlicerTestManaged.3.1.bmp", createBitmapSource(gdiSlicerBitmapRight));
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

                saveBitmapSource("dwmSlicerTestManaged.1.0.bmp", createBitmapSource(dwmSlicerBitmap));
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

                saveBitmapSource("dwmSlicerTestManaged.2.0.bmp", createBitmapSource(dwmSlicerBitmapLeft));
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

                saveBitmapSource("dwmSlicerTestManaged.3.0.bmp", createBitmapSource(dwmSlicerBitmapLeft));
                saveBitmapSource("dwmSlicerTestManaged.3.1.bmp", createBitmapSource(dwmSlicerBitmapRight));
            }
        }
    }
}
