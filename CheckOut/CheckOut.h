#pragma once
/////////////////////////////////////////////////////////////////////
// CheckOut.h - retrieves package files, removing version          //
//            information from their filenames. Retrieved files    //
//			will be copied to a specified directory.               //
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
* This package retrieves package files, removing version information f
* rom their filenames. Retrieved files will be copied to a specified directory.
*
* Public Interface:
* =================
*  void checkoutFile(const std::string& file_name, DbCore<PayLoad>& db, bool singleFile) - to check out the folder and it's children
*  void checkFolder(const std::string& path) - to check if directory exists
*  std::string checkout::checkoutFile1(const std::string &file_name) - to check out the folder and it's children
*
* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* PayLoad.h, PayLoad.cpp
* FileSystem.h, FileSystem.cpp
* version.h, version.cpp
* DateTime.h, DateTime.cpp
*
* Maintenance History:
* --------------------
* ver 1.1 : provided the methods for file check out
* - first release
*/

#include <string>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../version/version.h"
#include "../PayLoad/PayLoad.h"
#include "../DbCore/DbCore.h"
#include <iostream>

using namespace FileSystem;
namespace NoSqlDb
{
	//CLass provides method to check out files
	class checkout
	{
		public:
		static void checkoutFile(const std::string& file_name, DbCore<PayLoad>& db, bool singleFile);
		static void checkFolder(const std::string& path);
		static std::string checkoutFile1(const std::string &file_name);
	};

	// to check out the folder and it's children
	void checkout::checkoutFile(const std::string &file_name, DbCore<PayLoad>& db, bool singleFile)
	{
		std::string srcDir = "../ServerStorage/" ;	
		std::string dstDir = "../SendFiles/";

		std::string folder = version::getFileName(file_name, "/:");
		std::string file_v = version::getFile(file_name, "/:");
		std::string file = version::getFileName(file_v, "/.");
		std::string srcPath = srcDir +folder + "/" + file_v;
 		std::string dstPath = dstDir + file;
		if (File::exists(srcPath))
		{
			checkFolder(dstDir);
			bool result = File::copy(srcPath, dstPath);
			if (!singleFile)
			{
				Keys keys = db[file_name].children();
				if (keys.size() > 0)
				{
					for (auto k : keys)
					{
						checkoutFile(k, db, false);
					}
				}
			}
		}
		else {
			std::cout << "\n File : " << srcPath << "  does not exist";
		}
	}


	// to check out the folder and it's children
	std::string checkout::checkoutFile1(const std::string &file_name)
	{
		std::string srcDir = "../ServerStorage/";
		std::string dstDir = "../SendFiles/";

		std::string folder = version::getFileName(file_name, "/:");
		std::string file_v = version::getFile(file_name, "/:");
		std::string file = version::getFileName(file_v, "/.");
		std::string srcPath = srcDir + folder + "/" + file_v;
		std::string dstPath = dstDir + file;
		std::cout << "\n File : " << srcPath << "  " << dstPath;
		if (File::exists(srcPath))
		{
			
			checkFolder(dstDir);
			bool result = File::copy(srcPath, dstPath);
		}
		else {
			std::cout << "\n File : " << srcPath << "  does not exist";
		}
		return file;
	}

	//to check if directory exists
	void checkout::checkFolder(const std::string& path)
	{
		if (!Directory::exists(path))
		{
			Directory::create(path);
		}
	}

}