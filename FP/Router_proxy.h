#ifndef _HPG_Router_proxy_h_H_
#define _HPG_Router_proxy_h_H_

#include "systemc.h"

SC_HDL_MODULE( Router) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<sc_lv<34> > out_flit_N;
    sc_out<sc_lv<34> > out_flit_E;
    sc_out<sc_lv<34> > out_flit_S;
    sc_out<sc_lv<34> > out_flit_W;
    sc_out<sc_lv<34> > out_flit_Core;
    sc_out<bool> out_req_N;
    sc_out<bool> out_req_E;
    sc_out<bool> out_req_S;
    sc_out<bool> out_req_W;
    sc_out<bool> out_req_Core;
    sc_in<bool> in_ack_N;
    sc_in<bool> in_ack_E;
    sc_in<bool> in_ack_S;
    sc_in<bool> in_ack_W;
    sc_in<bool> in_ack_Core;
    sc_in<sc_lv<34> > in_flit_N;
    sc_in<sc_lv<34> > in_flit_E;
    sc_in<sc_lv<34> > in_flit_S;
    sc_in<sc_lv<34> > in_flit_W;
    sc_in<sc_lv<34> > in_flit_Core;
    sc_in<bool> in_req_N;
    sc_in<bool> in_req_E;
    sc_in<bool> in_req_S;
    sc_in<bool> in_req_W;
    sc_in<bool> in_req_Core;
    sc_out<bool> out_ack_N;
    sc_out<bool> out_ack_E;
    sc_out<bool> out_ack_S;
    sc_out<bool> out_ack_W;
    sc_out<bool> out_ack_Core;

    std::string hpg_log_lib;
    std::string hpg_module_name;
    std::string hpg_hdl_src_path;

    std::string libraryName() { return hpg_log_lib; }

    std::string moduleName() { return hpg_module_name; }

    cwr_hdlLangType hdl_language_type() { return sc_hdl_module::cwr_verilog; }

    void getVerilogSourceFiles(std::vector<std::string>& verilog_files) {
        verilog_files.push_back(hpg_hdl_src_path + std::string("/home/2024MLChip/mlchip040/FP_3X3_VERSION/router.v"));
    }

    Router(sc_module_name name, int id = 0, const char* hdlSrcPath="") : 
        sc_hdl_module(name), hpg_log_lib("CWR_HDL_WORK"), hpg_module_name("Router"), hpg_hdl_src_path()
        , clk("clk"), rst("rst"), out_flit_N("out_flit_N"), out_flit_E("out_flit_E")
        , out_flit_S("out_flit_S"), out_flit_W("out_flit_W"), out_flit_Core("out_flit_Core")
        , out_req_N("out_req_N"), out_req_E("out_req_E"), out_req_S("out_req_S")
        , out_req_W("out_req_W"), out_req_Core("out_req_Core"), in_ack_N("in_ack_N")
        , in_ack_E("in_ack_E"), in_ack_S("in_ack_S"), in_ack_W("in_ack_W"), in_ack_Core("in_ack_Core")
        , in_flit_N("in_flit_N"), in_flit_E("in_flit_E"), in_flit_S("in_flit_S")
        , in_flit_W("in_flit_W"), in_flit_Core("in_flit_Core"), in_req_N("in_req_N")
        , in_req_E("in_req_E"), in_req_S("in_req_S"), in_req_W("in_req_W"), in_req_Core("in_req_Core")
        , out_ack_N("out_ack_N"), out_ack_E("out_ack_E"), out_ack_S("out_ack_S")
        , out_ack_W("out_ack_W"), out_ack_Core("out_ack_Core") {

        if (hdlSrcPath != 0 && strlen(hdlSrcPath) != 0) {
          hpg_hdl_src_path = std::string(hdlSrcPath) + "/";
        }

        ncwr_set_hdl_param("id", id);

    }
};

#endif
