#include <systemc.h>
SC_MODULE(LFSR)
{
    sc_in<sc_logic> clk;
    sc_in<sc_logic> data_in;

    sc_out<sc_logic> Q1, Q2, Q3;

    void run()
    {
        Q1 = data_in;
        Q2 = Q1;
        Q3 = Q2;
    }

    SC_CTOR(LFSR)
    {
        SC_METHOD(run);
        sensitive << clk.pos();
    }
};

SC_MODULE(LFSR)
{
    sc_in<sc_logic> clk;
    sc_in<sc_logic> data_in;

    sc_logic ff1, ff2, ff3;
    sc_out<sc_logic> Q1, Q2, Q3;

    void run()
    {
        for (;;)
        {
            Q1.write(ff1);
            Q2.write(ff2);
            Q3.write(ff3);

            ff3 = ff2;
            ff2 = ff1;
            ff1 = data_in.read();
            wait();
        }
    }

    SC_CTOR(LFSR)
    {
        SC_THREAD(run);
        sensitive << clk.pos();
    }
};