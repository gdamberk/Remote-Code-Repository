#pragma once
/////////////////////////////////////////////////////////////////////////
// Message.h - defines message structure used in communication channel //
// ver 1.2                                                             //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017        //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines an EndPoint struct and a Message class.  
*  - Endpoints define a message source or destination with an address and port number.
*  - Messages have an HTTP style structure with a set of attribute lines containing
*    name:value pairs.
*  - Message have a number of getter, setter methods for common attributes, and allow
*    definition of other "custom" attributes.
*
*  Required Files:
*  ---------------
*  Message.h, Message.cpp, Utilities.h, Utilities.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.2 : 27 Mar 2018
*  - added remove method to remove a message attribute, based on its key
*  ver 1.1 : 25 Mar 2018
*  - added method value(Key key) that returns value of attribute with key
*  ver 1.0 : 03 Oct 2017
*  - first release
*
*/
#include "../Utilities/Utilities.h"
#include <string>
#include <unordered_map>
#include <vector>

