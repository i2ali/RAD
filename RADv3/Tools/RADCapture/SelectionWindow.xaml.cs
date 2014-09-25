using System.Windows;
using System.Windows.Input;

namespace RADCapture
{
    /// <summary>
    /// Interaction logic for SelectionWindow.xaml
    /// </summary>
    public partial class SelectionWindow : Window
    {
        public SelectionWindow()
        {
            InitializeComponent();
        }


        //
        // Events
        //

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                this.mSelecting = false;
                this.DialogResult = false;
                this.Close();
            }
        }


        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (false == this.mSelecting)
            {
                // Get the mouse down position
                Point mouseDownPosition = e.GetPosition(this);

                // Position the selection rectangle
                this.mLine1.X1 = mouseDownPosition.X;
                this.mLine1.Y1 = mouseDownPosition.Y;
                this.mLine1.X2 = mouseDownPosition.X;
                this.mLine1.Y2 = mouseDownPosition.Y;

                this.mLine2.X1 = mouseDownPosition.X;
                this.mLine2.Y1 = mouseDownPosition.Y;
                this.mLine2.X2 = mouseDownPosition.X;
                this.mLine2.Y2 = mouseDownPosition.Y;

                this.mLine3.X1 = mouseDownPosition.X;
                this.mLine3.Y1 = mouseDownPosition.Y;
                this.mLine3.X2 = mouseDownPosition.X;
                this.mLine3.Y2 = mouseDownPosition.Y;

                this.mLine4.X1 = mouseDownPosition.X;
                this.mLine4.Y1 = mouseDownPosition.Y;
                this.mLine4.X2 = mouseDownPosition.X;
                this.mLine4.Y2 = mouseDownPosition.Y;

                // Make the selection rectangle visible
                this.mLine1.Visibility = System.Windows.Visibility.Visible;
                this.mLine2.Visibility = System.Windows.Visibility.Visible;
                this.mLine3.Visibility = System.Windows.Visibility.Visible;
                this.mLine4.Visibility = System.Windows.Visibility.Visible;

                // Save the mouse down position
                this.mMouseDownPosition = mouseDownPosition;

                // Set a bool indicating that we are in selection
                this.mSelecting = true;
            }
        }


        private void Window_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (true == this.mSelecting)
            {
                // Get the mouse up position
                Point mouseUpPosition = e.GetPosition(this);

                // Determine the selection rectangle
                double x, y, width, height;

                if (this.mMouseDownPosition.X < mouseUpPosition.X)
                {
                    x = this.mMouseDownPosition.X;
                    width = mouseUpPosition.X - this.mMouseDownPosition.X;
                }
                else
                {
                    x = mouseUpPosition.X;
                    width = this.mMouseDownPosition.X - mouseUpPosition.X;
                }

                if (this.mMouseDownPosition.Y < mouseUpPosition.Y)
                {
                    y = this.mMouseDownPosition.Y;
                    height = mouseUpPosition.Y - this.mMouseDownPosition.Y;
                }
                else
                {
                    y = mouseUpPosition.Y;
                    height = this.mMouseDownPosition.Y - mouseUpPosition.Y;
                }

                // Save the selection rectangle
                this.mSelectionRect = new Rect(x, y, width, height);

                // We made a selection
                this.DialogResult = true;

                // Close the selection window
                this.Close();

                this.mSelecting = false;
            }
        }


        private void Window_MouseMove(object sender, MouseEventArgs e)
        {
            if (true == this.mSelecting)
            {
                Point mousePosition = e.GetPosition(this);

                // Reposition the selection rectangle
                this.mLine1.X2 = mousePosition.X;
                this.mLine2.Y2 = mousePosition.Y;

                this.mLine3.Y1 = mousePosition.Y;
                this.mLine3.X2 = mousePosition.X;
                this.mLine3.Y2 = mousePosition.Y;

                this.mLine4.X1 = mousePosition.X;
                this.mLine4.X2 = mousePosition.X;
                this.mLine4.Y2 = mousePosition.Y;
            }
        }


        //
        // Public Properties
        //

        public Rect SelectionRect
        {
            get
            {
                return this.mSelectionRect;
            }
        }


        //
        // Private Data Members
        //

        private Point mMouseDownPosition;
        private Rect  mSelectionRect;
        private bool  mSelecting = false;
    }
}
