//BEGIN ea_heartbeat.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See ea_heartbeat.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SC_USE_SC_STRING_OLD
#include <systemc.h>
#include "ea_heartbeat.h"

void ea_heartbeat::heartbeat_method(void) {
  m_heartbeat.notify(m_period);
}//end ea_heartbeat::heartbeat_method()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_heartbeat.cpp,v 1.1 2004/03/03 03:18:36 dcblack Exp $
