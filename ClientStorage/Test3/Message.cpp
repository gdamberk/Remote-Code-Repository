///////////////////////////////////////////////////////////////////////////
// Message.cpp - defines message structure used in communication channel //
// ver 1.2                                                               //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017          //
///////////////////////////////////////////////////////////////////////////

#include "Message.h"
#include <iostream>

using namespace MsgPassingCommunication;
using SUtils = Utilities::StringHelper;

//----< default constructor results in Message with no attributes >----

Message::Message() {}

//----< constructor accepting dst and src addresses >------------------

Message::Message(EndPoint to, EndPoint from)
{
  attributes_["to"] = to.toString();
  attributes_["from"] = from.toString();
}

  SUtils::title("testing assignment");
  Message srcMsg;
  srcMsg.name("srcMsg");
  srcMsg.attribute("foobar", "feebar");
  srcMsg.show();
  std::cout << "\n  assigning srcMsg to msg #1";
  newMsg = srcMsg;
  newMsg.show();

  std::cout << "\n\n";
  return 0;
}
#endif
