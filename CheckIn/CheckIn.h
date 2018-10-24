#pragma once
/////////////////////////////////////////////////////////////////////
// CheckIn.h - provides the functionality to start a new package   //
//        version by accepting files, appending version numbers to //
//        their filenames, providing dependency and category       //
//        information, creating metadata, and storing the files in //
//        a designated location.                                   //
// ver 1.1                                                         //
// Language:    C++, Visual Studio 2017                             //
// Platform:     Windows 7                                          //
// Application: Software Repository Testbed                         //
//                                                                  //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett       								    //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides check in class
* which contains the methods for check in file, close status
*
* Public Interface:
* =================
* NoSqlDb::DbCore<PayLoad> checkInFile(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db) - to check in the file
* NoSqlDb::DbCore<PayLoad> closeStatus(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db) - Method to close the status 
* Keys getKeys(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db) - Get keys for particular key
* std::string getMaxVersion(Keys keys, NoSqlDb::DbCore<PayLoad>& db, std::string pattern) - Get max version of file
* void checkFolder(const std::string& file_name) - Check if directory exists
* std::vector<std::string> checkin::parse(std::strings s) -  used to parse the string
*
* Required Files:
* ---------------
* Query.h, Query.cpp
* PayLoad.h, PayLoad.cpp
* FileSystem.h, FileSystem.cpp
* version.h, version.cpp
* DateTime.h, DateTime.cpp
*
* Maintenance History:
* --------------------
* ver 1.1 : provided the methods for file check in
* - first release
*/

#include <string>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Query/Query.h"
#include "../PayLoad/PayLoad.h"
#include "../version/version.h"
#include <iostream>

using namespace FileSystem;
namespace NoSqlDb
{
	// provide the methods for file check in
	class checkin 
	{
	    public:
		static NoSqlDb::DbCore<PayLoad> checkInFile(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db, const std::string& name, const std::string& description, const std::string& category, const std::string& dependancy);
		static NoSqlDb::DbCore<PayLoad> closeStatus(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db);
		static NoSqlDb::DbCore<PayLoad> updateParent(const std::string& file_name, const std::string& name, NoSqlDb::DbCore<PayLoad>& db);
		static Keys getKeys(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db);
		static std::string getMaxVersion(Keys keys, NoSqlDb::DbCore<PayLoad>& db, std::string pattern);
		static void checkFolder(const std::string& file_name);
		static std::vector<std::string> parse(std::string s);
		
	};

	//to check in the file
	NoSqlDb::DbCore<PayLoad> checkin::checkInFile(const std::string &file_name, NoSqlDb::DbCore<PayLoad>& db, const std::string& Aname, const std::string& description, const std::string& category, const std::string& dependancy)
	{   std::string folder = version::getFileName(file_name, "/:");
		std::string srcDir = "../SendFiles/";
		std::string dstDir = "../ServerStorage/" + folder + "/";
		checkFolder(dstDir);
		std::string file_v = version::getFile(file_name, "/:");
		std::string name;
		std::vector<std::string> categories = parse(category);
		std::vector<std::string> dependancies = parse(dependancy);
		bool result;
		Keys keys = getKeys(file_name, db);
		if (keys.empty()){
			name = file_v + ".1";
			dstDir = dstDir + name;
			result = File::copy(srcDir + file_v, dstDir);
			if (result) {
				DbElement<PayLoad> elem = makeElement<PayLoad>(Aname,description);
				elem.dateTime(DateTime().now());
				PayLoad pl;
				pl.value(dstDir);
				pl.categories() = categories;
				elem.children() = dependancies;
				elem.payLoad(pl);
				db[folder+ ":"+ name] = elem;}}
		else{
			Key max_key = getMaxVersion(keys, db, "/.");
			std::string fileV = version::getFile(max_key, "/:");
			int value = version::getVersion(fileV, "/.");
			DbElement<PayLoad> elem = db[max_key];
			elem.name(Aname);
			elem.descrip(description);
			elem.payLoad().categories() = categories;
			elem.children() = dependancies;
			if (elem.payLoad().status() == "OPEN"){
				dstDir = dstDir + fileV;
				result = File::copy(srcDir + file_v, dstDir);
				db[max_key] = elem;}
			else{
				name = file_v + "." + std::to_string(value);
			    dstDir = dstDir.append(name);
				result = File::copy(srcDir + file_v, dstDir);
				if (result) {
					elem.dateTime(DateTime().now());
					elem.payLoad().value(dstDir);
					elem.payLoad().status("OPEN");
					db[folder + ":" + name] = elem;
					updateParent(max_key, folder + ":" + name, db);
				}}} return db;
	}

	//Method to close the status 
	NoSqlDb::DbCore<PayLoad> checkin::closeStatus(const std::string &file_name, NoSqlDb::DbCore<PayLoad>& db)
	{
		DbElement<PayLoad> elem = db[file_name];
		DbElement<PayLoad> childElem ;
		Children children = elem.children();
		bool flag = false;
		std::cout << "\n\n Trying to change the status of " << file_name;
		if (children.size() > 0)
		{
			for (auto key : children)
			{
				childElem = db[key];
				if (childElem.payLoad().status() == "OPEN")
					flag = true;
			}
		}
		if (flag)
		{
			elem.payLoad().status("PENDING");
			std::cout << "\n\n Status = PENDING  for file " << file_name;
		}
		else {
			elem.payLoad().status("CLOSED");	
		}
		db[file_name] = elem;
	elem.payLoad().showDb(db);
		return db;
	}

	//Get max version of file
	std::string checkin::getMaxVersion(Keys keys, NoSqlDb::DbCore<PayLoad>& db, std::string pattern)
	{
		std::string max_version = "0";
		Key max_key = "";
		for (auto k : keys)
		{
			DbElement<PayLoad> elem = db[k];
			std::string folder = version::getFileName(k, "/:");
			std::string fileName = version::getFile(k, "/:");
			std::size_t pos = fileName.find_last_of("/.");
			std::string v = fileName.substr(pos + 1);
			if (max_version < v)
			{
				max_version = v;
				max_key = k;
			}
		}
		return max_key;
	}
	// Get keys for particular key
	Keys checkin::getKeys(const std::string& file_name, NoSqlDb::DbCore<PayLoad>& db)
	{
		Query<PayLoad> q1(db);
		Conditions<PayLoad> conds1;
		conds1.key(file_name);
		q1.select(conds1);
		return q1.keys();
	}

	//Update parent children
	NoSqlDb::DbCore<PayLoad> checkin::updateParent(const std::string &file_name, const std::string &name, NoSqlDb::DbCore<PayLoad>& db)
	{
		Query<PayLoad> q1(db);
		Keys keys{ file_name };
		Conditions<PayLoad> conds1;
		conds1.children(keys);
		q1.select(conds1);
		if (q1.keys().size() > 0)
		{
			for (auto k : q1.keys())
			{
				db[k].addChildKey(name);
			}
		}
		return db;
	}
	//Check if directory exists
	void checkin::checkFolder(const std::string& path)
	{
		if (!Directory::exists(path))
		{
			Directory::create(path);
		}	
	}

	//used to parse the string
	std::vector<std::string> checkin::parse(std::string s)
	{
		std::string delimiter = "|";
		std::vector<std::string> parsed;
		size_t pos = 0;
		std::string token;
		if (s != "")
		{
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				parsed.push_back(token);
				s.erase(0, pos + delimiter.length());
			}
			parsed.push_back(s);
		}
		return parsed;
	}
}
