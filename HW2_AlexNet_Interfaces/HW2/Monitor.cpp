#include "Monitor.h"
#include "LAYER_PARAM.h"
#include "utils.h"

void Monitor::run()
{
	for (;;)
	{
		wait();
		if (rst.read() == 1)
		{
			cout << "Monitor reset" << endl;
		}
		else if (cycle != 0)
		{
			// Give me case statement cycle for display info
			cout<<"\033[1;32m";
			cout<<"==================================================================="<<endl;
			//change the color of cycle display on linux window to green
			cout << "Cycle: " << cycle << "			   " << "Start signal: " << start << endl;
			cout<<"==================================================================="<<endl;
			//change it back to default color
			cout<<"\033[0m";

			switch (cycle)
			{
			case 1:
				cout << "Start signal is given" << endl;
				break;
			case 4:
				cout << "Cat image is given" << endl;
				display1DTensor(tensor3dImgi.read(), 10);
				break;
			case 5:
				cout << "Dog image is given" << endl;
				display1DTensor(tensor3dImgi.read(), 10);
				break;
			default:
				cout << "Running" << endl;
				break;
			}

			if(valid_o.read() == 1)
			{
				cout << "Softmax result" << endl;
				display1DTensor(fully_connected_result.read(), 10);
			}
		}
		cycle++;
	}
}
