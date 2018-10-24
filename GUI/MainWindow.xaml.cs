///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Remote Repository                      //
// ver 1.2                                                          //
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
 * This package provides a WPF-based GUI for Remote Repository demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 * - You can checkIn, checkout files, view file content and metadata.
 *
 * Public Interface
 * ================
processMessages() - process incoming messages on child thread
clearDirs(string type) - function dispatched by child thread to main thread for clear directory
addDir(string dir, string type) - function dispatched by child thread to main thread for Add directory
insertParent(string type) - function dispatched by child thread to main thread for insert parent
clearFiles(string type) - function dispatched by child thread to main thread for clear files
addFile(string file, string type) - function dispatched by child thread to main thread for add files
setData(string contents) - function dispatched by child thread to main thread for set metadata
addClientProc(string key, Action<CsMessage> clientProc) - add client processing for message with key
DispatcherLoadGetDirs() - load getDirs processing into dispatcher dictionary
DispatcherLoadGetFiles() - load getFiles processing into dispatcher dictionary
popUp(string fileName) - create popUp for given fileName
CheckIn(string filePath, string fileName) - Send check In message for given fileName
CheckOut(string filePath, string fileName) - Send check Out message for given fileName
viewMetadataFun(string filePath, string fileName) - Send view metadata message for given fileName
connectToServer() - Send connect message for given fileName
connectToServerTest() - Send connect message for given fileName
viewFileContent(string filePath, string fileName) - Send view File Content message for given fileName
TestFun() -  Perform unit test cases
enableWindowTest() - Enable window for test 
enableWindow() - Enable window
fetchFiles() - fetch files
Load window and set comm - Window_Loaded(object sender, RoutedEventArgs e)
removeFirstDir(string path) - strip off name of first part of path 
DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e) - respond to mouse double-click on dir name
DirList2_MouseDoubleClick(object sender, MouseButtonEventArgs e) - respond to mouse double-click on dir name 
DirList3_MouseDoubleClick(object sender, MouseButtonEventArgs e) - respond to mouse double-click on dir name
CheckoutFiles_Click(object sender, RoutedEventArgs e) - checkout file click function
chkInButton_Click(object sender, RoutedEventArgs e) - checkIn file click function
filesListBox3_MouseDoubleClick(object sender, MouseButtonEventArgs e) - filebox3 double click function
showFile(string fileName, Window1 popUp) - Show file in window pop up
Refresh1Button_Click(object sender, RoutedEventArgs e) - Refresh button double click
Refresh2Button_Click(object sender, RoutedEventArgs e) - Refresh button double click
ViewMetadata_Click(object sender, RoutedEventArgs e) - view metadata button click
ViewFile_Click(object sender, RoutedEventArgs e) - view metadata button click
ConnectButton_Click(object sender, RoutedEventArgs e) - Connect button click 
 * 
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 * ver 1.1 : 12 Apr 2018
 * - Updated the GUI for checkin , check out , Browse tabs
 * ver 1.2 : 30 May 2018
 * - Updated GUI for checkin, Query tab
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Input;
using System.Threading;
using MsgPassingCommunication;

namespace WpfApp1
{
  public partial class MainWindow : Window
  {
    //Initialize component
    public MainWindow()
    {
      InitializeComponent();
      Console.Title = "Client Console";
    }
    
    //Main WIndow Constructor
    public MainWindow(string port1)
    {
      InitializeComponent();
      Console.Title = "Client Console";
      port = port1;
    }

    List<Window1> popups = new List<Window1>();
    private string port = "8082";
    private Stack<string> pathStack_ = new Stack<string>();
    private Stack<string> pathClient_ = new Stack<string>();
    private Stack<string> pathBrowse_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private CsEndPoint serverEndPoint;
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ 
      = new Dictionary<string, Action<CsMessage>>();
    private string storagePath = "../ServerStorage";
    private string clientPath = "../ClientStorage";
    private List<string> catList = new List<string>();
    private List<string> depList = new List<string>();


    //----< process incoming messages on child thread >----------------

    private void processMessages()
    {
      ThreadStart thrdProc = () => {
        while (true)
        {
          CsMessage msg = translater.getMessage();
              if (msg.attributes.Count ==0)
                  continue;
              msg.show();
          string msgId = msg.value("command");
          if (dispatcher_.ContainsKey(msgId))
          {
            dispatcher_[msgId].Invoke(msg);
          }
        }
      };
      rcvThrd = new Thread(thrdProc);
      rcvThrd.IsBackground = true;
      rcvThrd.Start();
    }
    //----< function dispatched by child thread to main thread  for clear directory>-------

    private void clearDirs(string type)
    {
      if (type.Contains("CheckIn"))
      {
         DirList2.Items.Clear();
      }
      else if(type.Contains("CheckOut")){
         DirList.Items.Clear();
      }
      else if(type.Contains("Browse")){
         DirList3.Items.Clear();
      }
    }
    //----< function dispatched by child thread to main thread for Add directory>-------

    private void addDir(string dir, string type)
    {
      if (type.Contains("CheckIn"))
      {
         DirList2.Items.Add(dir);
      }
      else if(type.Contains("CheckOut")){
         DirList.Items.Add(dir);
      }
      else if(type.Contains("Browse")){
         DirList3.Items.Add(dir);
      }
    }
    //----< function dispatched by child thread to main thread >-------

    private void insertParent(string type)
    {
      if (type.Contains("CheckIn"))
      {
        DirList2.Items.Insert(0, "..");
      }
      else if(type.Contains("CheckOut")){
        DirList.Items.Insert(0, "..");
      }
      else if(type.Contains("Browse")){
        DirList3.Items.Insert(0, "..");
      }
    }
    //----< function dispatched by child thread to main thread for clear files >-------

    private void clearFiles(string type)
    {
      if (type.Contains("CheckIn"))
      {
        FileList2.Items.Clear();
      }
      else if(type.Contains("CheckOut")){
        FileList.Items.Clear();
      }
      else if(type.Contains("Browse")){
        FileList3.Items.Clear();
      }
      
    }
    //----< function dispatched by child thread to main thread for add files >-------

    private void addFile(string file, string type)
    {
      if (type.Contains("CheckIn"))
      {
        FileList2.Items.Add(file);
      }
      else if(type.Contains("CheckOut")){
        FileList.Items.Add(file);
      }
      else if(type.Contains("Browse")){
        FileList3.Items.Add(file);
      }
    }

     //----< function dispatched by child thread to main thread for set metadata >-------
    private void setData(string contents)
    {
            string data = "";
            string[] list = contents.Split('|');
            foreach (string content in list)
            {
                data = data + "\n" + content;
            }
            Metadata.Text = data;
            statusBarText.Text = "Status: View Metadata Completed";
    }
    
     //----< function dispatched by child thread to main thread for set search result >-------
    private void setSearchResult(string result)
    {
            searchList.Items.Clear();
            string[] list = result.Split('|');
            foreach (string content in list)
            {
                searchList.Items.Add(content);
            }
            statusBarText.Text = "Status: Search Files Completed";
    }
        
    //----< function dispatched by child thread to main thread for set files with no parent >-------
    private void setNoParentsResult(string result)
        {
            searchList.Items.Clear();
            string[] list = result.Split('|');
            foreach (string content in list)
            {
                searchList.Items.Add(content);
            }
            statusBarText.Text = "Status: Get files with No Parents Completed";
        }

        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
    }
    //----< load getDirs processing into dispatcher dictionary >-------

    private void DispatcherLoadGetDirs()
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {
        string type = rcvMsg.value("type");
        Action clrDirs = () =>
        {
          clearDirs(type);
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {
            Action<string> doDir = (string dir) =>
            {
              addDir(dir, type);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
          }
        }
        Action insertUp = () =>
        {
          insertParent(type);
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      addClientProc("getDirs", getDirs);
    }
    //----< load getFiles processing into dispatcher dictionary >------

    private void DispatcherLoadGetFiles()
    {
      Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
      {
        string type = rcvMsg.value("type");
        Action clrFiles = () =>
        {
          clearFiles(type);
        };
        Dispatcher.Invoke(clrFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("file"))
          {
            Action<string> doFile = (string file) =>
            {
              addFile(file, type);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("getFiles", getFiles);
    }

   //----< load metadata processing into dispatcher dictionary >------
  private void DispatcherMetadata()
  {
    Action<CsMessage> metadata = (CsMessage rcvMsg) =>
    {
        Action<string> setMetaData = (string content) =>
        {
            setData(content);
        };
        Dispatcher.Invoke(setMetaData, new Object[] { rcvMsg.value("Content") });
        
    };
    addClientProc("metadata", metadata);
  }

   
  //----< load search result into dispatcher dictionary >------
  private void DispatcherSearch()
  {
    Action<CsMessage> fileList = (CsMessage rcvMsg) =>
    {
        Action<string> searchList = (string result) =>
        {
            setSearchResult(result);
        };
        Dispatcher.Invoke(searchList, new Object[] { rcvMsg.value("result") });
        
    };
    addClientProc("fileList", fileList);
  }

    //----< load search result into dispatcher dictionary >------
  private void DispatcherNoParent()
  {
    Action<CsMessage> noParent = (CsMessage rcvMsg) =>
    {
        Action<string> noParentList = (string result) =>
        {
            setNoParentsResult(result);
        };
        Dispatcher.Invoke(noParentList, new Object[] { rcvMsg.value("result") });
        
    };
    addClientProc("noParent", noParent);
  }
        
  //----< load ready processing into dispatcher dictionary >------
  private void DispatcherCheckInFile()
  {
    Action<CsMessage> ready = (CsMessage rcvMsg) =>
    {
      CsMessage msg1 = new CsMessage();
      msg1.add("to", CsEndPoint.toString(serverEndPoint));
      msg1.add("from", CsEndPoint.toString(endPoint_));
      msg1.add("command", "fileTransfer");
      msg1.add("path", rcvMsg.value("path"));
      msg1.add("fileName", rcvMsg.value("fileName"));
      msg1.add("type", rcvMsg.value("type"));
      msg1.add("file", rcvMsg.value("fileName"));
      msg1.add("name", rcvMsg.value("name"));
      msg1.add("description", rcvMsg.value("description"));
      msg1.add("categories", rcvMsg.value("categories"));
      msg1.add("dependancy", rcvMsg.value("dependancy"));
      msg1.add("Comment", "File Sent");
      translater.postMessage(msg1);  
    };
    addClientProc("ready", ready);
  }

   //----< load fileTransfer processing into dispatcher dictionary >------
  private void DispatcherCheckOutFile()
  {
    Action<CsMessage> fileTransfer = (CsMessage rcvMsg) =>
    {
      CsMessage msg1 = new CsMessage();
      msg1.add("to", CsEndPoint.toString(serverEndPoint));
      msg1.add("from", CsEndPoint.toString(endPoint_));
      msg1.add("command", "Done");
      msg1.add("type", rcvMsg.value("type"));
      msg1.add("path", rcvMsg.value("path"));
      msg1.add("fileName", rcvMsg.value("fileName"));
      msg1.add("Comment", "File Received");
      translater.postMessage(msg1);

      if(rcvMsg.value("type").Equals("CheckOut"))
      {
        Action statuscheckIn = () =>
        {
           statusBarText.Text =  "Status: File CheckOut completed"; ;
        };
        Dispatcher.Invoke(statuscheckIn, new Object[] { });
      }
    
    };
    addClientProc("fileTransfer", fileTransfer);
  }
   
   //----< load Done processing into dispatcher dictionary >------
  private void DispatcherDone()
  {
    Action<CsMessage> Done = (CsMessage rcvMsg) =>
    {
        if (rcvMsg.value("type").Equals("Connect"))
        {
            Action setWindow = () =>
            {
                enableWindow();
            };
            Dispatcher.Invoke(setWindow, new Object[] { });
        }
        else if (rcvMsg.value("type").Equals("Test"))
        {
            Action setWindowTest= () =>
            {
                enableWindowTest();
            };
            Dispatcher.Invoke(setWindowTest, new Object[] { });
        }
        else if(rcvMsg.value("type").Equals("CheckIn"))
        {
            Action statuscheckIn = () =>
            {
                statusBarText.Text =  "Status: File CheckIn Completed"; ;
            };
            Dispatcher.Invoke(statuscheckIn, new Object[] { });
        }
    };
    addClientProc("Done", Done);
  }
   
   //----< load sendFile processing into dispatcher dictionary >------
  private void DispatcherSendFile()
  {
    Action<CsMessage> sendFile = (CsMessage rcvMsg) =>
    {
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
            string key = enumer.Current.Key;
            if (key.Contains("fileName"))
            {
                Action<string> setPopup = (string file) =>
                {
                    popUp(file);
                };
                Dispatcher.Invoke(setPopup, new Object[] { enumer.Current.Value });
            }
        }
          
    };
    addClientProc("sendFile", sendFile);
  }

  //----< load all dispatcher processing >---------------------------

    private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
      DispatcherCheckInFile();
      DispatcherCheckOutFile();
      DispatcherDone();
      DispatcherMetadata();
      DispatcherSendFile();
      DispatcherSearch();
      DispatcherNoParent();
    }

     //----< create popUp for given fileName>------
    private void popUp(string fileName)
    {
            Window1 codePopup = new Window1();
            codePopup.Show();
            popups.Add(codePopup);

            codePopup.codeView.Blocks.Clear();

            codePopup.codeLabel.Text = "Source code: " + fileName;
            statusBarText.Text = "Status: View File Content Completed";

            showFile(fileName, codePopup);
    }    
    //----< Send check In message for given fileName>------

    private void CheckIn(string filePath, string fileName)
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "checkIn");
      msg.add("path", filePath);
      msg.add("fileName", fileName);
      msg.add("type","CheckIn");
      msg.add("name", Author.Text);
      msg.add("description", description.Text);
      string cat = string.Join("|", catList);
      msg.add("categories", cat);
      string dependancy1 = string.Join("|", depList);
      msg.add("dependancy", dependancy1);
      translater.postMessage(msg);
      statusBarText.Text = "Sending  message : " + msg.value("command");
    }

    //----< Send check out message for given fileName>------
    private void CheckOut(string filePath, string fileName)
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "checkOut");
      msg.add("path", filePath);
      msg.add("fileName", fileName);
      msg.add("type","CheckOut");
      translater.postMessage(msg);
      msg.show();
    }

     //------<Send close checkin message for given file name
     private void closeCheckIn(string filePath, string fileName)
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "close");
      msg.add("path", filePath);
      msg.add("fileName", fileName);
      msg.add("type","close");
      translater.postMessage(msg);
    }
   
    //----< Send view metadata message for given fileName>------
    private void viewMetadataFun(string filePath, string fileName)
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "SendMetadata");
      msg.add("path", filePath);
      msg.add("fileName", fileName);
      msg.add("type","Browse");
      translater.postMessage(msg);
    }

     //----< Send connect message for given fileName>------
    private void connectToServer()
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "connectServer");
      msg.add("type", "Connect");
      translater.postMessage(msg);
    }
    
    //----< Send connect message for given fileName>------
    private void connectToServerTest()
    {
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "connectServer");
      msg.add("type", "Test");
      translater.postMessage(msg);
    }

     //----< Send view File Content message for given fileName>------
    private void viewFileContent(string filePath, string fileName)
    {
        CsMessage msg = new CsMessage();
        msg.add("to", CsEndPoint.toString(serverEndPoint));
        msg.add("from", CsEndPoint.toString(endPoint_));
        msg.add("command", "sendFile");
        msg.add("path", filePath);
        msg.add("fileName", fileName);
        translater.postMessage(msg);
    }

     //----< Perform unit test cases >------
    private void TestFun()
    {
       if (port.Equals("8082"))
       {
          Console.WriteLine("\n\n  Demonstrating requirement 3. Checkout - retrieves package files, removing version information from their filenames.Retrieved files will be copied to a specified directory.");
          Console.WriteLine("\n  Checkout file without child - Test1.h");
          Console.WriteLine("\n  ---------------------------");
          CheckOut("../ServerStorage/Test1", "Test1.h.2");
          Console.WriteLine("\n  Checkout file with child - Test2.cpp");
          Console.WriteLine("\n  ---------------------------");
          CheckOut("../ClientStorage/Test2", "Test2.cpp.1");
          testCheckin();
       }
       else{
          Console.WriteLine("\n  Demonstrating reqiuirement Browse: provides the capability to locate files and 6. view their contents and 7. Metadata.");
          Console.WriteLine("\n  View File Content for : Test2.h.1");
          Console.WriteLine("\n  ---------------------------");
          viewMetadataFun("../ServerStorage/Test2", "Test2.h.1");
          Console.WriteLine("\n  View Metadata for : Test2.h.1");
          Console.WriteLine("\n  ---------------------------");
          viewFileContent("../ServerStorage/Test2", "Test2.h.1");
          testQuery();
          Console.WriteLine("\n  Demonstrating Requirement 9. Display Files with No parent");
          Console.WriteLine("\n  ---------------------------");
          CsMessage msg = new CsMessage();
          msg.add("to", CsEndPoint.toString(serverEndPoint));
          msg.add("from", CsEndPoint.toString(endPoint_));
          msg.add("command", "noParent");
          msg.add("type", "noParent");
         
          translater.postMessage(msg);
          
       }
    }

    //test Checkin functionality
    private void testCheckin()
    {
          Console.WriteLine("\n\n  Demonstrating requirement 4. CheckIn -provides the functionality to start a new package version by accepting files, appending version numbers to their filenames, providing dependency and category information, creating metadata, and storing the files in a designated location.");
          Console.WriteLine("\n  CheckIn New file  - Message.h");
          Console.WriteLine("\n  ---------------------------");
          Author.Text = "Fawcett";
          description.Text = "File For Testing";
          catList.Add("Header");
          CheckIn("../ClientStorage/Test3", "Message.h");
          Console.WriteLine("\n  CheckIn file with OPEN status - Test1.h.2");
          Console.WriteLine("\n  ---------------------------");
          CheckIn("../ClientStorage/Test1", "Test1.h");
          Console.WriteLine("\n  Demonstrating requirement 5. Change status of file");
          Console.WriteLine("\n  Trying to close checkin for Test2.cpp.1. It will change the status to PENDING as status of one child is OPEN");
          Console.WriteLine("\n  ------------------------------------------------------------------------------------------------------------");
          closeCheckIn("../ServerStorage/Test2", "Test2.cpp.1");
          Console.WriteLine("\n  CheckIn file with PENDING status - Test2.cpp.2");
          Console.WriteLine("\n  ---------------------------");
          CheckIn("../ClientStorage/Test2", "Test2.cpp");
          Console.WriteLine("\n  Change status of Test2.h.1 to Closed");
          Console.WriteLine("\n  ---------------------------");
          closeCheckIn("../ServerStorage/Test2", "Test2.h.1");
          Console.WriteLine("\n  Trying to close checkin for Test2.cpp.1. It will change the status to CLOSED.");
          Console.WriteLine("\n  -----------------------------------------------------------------------------");
          closeCheckIn("../ServerStorage/Test2", "Test2.cpp.1");
          Console.WriteLine("\n  CheckIn file with CLOSED status - Test2.cpp.2");
          Console.WriteLine("\n  ---------------------------");
          catList.Add("CPP");
          CheckIn("../ClientStorage/Test2", "Test2.cpp");
          catList.Clear();
    }
    //Testing queries
    private void testQuery()
    {
            Console.WriteLine("\n  Demonstrating reqiuirement 8.Fetching file list using Query");
            Console.WriteLine("\n  Query: FileName - 'Test1' and Version - 1");
            Console.WriteLine("\n  -----------------------------------------");
            sendQuery("", "Test1", "", "1", "", "");
            Console.WriteLine("\n  Query: Child - 'Test2.h'and Description - 'File'");
            Console.WriteLine("\n  ------------------------------------------------");
            sendQuery("", "", "", "", "Fawcett", "for");
            Console.WriteLine("\n  Query: Category - cpp and child - 'Test2.h'");
            Console.WriteLine("\n  -------------------------------------------");
            sendQuery("cpp", "", "Test2.h", "", "", "");

        }
    //sending Query Msg
    private void sendQuery(string cat, string filename, string dep, string version, string author, string desc)
    {
           CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "search");
            msg.add("Category", cat);
            msg.add("FileName", filename);
            msg.add("Dependancy", dep);
            msg.add("Version", version);
            msg.add("Aname", author);
            msg.add("Description", desc);
            msg.add("type", "Search");
            translater.postMessage(msg);
    }
     //----< Enable window for test >------
    private void enableWindowTest()
    {
            fetchFiles();
            TestFun();
    }
   
      //----< Enable window>------
    private void enableWindow()
    {
            fetchFiles();
    }
    
    //----< fetch files >------
    private void fetchFiles()
    {
            PathTextBlock.Text = "ServerStorage";
            pathStack_.Push(storagePath);

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            msg.add("type", "CheckOut");
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);

            PathTextBlock3.Text = "ServerStorage";
            pathBrowse_.Push(storagePath);

            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "getDirs");
            msg2.add("path", pathBrowse_.Peek());
            msg2.add("type", "Browse");
            translater.postMessage(msg2);
            msg2.remove("command");
            msg2.add("command", "getFiles");
            translater.postMessage(msg2);

            PathTextBlock2.Text = "ClientStorage";
            pathClient_.Push(clientPath);

            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirs");
            msg1.add("path", pathClient_.Peek());
            msg1.add("type", "CheckIn");
            translater.postMessage(msg1);
            msg1.remove("command");
            msg1.add("command", "getFiles");
            translater.postMessage(msg1);
    }
     
     //----< Load window and set comm>------
    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      // start Comm
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = "localhost";
      endPoint_.port = Int32.Parse(port);
      serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;

      translater = new Translater();
      translater.listen(endPoint_);

      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();
      checkInTab.IsEnabled = false;
      checkOutTab.IsEnabled = false;
      browseTab.IsEnabled = false;
      queryTab.IsEnabled = false;
      
      Console.WriteLine("\n\n Demonstrating the requirement for 1. Connect to server  and 2. Multiple Client");
      Console.WriteLine("\n ------------------------------------------------------------------------------");
      connectToServerTest();
      
    }
    //----< strip off name of first part of path >---------------------

    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }
    //----< respond to mouse double-click on dir name >----------------

    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      // build path for selected dir
      string selectedDir = (string)DirList.SelectedItem;
      string path;
      if(selectedDir == "..")
      {
        if (pathStack_.Count > 1)  // don't pop off "Storage"
          pathStack_.Pop();
        else
          return;
      }
      else
      {
        path = pathStack_.Peek() + "/" + selectedDir;
        pathStack_.Push(path);
      }
      // display path in Dir TextBlcok
      PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
      
      // build message to get dirs and post it
      serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      msg.add("type","CheckOut");
      translater.postMessage(msg);
      
      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }
    
    //----< respond to mouse double-click on dir name >----------------
    private void DirList2_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
            // build path for selected dir
            string selectedDir = (string)DirList2.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathClient_.Count > 1)  // don't pop off "Storage"
                    pathClient_.Pop();
                else
                    return;
            }
            else
            {
                path = pathClient_.Peek() + "/" + selectedDir;
                pathClient_.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock2.Text = removeFirstDir(pathClient_.Peek());

            // build message to get dirs and post it

            serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathClient_.Peek());
            msg.add("type", "CheckIn");
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
    }

    //----< respond to mouse double-click on dir name >----------------
    private void DirList3_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      // build path for selected dir
      string selectedDir = (string)DirList3.SelectedItem;
      string path;
      if(selectedDir == "..")
      {
        if (pathBrowse_.Count > 1)  // don't pop off "Storage"
          pathBrowse_.Pop();
        else
          return;
      }
      else
      {
        path = pathBrowse_.Peek() + "/" + selectedDir;
        pathBrowse_.Push(path);
      }
      // display path in Dir TextBlcok
      PathTextBlock3.Text = removeFirstDir(pathBrowse_.Peek());
      
      // build message to get dirs and post it
      serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathBrowse_.Peek());
      msg.add("type","Browse");
      translater.postMessage(msg);
      
      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }
    //----< checkout file click function>---------------------------------

        private void CheckoutFiles_Click(object sender, RoutedEventArgs e)
        {
            if (FileList.SelectedItem == null || FileList.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to CheckOut";
            }
            else
            {
                string filePath = pathStack_.Peek();
                foreach (string file in FileList.SelectedItems)
                {
                    CheckOut(filePath, file);
                }
                statusBarText.Text = "Status: Files are sent for CheckOut";
                FileList.UnselectAll();
            }
        }


        //Filename text changed
        private void fileName1_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {

        }

        //----< checkIn file click function>---------------------------------
        private void chkInButton_Click(object sender, RoutedEventArgs e)
        {
            if (FileList2.SelectedItem == null || FileList2.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to CheckIn";
            }
            else if(Author.Text == "" || description.Text == "")
            {
                statusBarText.Text = "Status: Enter all the file details ";
            }
            else
            {
                string filePath = pathClient_.Peek();
                CheckIn(filePath, (string)FileList2.SelectedItem);
                statusBarText.Text = "Status: Files are sent for CheckIn";
                FileList2.UnselectAll();
                Author.Clear();
                description.Clear();
                catList.Clear();
                depList.Clear();
            }
        }    

        //----< filebox3 double click function>---------------------------------
        private void filesListBox3_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (FileList3.SelectedItem == null || FileList3.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to View Content";
            }
            else
            {
                viewFileContent(pathBrowse_.Peek(), (string)FileList3.SelectedItem);
                statusBarText.Text = "Status: Sent request for View File Content";
            }
            return;
        }
        
        // Show file in window pop up
        private void showFile(string fileName, Window1 popUp)
        {
            string path = System.IO.Path.Combine("../../../../SaveFiles\\", fileName);
            Paragraph paragraph = new Paragraph();
            string fileText = "";
            try
            {
               fileText = System.IO.File.ReadAllText(path);
              
            }
            finally
            {
                paragraph.Inlines.Add(new Run(fileText));
            }

            // add code text to code view
            popUp.codeView.Blocks.Clear();
            popUp.codeView.Blocks.Add(paragraph);

        }

        // Refresh button double click
        private void Refresh1Button_Click(object sender, RoutedEventArgs e)
        {
            FileList2.UnselectAll();
            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirs");
            msg1.add("path", pathClient_.Peek());
            msg1.add("type", "CheckIn");
            translater.postMessage(msg1);
            msg1.remove("command");
            msg1.add("command", "getFiles");
            translater.postMessage(msg1);
        }

        // Refresh button double click
        private void Refresh2Button_Click(object sender, RoutedEventArgs e)
        {
            FileList.UnselectAll();
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            msg.add("type", "CheckOut");
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        // view metadata button click
        private void ViewMetadata_Click(object sender, RoutedEventArgs e)
        {
            if (FileList3.SelectedItem == null || FileList3.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to View Metadata";
            }
            else
            {
                viewMetadataFun(pathBrowse_.Peek(), (string)FileList3.SelectedItem);
                statusBarText.Text = "Status: Request sent for View Metadata";
            }
        }

        // view file button click
        private void ViewFile_Click(object sender, RoutedEventArgs e)
        {
            if (FileList3.SelectedItem == null || FileList3.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to View Content";
            }
            else
            {
                viewFileContent(pathBrowse_.Peek(), (string)FileList3.SelectedItem);
                statusBarText.Text = "Status: Sent request for View File Content";
            }
            return;
        }

        //Connect button click 
        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            connectToServer();
            checkInTab.IsEnabled = true;
            checkOutTab.IsEnabled = true;
            browseTab.IsEnabled = true;
            queryTab.IsEnabled = true;
            Metadata.Text = "";
            connectButton.IsEnabled = false;
            statusBarText.Text = "Status: Connected to server";
        }

        // adds category in category list
        private void category_Click(object sender, RoutedEventArgs e)
        {
            catList.Add(categories.Text);
            categories.Clear();
        }
        // adds dependancy in dependancy list
        private void AddDep_Click(object sender, RoutedEventArgs e)
        {
            depList.Add(dep.Text);
            dep.Clear();
        }

        // search button click
        private void search_Click(object sender, RoutedEventArgs e)
        {
            if( cat1.Text != "" ||  fileName1.Text != "" || dep1.Text !=""  || version1.Text != "" || aname1.Text != "" || desc1.Text != "")
            {
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "search");
                msg.add("Category", cat1.Text);
                msg.add("FileName", fileName1.Text);
                msg.add("Dependancy", dep1.Text);
                msg.add("Version", version1.Text);
                msg.add("Aname", aname1.Text);
                msg.add("Description", desc1.Text);
                msg.add("type", "Search");
                translater.postMessage(msg);
                searchList.Items.Clear();
                statusBarText.Text = "Status: Sent request for Search files";
            }
            else
            {
                statusBarText.Text = "Status: Give atleast one criteria";
            }
        }

        //closes check in for selected files
        private void close_Click(object sender, RoutedEventArgs e)
        {
            if (FileList.SelectedItem == null || FileList.SelectedItems.Count == 0)
            {
                statusBarText.Text = "Status: Select file to Close CheckIn";
            }
            else
            {
                foreach (string file in FileList.SelectedItems)
                {
                    closeCheckIn(pathStack_.Peek(),file);
                }
                statusBarText.Text = "Status: Sent request for Close CheckIn";
            }
            FileList.UnselectAll();
            return;
        }

        //displays files with no parent
        private void noParent_Click(object sender, RoutedEventArgs e)
        {
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "noParent");
            msg.add("type", "noParent");
            translater.postMessage(msg);
        }
    }
}
