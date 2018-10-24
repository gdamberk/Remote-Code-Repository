#pragma once
///////////////////////////////////////////////////////////////////////
// ServerPrototype.h - Console App that processes incoming messages  //
// ver 1.2                                                          //
// Language:    C++, Visual Studio 2017                              //
// Platform:     Windows 7                                          //
// Application: Remote Repository Prototypes                        //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett                                          //
// CSE687 - Object Oriented Design, Spring 2018                     //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
* Public Interface:
* =================
*   void start() - start server's instance of Comm
*   void stop() - stop Comm instance
*   void addMsgProc(Key key, ServerProc proc) -  add ServerProc callable object to server's dispatcher
*   void processMessages() - start processing messages on child thread
*   void postMessage(MsgPassingCommunication::Message msg) - pass message to Comm for sending
*   MsgPassingCommunication::Message getMessage() - get message from Comm 
*   static Dirs getDirs(const SearchPath& path = storageRoot) - fetch list of directories
*   static Files getFiles(const SearchPath& path = storageRoot) - fetch list of files
*   void Server::getChilds(MsgPassingCommunication::Message msg) - get child from DB for checkout
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*  ver 1.1 : 4/12/2018
*  - handle done message
*  ver 1.2 : 4/30/2018
*  - updated to checkout childs
*/
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include <windows.h>
#include <tchar.h>
#include "../PayLoad/PayLoad.h"
#include "../CheckOut/CheckOut.h"
#include "../CheckIn/CheckIn.h"


using namespace NoSqlDb; 
namespace Repository
{
  using File = std::string;
  using Files = std::vector<File>;
  using Dir = std::string;
  using Dirs = std::vector<Dir>;
  using SearchPath = std::string;
  using Key = std::string;
  using Msg = MsgPassingCommunication::Message;
  using ServerProc = std::function<Msg(Msg)>;
  using MsgDispatcher = std::unordered_map<Key,ServerProc>;
  
  const SearchPath storageRoot = "../ServerStorage";  // root for all server file storage
  const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint

  class DbProvider
  {
  public:
	  DbCore<PayLoad>& db() { return db_; }
  private:
	  static DbCore<PayLoad> db_;
  };
  DbCore<PayLoad> DbProvider::db_;

  class Server
  {
  public:
    Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
    void start();
    void stop();
    void addMsgProc(Key key, ServerProc proc);
    void processMessages();
    void postMessage(MsgPassingCommunication::Message msg);
	void getChilds(MsgPassingCommunication::Message msg);
    MsgPassingCommunication::Message getMessage();
    static Dirs getDirs(const SearchPath& path = storageRoot);
    static Files getFiles(const SearchPath& path = storageRoot);
  private:
    MsgPassingCommunication::Comm comm_;
    MsgDispatcher dispatcher_;
    std::thread msgProcThrd_;
  };
  //----< initialize server endpoint and give server a name >----------

  inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
    : comm_(ep, name) {}

  //----< start server's instance of Comm >----------------------------

  inline void Server::start()
  {
    comm_.start();
  }
  //----< stop Comm instance >-----------------------------------------

  inline void Server::stop()
  {
    if(msgProcThrd_.joinable())
      msgProcThrd_.join();
    comm_.stop();
  }
  //----< pass message to Comm for sending >---------------------------

  inline void Server::postMessage(MsgPassingCommunication::Message msg)
  {
    comm_.postMessage(msg);
  }
  //----< get message from Comm >--------------------------------------

  inline MsgPassingCommunication::Message Server::getMessage()
  {
    Msg msg = comm_.getMessage();
    return msg;
  }
  //----< add ServerProc callable object to server's dispatcher >------

  inline void Server::addMsgProc(Key key, ServerProc proc)
  {
    dispatcher_[key] = proc;
  }

  //----< get child from DB for checkout >--------------------------------------
  inline void Server::getChilds(MsgPassingCommunication::Message msg)
  {
	  DbCore<PayLoad> db;
	  DbProvider dbp;
	  db = dbp.db();
	  checkout chout;
	  std::string path = msg.value("path");
	  std::size_t pos = path.find_last_of("/");
	  std::string v = path.substr(pos + 1);
	  std::string file_name = v + ":" + msg.value("fileName");
	  Keys keys = db[file_name].children();

	  if (keys.size() > 0)
	  {
		  for (auto k : keys)
		  {
			  Msg reply;
			  reply.to(msg.from());
			  reply.from(msg.to());
			  std::size_t pos = k.find_last_of(":");
			  std::string folder = k.substr(0, pos);
			  std::string filename = k.substr(pos + 1);

			  reply.attribute("path", "../ServerStorage/"+ folder );
			  reply.attribute("fileName", filename);
			  reply.attribute("Comment", "File Sent");
			  reply.attribute("type", msg.value("type"));
			  reply.file(chout.checkoutFile1(k));
			  reply.command("Done");
			  postMessage(reply);
		  }
	  }
  }

  //----< start processing messages on child thread >------------------

  inline void Server::processMessages()
  {
    auto proc = [&]()
    {
      if (dispatcher_.size() == 0)
      {
        std::cout << "\n  no server procs to call";
        return;
      }
      while (true)
      {
        Msg msg = getMessage();
        std::cout << "\n  Received message: " << msg.command() << " from " << msg.from().toString();
        if (msg.containsKey("verbose"))
        {
          std::cout << "\n";
          msg.show();
        }
        if (msg.command() == "serverQuit")
          break;
		if (msg.command() == "Done")
		{
			msg.show();
			if (msg.value("type") == "CheckOut")
			{
				getChilds(msg);
			}

		}
		else {
			Msg reply = dispatcher_[msg.command()](msg);
			if (msg.to().port != msg.from().port)  // avoid infinite message loop
			{
				postMessage(reply);
				msg.show();
				std::cout << "\n  Sending message: " << reply.command() << " from " << reply.from().toString();
				reply.show();
			}
			else
				std::cout << "\n  server attempting to post to self";
		}
      }
      std::cout << "\n  server message processing thread is shutting down";
    };
    std::thread t(proc);
    std::cout << "\n  starting server thread to process messages";
    msgProcThrd_ = std::move(t);
  }
}