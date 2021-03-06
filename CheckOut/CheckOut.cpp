/////////////////////////////////////////////////////////////////////
// CheckOut.cpp - retrieves package files, removing version          //
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

#include "CheckOut.h"
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>


using namespace NoSqlDb;

#define TEST_CHECKOUT
#ifdef TEST_CHECKOUT

int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> elem = makeElement<PayLoad>("Gauri", "File for testing");
	PayLoad pl;
	pl.value("../ClientRepo/Test1/Test1.h.1");
	pl.categories().push_back("header");
	elem.payLoad(pl);
	db["Test1:Test1.h.1"] = elem;
	std::cout << "\n Checkout  file : Test2.cpp.1 and it's children";
	std::string file_name = "Test2:Test2.cpp.1";
	checkout::checkoutFile1(file_name);
    return 0;
}

#endif // TEST_CHECKOUT


