/////////////////////////////////////////////////////////////////////
// version.cpp - manages version numbering for all files held        //
//                in the Repository.                               //
// ver 1.1                                                         //
// Language:    C++, Visual Studio 2017                             //
// Platform:     Windows 7                                          //
// Application:  Remote Repository Prototypes                          //
//                                                                  //
// Author Name : Gauri Amberkar                                     //
// Source: Dr. Jim Fawcett       								    //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "version.h" 

using namespace NoSqlDb;
#define TEST_VERSION
#ifdef  TEST_VERSION

int main()
{
	version v;
	std::cout << v.getFile("Test:Test1.h.1", "/:");
	std::cout << v.getFileName("Test:Test1.h.1", "/.");
    return 0;
}
#endif //  TEST_VERSION

