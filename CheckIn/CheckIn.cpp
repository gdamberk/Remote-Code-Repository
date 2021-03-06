/////////////////////////////////////////////////////////////////////
// CheckIn.cpp - provides the functionality to start a new package //
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
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "CheckIn.h"
using namespace NoSqlDb;

#define TEST_CHECKIN
#ifdef  TEST_CHECKIN
int main()
{
    DbCore<PayLoad> db;
	checkin chin;
	db = chin.checkInFile("Test1:Test1.h", db, "Fawcett", "Test1.h for testing", "Header", "");
	db = chin.closeStatus("Test1:Test1.h.1", db);
    return 0;
}
#endif

