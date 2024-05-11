//BEGIN ea_heartbeat.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See ea_heartbeat.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <systemc.h>
#include "ea_heartbeat.h"

void ea_heartbeat::heartbeat_method(void) {
  m_heartbeat.notify(m_period);
}//end ea_heartbeat::heartbeat_method()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_heartbeat.cpp,v 1.1 2004/02/12 15:50:48 dcblack Exp $
