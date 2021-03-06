/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.1                                                          //
// Language:    C++, Visual Studio 2017                              //
// Platform:     Windows 7                                          //
// Application: Remote Repository Prototypes                        //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett                                          //
// CSE687 - Object Oriented Design, Spring 2018                     //
///////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>


namespace MsgPassComm = MsgPassingCommunication;

using namespace FileSystem;
using namespace Repository;

using Msg = MsgPassingCommunication::Message;

//fetch list of files
Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

//fetch list of directories
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

//show message
template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

//reply for serverquit message
std::function<Msg(Msg)> serverQuit = [](Msg msg) {
  Msg reply = msg;
  reply.command(msg.command());
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//reply for getFiles message
std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  reply.attribute("path", msg.value("path"));
  reply.attribute("type", msg.value("type"));

  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

//reply for getDirs message
std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  reply.attribute("path", msg.value("path"));
  reply.attribute("type", msg.value("type"));
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

//reply for checkIn message
std::function<Msg(Msg)> checkIn = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	reply.attribute("type", msg.value("type"));
	reply.attribute("name", msg.value("name"));
	reply.attribute("description", msg.value("description"));
	reply.attribute("categories", msg.value("categories"));
	reply.attribute("dependancy", msg.value("dependancy"));
	reply.command("ready");
	return reply;
};

//reply for fileTransfer message
std::function<Msg(Msg)> fileTransfer = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	reply.attribute("Comment", "File Received");
	reply.attribute("type", msg.value("type"));
	reply.command("Done");

	DbCore<PayLoad> db;
	DbProvider dbp;
	db = dbp.db();
	checkin chin;
	std::string path = msg.value("path");
	std::size_t pos = path.find_last_of("/");
	std::string v = path.substr(pos + 1);
	std::cout << "\n  Checking In  file :  " + msg.value("fileName");
	db = chin.checkInFile(v + ":" + msg.value("fileName"), db, msg.value("name"), msg.value("description"), msg.value("categories"), msg.value("dependancy"));
	dbp.db() = db;
	std::cout << "\n\n";
	showDb(db);
	DbElement<PayLoad> elem = makeElement<PayLoad>("Gauri", "File for testing");
	std::cout << "\n\n";
	elem.payLoad().showDb(db);

	return reply;
};

//reply for checkOut message
std::function<Msg(Msg)> checkOut = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	checkout ch;
	std::string path = msg.value("path");
	std::size_t pos = path.find_last_of("/");
	std::string v = path.substr(pos + 1);
	reply.attribute("Comment", "File Sent");
	reply.attribute("type", msg.value("type"));
	std::cout << "\n  Checking Out  file :  " + msg.value("fileName");
	reply.file(ch.checkoutFile1(v + ":" + msg.value("fileName")));
	reply.command("fileTransfer");
	return reply;
};

//reply for sendMetadata message
std::function<Msg(Msg)> SendMetadata = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	DbCore<PayLoad> db;
	DbProvider dbp;
	db = dbp.db();
	std::string path = msg.value("path");
	std::string filename = msg.value("fileName");
	std::size_t pos = path.find_last_of("/");
	std::string v = path.substr(pos + 1);
	std::string file_name = v + ":" + filename;
	DbElement<PayLoad> elem = db[file_name];
	std::string metadata;
	metadata = metadata + "Name - " + elem.name();
	metadata = metadata + "|Description - " + elem.descrip();
	std::string date = elem.dateTime();
	metadata = metadata + "|Date Time - " + date;
	Children children = elem.children();
	if (children.size() > 0)
	{
		metadata = metadata + "|Children - ";
		for (auto key : children)
		{
			metadata = metadata + " " + key;
		}
	}
	metadata = metadata + "|Status - " + elem.payLoad().status();
	metadata = metadata + "|Value - " + elem.payLoad().value();
	metadata = metadata + "|Categories - ";
	for (std::string cat : elem.payLoad().categories())
	{
		metadata = metadata + " " + cat;
	}
	reply.attribute("Content", metadata);
	reply.attribute("type", msg.value("type"));
	reply.command("metadata");
	std::cout << "\n View Metadata";
	std::cout << "\n\n";
	showHeader();
	showElem(elem);
	return reply;
};

//reply for connctServer message
std::function<Msg(Msg)> connectServer = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("Comment", "Connected to Server");
	reply.attribute("type", msg.value("type"));
	reply.command("Done");
	return reply;
};

//reply for sendFile message
std::function<Msg(Msg)> sendFile = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("sendFile");
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	reply.file(msg.value("fileName"));
	reply.attribute("Comment", "File Sent");
	return reply;
};

//reply for close checkin message
std::function<Msg(Msg)> close = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("Done");
	reply.attribute("path", msg.value("path"));
	reply.attribute("fileName", msg.value("fileName"));
	reply.attribute("Comment", "Status changed");
	reply.attribute("type", msg.value("type"));
	DbCore<PayLoad> db;
	DbProvider dbp;
	db = dbp.db();
	checkin chin;
	std::string path = msg.value("path");
	std::size_t pos = path.find_last_of("/");
	std::string v = path.substr(pos + 1);
	std::cout << "\n Closing Checkin for :  " + msg.value("fileName");
	db = chin.closeStatus(v + ":" + msg.value("fileName"),db);
	dbp.db() = db;
	return reply;
};


//reply for search file message
std::function<Msg(Msg)> search = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("fileList");
	reply.attribute("type", msg.value("type"));
	DbCore<PayLoad> db,db1;
	DbProvider dbp;
	db = dbp.db();
	Query<PayLoad> q1(db), q2(db), q4(db);
	Conditions<PayLoad> conds1, conds2;
	conds1.key(msg.value("FileName"));
	if (msg.value("Dependancy") != "")
	{
		conds2.key(msg.value("Dependancy"));
		q4.select(conds2);
		Keys keys = q4.keys();
		conds1.children(keys);
	}
	conds1.version(msg.value("Version"));
	conds1.name(msg.value("Aname"));
	conds1.description(msg.value("Description"));
	q1.select(conds1);
	if (msg.value("Category") != "")
	{
		std::string category = msg.value("Category");
		auto hasCategory = [&category](DbElement<PayLoad>& elem) {
			return (elem.payLoad()).hasCategory(category);
		};
		q2.select(hasCategory);
		q1.query_or(q2);
	}
	std::string searchResult = "";
	if (!q1.keys().empty())
	{
		std::cout << "\n\n Query Result :";
		showHeader();
		for (std::string key : q1.keys())
		{
			showElem(db[key]);
			searchResult = key + "|" + searchResult;
		}
		std::cout << "\n\n";
	}
	reply.attribute("result", searchResult);
	dbp.db() = db;
	return reply;
};

//reply for noParent message
std::function<Msg(Msg)> noParent = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("noParent");
	DbCore<PayLoad> db, db1;
	DbProvider dbp;
	db = dbp.db();
	Conditions<PayLoad> conds1;
	Query<PayLoad> q1(db);
	std::string result = "";
	std::cout << "\n \nFiles with no Parent :";
	showHeader();
	for (auto key : db.keys())
	{
		Keys keys{ key };
		conds1.children(keys);
		q1.select(conds1);
		if (q1.keys().empty())
		{
			result = key + "|" + result;
			showElem(db[key]);

		}
	}
	std::cout << "\n\n";
	reply.attribute("result", result);
	reply.attribute("Comment", "Get File List with no Parent");
	return reply;
};

//Add records in Db 
void AddRecords()
{
	DbCore<PayLoad> db;
	DbProvider dbp;
	DbElement<PayLoad> elem = makeElement<PayLoad>("Gauri", "File for testing");
	PayLoad pl;

	checkin chin;
	db = chin.checkInFile("Test1:Test1.h", db, "Fawcett", "Test1.h for testing", "Header","");
	db = chin.closeStatus("Test1:Test1.h.1", db);
	db = chin.checkInFile("Test1:Test1.h", db, "Fawcett", "Test1.h for testing", "Header","");
	db = chin.checkInFile("Test2:Test2.h", db, "Fawcett", "Test2.h for testing", "Header", "");
	db = chin.checkInFile("Test2:Test2.cpp", db, "Fawcett", "Test2.cpp for testing", "CPP", "Test2:Test2.h.1|Test1:Test1.h.1");
	std::cout << "\n\n";
	showDb(db);
	std::cout << "\n\n";
	elem.payLoad().showDb(db);
	std::cout << "\n\n";
	dbp.db() = db;
}

#define TEST_SERVER
#ifdef TEST_SERVER
//Main function
int main()
{
  SetConsoleTitleA("Server Console");
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ========================== \n";
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  std::cout << "\n  Testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n"; 
  std::cout << "\n Testing requirement for: ";
  std::cout << "\n ---------------------------- \n1.Connect to server \n2. Multiple Client \n3.CheckOut \n4.CheckIn \n5.Change Status \n6.View File \n7.View Metadata  \n8.Search Files using Query \n9.Files with No Parent \n";
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", serverQuit);
  server.addMsgProc("checkIn", checkIn);
  server.addMsgProc("fileTransfer", fileTransfer);
  server.addMsgProc("checkOut", checkOut);
  server.addMsgProc("SendMetadata", SendMetadata);
  server.addMsgProc("connectServer", connectServer);
  server.addMsgProc("sendFile", sendFile);
  server.addMsgProc("close", close);
  server.addMsgProc("search", search);
  server.addMsgProc("noParent", noParent);
  server.processMessages();

  AddRecords();
  Msg msg(serverEndPoint, serverEndPoint);
  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

#endif

