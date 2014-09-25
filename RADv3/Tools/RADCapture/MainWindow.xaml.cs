using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using RAD.Managed;

namespace RADCapture
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }


        //
        // Window Handlers
        //

        private void Window_Initialized(object sender, EventArgs e)
        {
            // Create a SlicerFactory
            Marshal.ThrowExceptionForHR(
                SlicerFactory.Create(
                    out this.mSlicerFactory,
                    ComputeShaderModel.None));

            // Create a DWM slicer
            Marshal.ThrowExceptionForHR(
                this.mSlicerFactory.CreateDwmSlicer(
                    out this.mSlicer));

            // Display the virtual screen size
            this.mVirtualScreen_TextBlock.Text = String.Format(
                "{0}, {1}, {2}, {3}",
                SystemParameters.VirtualScreenLeft,
                SystemParameters.VirtualScreenTop,
                SystemParameters.VirtualScreenWidth,
                SystemParameters.VirtualScreenHeight);

            // Display the current selection rectangle on initialized
            //this.mCurrentSelection.Text = this.mSelectionRectangle.ToString();
            this.mCurrentSelection_TextBlock.Text = "None";
            this.mCountDown_TextBlock.Text = "-";
            this.mStatus_TextBlock.Text = "Select an area to begin...";

            // Enable the appropriate buttons
            this.mAreaSelect_Button.IsEnabled = true;
            this.mCancel_Button.IsEnabled = false;
            this.mEndCapture_Button.IsEnabled = false;
            this.mStartCapture_Button.IsEnabled = false;

            this.mTimer = new Timer(this.StartCaptureDelay, null, System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
        }


        //
        // mAreaSelectButton Handlers
        //

        private void mAreaSelectButton_Click(object sender, RoutedEventArgs e)
        {
            this.mStatus_TextBlock.Text = "Selecting Area...";

            // Create a new SelectionWindow the size of the virtual screen
            SelectionWindow selectionWindow = new SelectionWindow();

            selectionWindow.Left = SystemParameters.VirtualScreenLeft;
            selectionWindow.Top = SystemParameters.VirtualScreenTop;
            selectionWindow.Width = SystemParameters.VirtualScreenWidth;
            selectionWindow.Height = SystemParameters.VirtualScreenHeight;

            // Show the window as a dialog
            bool? result = selectionWindow.ShowDialog();

            if (null == result || false == result)
            {
                this.mStatus_TextBlock.Text = "Selection Canceled";
                this.mCurrentSelection_TextBlock.Text = "None";

                this.mStartCapture_Button.IsEnabled = false;
            }
            else
            {
                Rect currentSelection = selectionWindow.SelectionRect;

                if (currentSelection.Left >= currentSelection.Right || currentSelection.Top >= currentSelection.Bottom)
                {
                    this.mStatus_TextBlock.Text = "Bad Selection";
                    this.mCurrentSelection_TextBlock.Text = "None";

                    this.mStartCapture_Button.IsEnabled = false;
                }
                else
                {
                    this.mStatus_TextBlock.Text = "New Area Selected";
                    this.mCurrentSelection = currentSelection;
                    this.mCurrentSelection_TextBlock.Text = this.mCurrentSelection.ToString();

                    this.mStartCapture_Button.IsEnabled = true;
                }
            }

            this.mAreaSelect_Button.IsEnabled = true;
            this.mCancel_Button.IsEnabled = false;
            this.mEndCapture_Button.IsEnabled = false;
        }


        //
        // mCancel_Button
        //

        private void mCancel_Button_Click(object sender, RoutedEventArgs e)
        {
            if (true == this.mInCountdown)
            {
                this.mInCountdown = false;
                this.mTimeoutDelay = 0;

                // Kill the timer
                this.mTimer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);

                // Update the status text
                this.mStatus_TextBlock.Dispatcher.Invoke(new Action(() => { this.mStatus_TextBlock.Text = "Capture Canceled"; }));

                // Update the clock text
                this.mCountDown_TextBlock.Dispatcher.Invoke(new Action(() => { this.mCountDown_TextBlock.Text = "-"; }));

                // Enable the proper buttons
                this.mAreaSelect_Button.Dispatcher.Invoke(new Action(() => { this.mAreaSelect_Button.IsEnabled = true; }));
                this.mCancel_Button.Dispatcher.Invoke(new Action(() => { this.mCancel_Button.IsEnabled = false; }));
                this.mEndCapture_Button.Dispatcher.Invoke(new Action(() => { this.mEndCapture_Button.IsEnabled = false; }));
                this.mStartCapture_Button.Dispatcher.Invoke(new Action(() => { this.mStartCapture_Button.IsEnabled = true; }));

                this.mMaxFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mMaxFrames_TextBox.IsEnabled = true; }));
                this.mSkipFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mSkipFrames_TextBox.IsEnabled = true; }));
                this.mChannel_ComboBox.Dispatcher.Invoke(new Action(() => { this.mChannel_ComboBox.IsEnabled = true; }));
                this.mStartCaptureDelay_TextBox.Dispatcher.Invoke(new Action(() => { this.mStartCaptureDelay_TextBox.IsEnabled = true; }));

            }
        }


        //
        // mStartCaptureButton Handlers
        //

        private void mStartCaptureButton_Click(object sender, RoutedEventArgs e)
        {
            // Enable the proper buttons
            this.mAreaSelect_Button.IsEnabled = false;
            this.mCancel_Button.IsEnabled = true;
            this.mEndCapture_Button.IsEnabled = false;
            this.mStartCapture_Button.IsEnabled = false;

            // Disable the inputs
            this.mMaxFrames_TextBox.IsEnabled = false;
            this.mSkipFrames_TextBox.IsEnabled = false;
            this.mChannel_ComboBox.IsEnabled = false;
            this.mStartCaptureDelay_TextBox.IsEnabled = false;

            // Parse the curent values in the inputs
            uint maxFrames = 0;
            uint skipFrames = 0;
            uint startCaptureDelay = 0;

            UInt32.TryParse(this.mMaxFrames_TextBox.Text, out maxFrames);
            UInt32.TryParse(this.mSkipFrames_TextBox.Text, out skipFrames);
            UInt32.TryParse(this.mStartCaptureDelay_TextBox.Text, out startCaptureDelay);

            Channel channel;

            switch (this.mChannel_ComboBox.SelectedIndex)
            {
                default:
                case 0:
                    channel = Channel.Left;
                    break;

                case 1:
                    channel = Channel.Right;
                    break;

                case 2:
                    channel = Channel.LeftAndRight;
                    break;

            }

            // Save the capture paramters
            this.mMaximumNumberOfFramesToCapture = maxFrames;
            this.mFramesToSkip = skipFrames;
            this.mTimeoutDelay = startCaptureDelay;
            this.mChannel = channel;

            // Update the status message
            this.mStatus_TextBlock.Text = "Starting Capture...";

            // Indicate that we are in the countdown phase
            this.mInCountdown = true;

            // Start a timer
            this.mTimer.Change(0, 1000);
        }


        //
        // mEndCaptureButton Handlers
        //

        private void mEndCaptureButton_Click(object sender, RoutedEventArgs e)
        {
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


            // End capturing
            uint framesCaptured = 0;
            SystemBitmap leftImage = null;
            SystemBitmap rightImage = null;

            try
            {
                Marshal.ThrowExceptionForHR(
                    this.mSlicer.EndCapture(
                        ref framesCaptured,
                        out leftImage,
                        out rightImage));

                if (null != leftImage)
                {
                    // Show a save file dialog first
                    Microsoft.Win32.SaveFileDialog s = new Microsoft.Win32.SaveFileDialog();
                    s.Title = "Left Image";
                    s.Filter = "Bitmap|*.bmp";
                    s.AddExtension = true;
                    s.DefaultExt = ".bmp";

                    bool? retval = s.ShowDialog();

                    if (null != retval && true == retval)
                    {
                        FileStream stream = new FileStream(s.FileName, FileMode.Create);
                        BmpBitmapEncoder encoder = new BmpBitmapEncoder();
                        encoder.Frames.Add(BitmapFrame.Create(createBitmapSource(leftImage)));
                        encoder.Save(stream);
                        stream.Flush();
                        stream.Close();
                    }
                }

                if (null != rightImage)
                {
                    // Show a save file dialog first
                    Microsoft.Win32.SaveFileDialog s = new Microsoft.Win32.SaveFileDialog();
                    s.Title = "Right Image";
                    s.Filter = "Bitmap|*.bmp";
                    s.AddExtension = true;
                    s.DefaultExt = ".bmp";

                    bool? retval = s.ShowDialog();

                    if (null != retval && true == retval)
                    {
                        FileStream stream = new FileStream(s.FileName, FileMode.Create);
                        BmpBitmapEncoder encoder = new BmpBitmapEncoder();
                        encoder.Frames.Add(BitmapFrame.Create(createBitmapSource(rightImage)));
                        encoder.Save(stream);
                        stream.Flush();
                        stream.Close();
                    }
                }

                // Enable the proper buttons again
                this.mAreaSelect_Button.Dispatcher.Invoke(new Action(() => { this.mAreaSelect_Button.IsEnabled = true; }));
                this.mCancel_Button.Dispatcher.Invoke(new Action(() => { this.mCancel_Button.IsEnabled = false; }));
                this.mEndCapture_Button.Dispatcher.Invoke(new Action(() => { this.mEndCapture_Button.IsEnabled = false; }));
                this.mStartCapture_Button.Dispatcher.Invoke(new Action(() => { this.mStartCapture_Button.IsEnabled = true; }));

                this.mMaxFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mMaxFrames_TextBox.IsEnabled = true; }));
                this.mSkipFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mSkipFrames_TextBox.IsEnabled = true; }));
                this.mChannel_ComboBox.Dispatcher.Invoke(new Action(() => { this.mChannel_ComboBox.IsEnabled = true; }));
                this.mStartCaptureDelay_TextBox.Dispatcher.Invoke(new Action(() => { this.mStartCaptureDelay_TextBox.IsEnabled = true; }));

                this.mStatus_TextBlock.Text = "Capture Complete";
                this.mCountDown_TextBlock.Text = "-";
            }
            catch (Exception exception)
            {
                this.mStatus_TextBlock.Dispatcher.Invoke(new Action(() => { this.mStatus_TextBlock.Text = String.Format("Error 0x{0:X08}", exception.HResult); }));
                this.mCountDown_TextBlock.Dispatcher.Invoke(new Action(() => { this.mCountDown_TextBlock.Text = "-"; } ));

                // Enable the proper buttons again
                this.mAreaSelect_Button.Dispatcher.Invoke(new Action(() => { this.mAreaSelect_Button.IsEnabled = true; }));
                this.mCancel_Button.Dispatcher.Invoke(new Action(() => { this.mCancel_Button.IsEnabled = false; }));
                this.mEndCapture_Button.Dispatcher.Invoke(new Action(() => { this.mEndCapture_Button.IsEnabled = false; }));
                this.mStartCapture_Button.Dispatcher.Invoke(new Action(() => { this.mStartCapture_Button.IsEnabled = true; }));

                this.mMaxFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mMaxFrames_TextBox.IsEnabled = true; }));
                this.mSkipFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mSkipFrames_TextBox.IsEnabled = true; }));
                this.mChannel_ComboBox.Dispatcher.Invoke(new Action(() => { this.mChannel_ComboBox.IsEnabled = true; }));
                this.mStartCaptureDelay_TextBox.Dispatcher.Invoke(new Action(() => { this.mStartCaptureDelay_TextBox.IsEnabled = true; }));
            }
        }


        //
        // mSkipFrames Event Handlers
        //

        private void mSkipFrames_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            this.HandleSpacesOnPreviewKeyDown(sender, e);
        }

        private void mSkipFrames_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            this.HandleDigitsOnPreviewTextInput(sender, e);
        }

        private void mSkipFrames_TextChanged(object sender, TextChangedEventArgs e)
        {
            this.HandleInvalidCharactersOnTextChanged(sender, e);
        }


        //
        // mMaxFrames Event Handlers
        //

        private void mMaxFrames_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            this.HandleSpacesOnPreviewKeyDown(sender, e);
        }

        private void mMaxFrames_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            this.HandleDigitsOnPreviewTextInput(sender, e);
        }

        private void mMaxFrames_TextChanged(object sender, TextChangedEventArgs e)
        {
            this.HandleInvalidCharactersOnTextChanged(sender, e);
        }


        //
        // mStartCaptureDelay Event Handlers
        //

        private void mStartCaptureDelay_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            this.HandleSpacesOnPreviewKeyDown(sender, e);
        }

        private void mStartCaptureDelay_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            this.HandleDigitsOnPreviewTextInput(sender, e);
        }

        private void mStartCaptureDelay_TextChanged(object sender, TextChangedEventArgs e)
        {
            this.HandleInvalidCharactersOnTextChanged(sender, e);
        }


        //
        // Helper Methods
        //

        private void HandleSpacesOnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }

        private void HandleDigitsOnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            bool containsNonDigit = true;

            for (int i = 0; i < e.Text.Length; ++i)
            {
                if (false == Char.IsDigit(e.Text[i]))
                {
                    containsNonDigit = false;
                    break;
                }
            }

            e.Handled = !containsNonDigit;
        }

        private void HandleInvalidCharactersOnTextChanged(object sender, TextChangedEventArgs e)
        {
            if (!(sender is TextBox))
            {
                return;
            }

            TextBox target = (TextBox)sender;

            StringBuilder sb = new StringBuilder(target.Text);

            int currentSelectionStart = target.SelectionStart;

            int currentIndex = 0;
            bool modified = false;

            while (currentIndex < sb.Length)
            {
                Char currentChar = sb[currentIndex];

                if (Char.IsWhiteSpace(currentChar) || !Char.IsDigit(currentChar))
                {
                    sb.Remove(currentIndex, 1);
                    modified = true;

                    if (currentIndex < currentSelectionStart)
                    {
                        currentSelectionStart--;
                    }
                }
                else
                {
                    currentIndex++;
                }
            }

            if (true == modified)
            {
                target.Text = sb.ToString();
                target.SelectionStart = currentSelectionStart;
            }
        }

        //
        // Timer callback
        //

        private void StartCaptureDelay(Object stateInfo)
        {
            if (this.mTimeoutDelay > 0)
            {
                // Synchronously set the timeout clock on the UI thread
                this.mCountDown_TextBlock.Dispatcher.Invoke(new Action(() => { this.mCountDown_TextBlock.Text = this.mTimeoutDelay.ToString(); } ));

                // Tick down the counter
                this.mTimeoutDelay--;
            }
            else
            {
                // Enable the proper buttons
                this.mCancel_Button.Dispatcher.Invoke(new Action(() => { this.mCancel_Button.IsEnabled = false; }));
                this.mEndCapture_Button.Dispatcher.Invoke(new Action(() => { this.mEndCapture_Button.IsEnabled = true; }));

                // Not in countdown anymore
                this.mInCountdown = false;

                // Stop the countdown timer from signaling
                this.mTimer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);

                // Synchronously set the timout delay clock on the UI thread
                this.mCountDown_TextBlock.Dispatcher.Invoke(new Action(() => { this.mCountDown_TextBlock.Text = "0"; } ));

                // Synchronously update the status bar
                this.mStatus_TextBlock.Dispatcher.Invoke(new Action(() => { this.mStatus_TextBlock.Text = "Capturing..."; } ));

                // Invoke the capture on the window thread
                this.Dispatcher.Invoke(new Action(() =>
                {
                    try
                    {
                        // Start capturing
                        Marshal.ThrowExceptionForHR(
                            this.mSlicer.StartCapture(
                                (int)this.mCurrentSelection.Left,
                                (int)this.mCurrentSelection.Top,
                                (int)this.mCurrentSelection.Right,
                                (int)this.mCurrentSelection.Bottom,
                                this.mMaximumNumberOfFramesToCapture,
                                this.mFramesToSkip,
                                this.mChannel));
                    }
                    catch (Exception exception)
                    {
                        // Display an error in the status bar
                        this.mStatus_TextBlock.Dispatcher.Invoke(new Action(() => { this.mStatus_TextBlock.Text = String.Format("Error 0x{0:X08}", exception.HResult); }));
                        this.mCountDown_TextBlock.Dispatcher.Invoke(new Action(() => { this.mCountDown_TextBlock.Text = "-"; } ));

                        // Enable the proper buttons again
                        this.mAreaSelect_Button.Dispatcher.Invoke(new Action(() => { this.mAreaSelect_Button.IsEnabled = true; }));
                        this.mCancel_Button.Dispatcher.Invoke(new Action(() => { this.mCancel_Button.IsEnabled = false; }));
                        this.mEndCapture_Button.Dispatcher.Invoke(new Action(() => { this.mEndCapture_Button.IsEnabled = false; }));
                        this.mStartCapture_Button.Dispatcher.Invoke(new Action(() => { this.mStartCapture_Button.IsEnabled = true; }));

                        this.mMaxFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mMaxFrames_TextBox.IsEnabled = true; }));
                        this.mSkipFrames_TextBox.Dispatcher.Invoke(new Action(() => { this.mSkipFrames_TextBox.IsEnabled = true; }));
                        this.mChannel_ComboBox.Dispatcher.Invoke(new Action(() => { this.mChannel_ComboBox.IsEnabled = true; }));
                        this.mStartCaptureDelay_TextBox.Dispatcher.Invoke(new Action(() => { this.mStartCaptureDelay_TextBox.IsEnabled = true; }));
                    }

                }));
            }
        }


        //
        // Private Data Members
        //

        private Rect mCurrentSelection;
        private SlicerFactory mSlicerFactory;
        private IDwmSlicer mSlicer;
        private Timer mTimer;
        private uint mTimeoutDelay;
        private uint mMaximumNumberOfFramesToCapture;
        private uint mFramesToSkip;
        private Channel mChannel;
        private bool mInCountdown;

    }
}
