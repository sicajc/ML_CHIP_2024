#ifndef RESP_RTL_H
#define RESP_RTL_H
//BEGIN resp_rtl.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See interrupt.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>

SC_MODULE(resp_rtl) {
  sc_in<bool> irq_ip;
  SC_CTOR(resp_rtl)
  {
    SC_METHOD(resp_method);
    sensitive << irq_ip;
    dont_initialize();
  }
  void resp_method(void);
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: resp_rtl.h,v 1.2 2004/04/15 17:36:53 dcblack Exp $
