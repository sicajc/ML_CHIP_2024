#include <systemc.h>

SC_MODULE(Buffer)
{
    sc_in<sc_lv<4>> in;
    sc_out<sc_lv<4>> out;

    void run()
    {
        out.write(in.read());
    }

    SC_CTOR(Buffer)
    {
        SC_METHOD(run);
        sensitive << clk.pos();
    }
};