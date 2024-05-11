//FILE:main.cpp
#include "Hello_SystemC.h"
int sc_main(int argc, char* argv[]) {
    const sc_time t_PERIOD(8,SC_NS);
    sc_clock clk("clk",t_PERIOD);
    Hello_SystemC iHello_SystemC("iHello_SystemC");
    iHello_SystemC.clk_pi(clk);
    sc_start(10,SC_NS);
    return 0;
}
