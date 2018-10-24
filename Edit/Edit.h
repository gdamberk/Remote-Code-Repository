#pragma once
///////////////////////////////////////////////////////////////////////
// Edit.h - Edit metadata values                                     //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2017                             //
// Platform:     Windows 7                                          //
// Application: Software Repository Testbed							//
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines a single Edit class that:
*  - accepts a DbElement<P> instance when constructed
*  - provides facilities to edit it's metadata
*
*  Public Interface
*  ----------------
*   static void identify(std::ostream& = std::cout) - show file name
*   void name(const std::string& name) - set name
*   void description(const std::string& descrip) - set description
*   DateTime& dateTime() - set date time
*   void dateTime(const DateTime& dt) - edit date time
*   void clearChildKeys() - clear child key
*   bool addChildKey(const Key& key) - add child key
*   bool removeChildKey(const Key& key) - remove child key
*
*  Required Files:
*  ---------------
*  Edit.h, Edit.cpp
*  DbCore.h, DbCore.cpp
*  Query.h, Query.cpp
*  DateTime.h, DateTime.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 17 Feb 2018
*  - first release
*/

#include "../DbCore/DbCore.h"
#include "../Query/Query.h"
#include "../DateTime/DateTime.h"
#include <string>
#include <iostream>

namespace NoSqlDb
{
  template<typename P>
  class Edit
  {
  public:
    Edit(DbElement<P>& elem) : elem_(elem) {}
    static void identify(std::ostream& = std::cout);
    void name(const std::string& name);
    void description(const std::string& descrip);
    DateTime& dateTime();
    void dateTime(const DateTime& dt);
    void clearChildKeys();
    bool addChildKey(const Key& key);
    bool removeChildKey(const Key& key);
    P& payLoad();
    void payLoad(const P& p);
    DbElement<P> DbElement() { return elem_; }
  private:
    NoSqlDb::DbElement<P>& elem_;
  };

  //----< show file name >---------------------------------------------

  template<typename P>
  void Edit<P>::identify(std::ostream& out)
  {
    out << "\n  \"" << __FILE__ << "\"";
  }
  //----< set name >---------------------------------------------
  template<typename P>
  void Edit<P>::name(const std::string& name)
  {
    elem_.name(name);
  }
  //----< set description >---------------------------------------------
  template<typename P>
  void Edit<P>::description(const std::string& descrip)
  {
    elem_.descrip(descrip);
  }
  //----< set date time >---------------------------------------------
  template<typename P>
  DateTime& Edit<P>::dateTime()
  {
    return elem_.dateTime();
  }
  //----< edit date time >---------------------------------------------
  template<typename P>
  void Edit<P>::dateTime(const DateTime& dt)
  {
    elem_.dateTime(dt);
  }
  //----< clear child key  >---------------------------------------------
  template<typename P>
  void Edit<P>::clearChildKeys()
  {
    elem_.children().clear();
  }
  //----< add child key  >---------------------------------------------
  template<typename P>
  bool Edit<P>::addChildKey(const Key& key)
  {
    return elem_.addChildKey(key);
  }
  //----< remove child key  >---------------------------------------------
  template<typename P>
  bool Edit<P>::removeChildKey(const Key& key)
  {
    return elem_.removeChildKey(key);
  }
  //----< edit payload  >---------------------------------------------
  template<typename P>
  P& Edit<P>::payLoad()
  {
    return elem_.payLoad();
  }
  //----< edit payload  >---------------------------------------------
  template<typename P>
  void Edit<P>::payLoad(const P& p)
  {
    elem_.payLoad(p);
  }
}