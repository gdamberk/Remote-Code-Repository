///////////////////////////////////////////////////////////////////////
// Window1.xaml.cs - GUI for showing file content                //
// ver 1.0                                                          //
// Language:    C#, Visual Studio 2017                              //
// Platform:     Windows 7                                          //
// Application: Remote Repository Prototypes                        //
//                                                                  //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett                                          //
// CSE687 - Object Oriented Design, Spring 2018                     //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for showing file content.
 * Provide functionality for exit button
 *
 * Public Interface
 * ================
 * exitButton_Click(object sender, RoutedEventArgs e) - Exit button click
 * 
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs , Window1.xaml
 *
 * Maintenance History:
 * --------------------
 * ver 1.0 : 12 Apr 2018
 * - first release

 */

using System.Windows;

namespace WpfApp1
{/// <summary>
 /// Interaction logic for Window1.xaml
 /// </summary>
    public partial class Window1 : Window
    {
        private static double leftOffset = 500.0;
        private static double topOffset = -20.0;

        //Constructor
        public Window1()
        {
            InitializeComponent();
            double Left = Application.Current.MainWindow.Left;
            double Top = Application.Current.MainWindow.Top;
            this.Left = Left + leftOffset;
            this.Top = Top + topOffset;
            this.Width = 600.0;
            this.Height = 800.0;
            leftOffset += 20.0;
            topOffset += 20.0;
            if (leftOffset > 700.0)
                leftOffset = 500.0;
            if (topOffset > 180.0)
                topOffset = -20.0;
        }


        //Close the pop up 
        private void exitButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
