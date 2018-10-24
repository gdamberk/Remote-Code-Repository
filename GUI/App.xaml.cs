///////////////////////////////////////////////////////////////////////
// App.xaml.cs - Set  application parameters                        //
// ver 1.1                                                          //
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
 * Set  application parameters  
 *
 * Public Interface
 * ================
 * Application_Startup(object sender, StartupEventArgs e) - Create instances of 2 client.
 * 
 * Required Files:
 * ---------------
 * App.xaml.cs , App.xaml
 *
 * Maintenance History:
 * --------------------
 * ver 1.1 : 12 Apr 2018
 */
using System.Windows;

namespace WpfApp1
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : Application
  {
    private void Application_Startup(object sender, StartupEventArgs e)
    {
      MainWindow client1 = new MainWindow("8082");
      client1.Show();
      MainWindow client2 = new MainWindow("8084");
      client2.Show();
    }
  }
}
