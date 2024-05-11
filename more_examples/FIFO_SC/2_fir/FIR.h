#include "systemc.h"

class FIR : public sc_module {
public:
    sc_port<sc_fifo_in_if<int>> data_in_;
    sc_port<sc_fifo_out_if<int>> data_out_;
    sc_port<sc_fifo_in_if<int>> orig_in_;

    void FirExecution() {
        while (true) {
            int data = 0;
            if (data_in_->nb_read(data)) {
                data_out_->write(data + 100);
            }
            wait(1, SC_NS);
        }
    }

    void Monitor() {
        for (unsigned i = 0;; i++) {
            int data = 0;
            int result = 0;
            if (orig_in_->nb_read(data) && data_out_->nb_read(result)) {
                printf("[tap %u] %d %d\n", i, data, result);
            }
            wait(1, SC_NS);
        }
    }

    SC_CTOR(FIR) {
        SC_THREAD(FirExecution);
        SC_THREAD(Monitor);
    }
};
