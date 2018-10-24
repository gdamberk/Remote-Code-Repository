/////////////////////////////////////////////////////////////////
// Graph.cpp - Demonstrate how to do Graph walks               //
//                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2014   //
/////////////////////////////////////////////////////////////////

#include "Graph.h"

//----< utilities >----------------------------------------------------------

void title(const std::string& t, char underline = '-')
{
  std::cout << "\n  " << t;
  std::cout << "\n " << std::string(t.size() + 2, underline);
}

void putLine(size_t lines = 1)
{
  for (size_t i = 0; i < lines; ++i)
    std::cout << "\n";
}

void putLine_Indent()
{
  std::cout << "\n  ";
}
//----< demonstration >------------------------------------------------------

