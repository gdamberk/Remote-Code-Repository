///////////////////////////////////////////////////////////////////////
// PayLoad.h - application defined payload                           //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2017                             //
// Platform:     Windows 7                                          //
// Application: Software Repository Testbed                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////

#include "PayLoad.h"
#include "../Utilities/StringUtilities/StringUtilities.h"

using namespace NoSqlDb;

#ifdef TEST_PAYLOAD

int main()
{
  Utilities::Title("Demonstrating Application Specific PayLoad class");
  Utilities::putline();


  Utilities::title("creating xml string from payload instance");
  PayLoad pl;
  pl.value("demo payload value");
  pl.categories().push_back("cat1");
  pl.categories().push_back("cat2");
  Utilities::putline();

  Utilities::title("creating payload instance from an XmlElement");
  std::cout << "\n\n";
}
#endif
