# include <systemc.h>

SC_MODULE(LFSR)
{
    sc_in<sc_logic> clk;
    sc_out<sc_logic> G1;
    sc_out<sc_logic> G2;

    sc_logic D0,D1,D2,D3,D4,D5,D6,D7;
    sc_lv<4> G1_tmp,G2_tmp;

    sc_logic D0_tmp,D1_tmp,D2_tmp,D3_tmp,D4_tmp,D5_tmp,D6_tmp,D7_tmp;


    void run()
    {
        G1_tmp[0] = (sc_logic)D0;
        G1_tmp[1] = (sc_logic)D1;
        G1_tmp[2] = (sc_logic)D2;
        G1_tmp[3] = (sc_logic)D3;

        // Notice feedback signal is needed to store the temp value
        D3_tmp = D3;
        D2_tmp = D2;
        D1_tmp = D1;
        D0_tmp = D0;

        D3 = D2_tmp;
        D2 = D1_tmp;
        D1 = D0_tmp^D3_tmp;
        D0 = D3_tmp;

        G2_tmp[0] = (sc_logic)D4;
        G2_tmp[1] = (sc_logic)D5;
        G2_tmp[2] = (sc_logic)D6;
        G2_tmp[3] = (sc_logic)D7;

        D7_tmp = D7;
        D6_tmp = D6;
        D5_tmp = D5;
        D4_tmp = D4;

        D7 = D6_tmp;
        D6 = D5_tmp ^ D7_tmp;
        D5 = D4_tmp;
        D4 = D7_tmp;

        G1.write(G1_tmp);
        G2.write(G2_tmp);
    }

    SC_CTOR(LFSR)
    {
        D0 = 0; D1 = 1; D2 = 0; D3 = 0;
        D4 = 0; D5 = 1; D6 = 0; D7 = 0;
        SC_METHOD(run);
        sensitive << clk.pos();
    }
};