#include "Counter.h"

void  Counter::ProcessMethod() {
	if(rst_.read() == true) {
	    v_ = 0;
	} else if (enable_.read()) {
		v_ += 11;
	}
	value_.write(v_);
}
