/////////////////////////////////////////////////////////////////////////
// Sockets.cpp - C++ wrapper for Win32 socket api                      //
// ver 5.2                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com //
//---------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                      //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////

#include "Sockets.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>
#include "../Utilities/Utilities.h"

using namespace Sockets;
using Util = Utilities::StringHelper;
template<typename T>
using Conv = Utilities::Converter<T>;
using Show = StaticLogger<1>;

/////////////////////////////////////////////////////////////////////////////
// SocketSystem class members

//----< constructor starts up sockets by loading winsock lib >---------------

SocketSystem::SocketSystem()
{
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    Show::write("\n  WSAStartup failed with error = " + Conv<int>::toString(iResult));
  }
}
//-----< destructor frees winsock lib >--------------------------------------

SocketSystem::~SocketSystem()
{
  int error = WSACleanup();
  Show::write("\n  -- Socket System cleaning up\n");
}
