//FILE:Hello_SystemC.cpp
#include "Hello_SystemC.h"
void Hello_SystemC::main_method(void) {
    std::cout << sc_time_stamp() <<  " Hello world!" << std::endl;
}
