#define SC_INCLUDE_FX
#include <systemc.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

SC_MODULE( Monitor ) {	

	sc_in < bool > rst;
	sc_in_clk clock;
	sc_in < bool > in_valid;
	sc_in < bool > conv1_valid;
	sc_in < sc_fixed_fast<40,17> > conv1_result;
	sc_in < bool > mp1_valid;
	sc_in < sc_fixed_fast<40,17> > mp1_result;
	sc_in < bool > conv2_valid;
	sc_in < sc_fixed_fast<40,17> > conv2_result;
	sc_in < bool > mp2_valid;
	sc_in < sc_fixed_fast<40,17> > mp2_result;
	sc_in < bool > conv3_valid;
	sc_in < sc_fixed_fast<40,17> > conv3_result;
	sc_in < bool > conv4_valid;
	sc_in < sc_fixed_fast<40,17> > conv4_result;
	sc_in < bool > conv5_valid;
	sc_in < sc_fixed_fast<40,17> > conv5_result;
	sc_in < bool > mp3_valid;
	sc_in < sc_fixed_fast<40,17> > mp3_result;
	sc_in < bool > linear1_valid;
	sc_in < sc_fixed_fast<40,17> > linear1_result;
	sc_in < bool > linear2_valid;
	sc_in < sc_fixed_fast<40,17> > linear2_result;
	sc_in < bool > linear3_valid;
	sc_vector < sc_in < sc_fixed_fast<40,17> > > linear3_result{"linear3_result", 1000};

	vector<string> imagenet_classes;

	//sc_in < sc_fixed_fast<40,17> > linear3_result;
	int cycle;
	
	void run();
	
	SC_CTOR( Monitor )
	{	
		cycle = 0;
	
		SC_METHOD( run );
		//sensitive << clock.neg();
		sensitive << clock.pos();
	}
};

