#include "systemc.h"
SC_MODULE(full_adder)
{
    sc_in<sc_lv<4>> a_i,b_i,Cin;
    sc_out<sc_logic> Cout;
    sc_out<sc_lv<4>> sum;

    void run()
    {
        sc_uint<4> a = a_i.read().to_uint();
        sc_uint<4> b = b_i.read().to_uint();
        sc_uint<4> c = Cin.read().to_uint();

        sc_unit<5> s = a + b + c;

        Cout.write(s[4]);
        sum.write(s.range(3,0));
    }

    SC_CTOR(full_adder)
    {
        SC_METHOD(run);
        sensitive << a_i << b_i << Cin;
    }
};