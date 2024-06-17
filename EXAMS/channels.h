#include <systemc.h>

SC_MODULE(Channel)
{
    sc_in<sc_lv<4>> g1_i,g2_i;
    sc_out<sc_lv<4>> a_o,b_o;
    sc_fifo<sc_lv<4>> fifo0,fifo1;

    void run()
    {
        for(;;)
        {
            fifo0.write(g1_i.read());
            fifo1.write(g2_i.read());

            a_o.write(fifo0.read());
            b_o.write(fifo1.read());

            wait();
        }
    }

    SC_CTOR(Channel)
    {
        SC_THREAD(run);
        sensitive << g1_i << g2_i;
    }
};