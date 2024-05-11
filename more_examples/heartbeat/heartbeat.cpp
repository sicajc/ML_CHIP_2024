//BEGIN heartbeat.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See heartbeat.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "heartbeat.h"

void heartbeat::heartbeat_method(void) {
  cout << "Received a tick at " << sc_time_stamp()<< "." << endl;
}//end heartbeat_PROCESS()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: heartbeat.cpp,v 1.1 2004/02/12 15:50:48 dcblack Exp $
