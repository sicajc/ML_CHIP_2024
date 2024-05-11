//BEGIN resp_rtl.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See resp_rtl.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "resp_rtl.h"

void resp_rtl::resp_method(void) {
  bool val = irq_ip->read();
  cout << "INFO: " << name()
       << " Port irq_ip is " << val
       << " Received event at " << sc_time_stamp() << endl;
}//end resp_rtl::resp_method

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: resp_rtl.cpp,v 1.2 2004/03/14 01:28:58 dcblack Exp $
