#pragma once
/////////////////////////////////////////////////////////////////////
// version.h - manages version numbering for all files held        //
//                in the Repository.                               //
// ver 1.1                                                         //
// Language:    C++, Visual Studio 2017                             //
// Platform:     Windows 7                                          //
// Application:  Remote Repository Prototypes                        //
//                                                                  //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett       								    //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* manages version numbering for all files held in the Repository. 
*
*
* Public Interface:
* =================
* std::string getFileName(std::string file_name, std::string pattern) - Get file name by removing extension
* int getVersion(std::string file_name, std::string pattern) - Get version of file
* std::string getFile(std::string file_name, std::string pattern) - Get file by removing extension
*
* Required Files:
* ---------------
* version.h, version.cpp
*
* Maintenance History:
* --------------------
* ver 1.1 : provided the methods for file check in
* - first release
*/
#include <string>
#include <iostream>


namespace NoSqlDb
{
	//Class provides methods for version
	class version
	{
		public:
			static std::string getFileName(std::string file_name, std::string pattern);
			static int getVersion(std::string file_name, std::string pattern);
			static std::string getFile(std::string file_name, std::string pattern);
	};

	//Get file name by removing extension
	std::string version::getFileName(std::string file_name, std::string pattern)
	{
		std::size_t pos = file_name.find_last_of(pattern);
		std::string file = file_name.substr(0, pos);
		return file;
	}
	//Get version of file
	int version::getVersion(std::string file_name, std::string pattern)
	{
		std::size_t pos = file_name.find_last_of(pattern);
		std::string v = file_name.substr(pos + 1);
		int value = atoi(v.c_str());
		value++;
		return value;
	}
	//Get file by removing extension
	std::string version::getFile(std::string file_name, std::string pattern)
	{
		std::size_t pos = file_name.find_last_of(pattern);
		std::string v = file_name.substr(pos + 1);
		return v;
	}

}