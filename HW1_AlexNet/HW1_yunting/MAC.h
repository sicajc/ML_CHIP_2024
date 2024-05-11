#define SC_INCLUDE_FX
#include <systemc.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

//--------------------------------------------------------
// CONV_RELU_1
// - in_feature_map: 150528 (3, 224, 224)
// - out_feature_map: 193600 (64, 55, 55)
//--------------------------------------------------------

SC_MODULE(CONV_RELU_1)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 150528};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 193600};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight1{"weight1", 23232};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias1{"bias1", 64};
	vector<double> weight1;
	vector<double> bias1;
	bool ready;

	void run()
	{
		const int IN_CHANNELS = 3;
		const int IN_HEIGHT = 224;
		const int IN_WIDTH = 224;
		const int KERNAL_SIZE = 11;
		const int OUT_CHANNELS = 64;
		const int OUT_HEIGHT = 55;
		const int OUT_WIDTH = 55;
		const int STRIDE = 4;
		const int PADDING = 2;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			ready = 1;
			std::ifstream bfile("data/conv1_bias.txt");
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				// bias1[bcnt].write((sc_fixed_fast<40,17>)(b_value)); //.push_back(b_value)
				bias1.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/conv1_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				// weight1[wcnt].write((sc_fixed_fast<40,17>)(w_value)); //.push_back(w_value)
				weight1.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();
			// cout<<"bias.size: "<<bias1.size()<<" bcnt: "<<bcnt<<endl;
			//			for(int i=0; i<bias1.size(); i++){
			//				cout<<"bias1["<<i<<"]: "<<bias1[i]<<endl;
			//			}
			// cout<<"weight1.size: "<<weight1[weight1.size()-1]<<" wcnt: "<<wcnt<<endl;
			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// conv1
		if (in_valid.read())
		{

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			// sc_fixed_fast<40,17> partial_sum; //double
			double partial_sum;
			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						partial_sum = 0;

						for (int ic = 0; ic < IN_CHANNELS; ic++)
						{
							for (int krow = 0; krow < KERNAL_SIZE; krow++)
							{
								for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
								{

									//[ic, wrow, wcol] -> [ic, stride*orow + krow -2, stride*ocol + kcol -2]
									i_ptr_x = STRIDE * ocol + kcol - PADDING;
									i_ptr_y = STRIDE * orow + krow - PADDING;
									i_ptr = ic * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
									w_ptr = oc * IN_CHANNELS * KERNAL_SIZE * KERNAL_SIZE + ic * KERNAL_SIZE * KERNAL_SIZE + krow * KERNAL_SIZE + kcol; //[oc, ic, krow, kcol]]

									if (!(i_ptr_x < 0 || i_ptr_x >= IN_WIDTH || i_ptr_y < 0 || i_ptr_y >= IN_HEIGHT))
									{
										partial_sum += (in_feature_map[i_ptr] * weight1[w_ptr]);
									}
								}
							}
						}

						if ((partial_sum + bias1[oc]) > 0)
						{
							partial_sum += bias1[oc];
							// out_feature_map[oc*OUT_HEIGHT*OUT_WIDTH + orow*OUT_WIDTH + ocol] = partial_sum + bias1[oc]; //(sc_fixed_fast<40,17>)(partial_sum);
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = (sc_fixed_fast<40, 17>)partial_sum;
						}
						else
						{
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = 0;
						}
					}
				}
			}
			out_valid.write(1);
		}
		// std::cout<<"conv1_result[0,0,0]"<<out_feature_map[0]<<std::endl;
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(CONV_RELU_1)
	{
		ready = 0;
		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// MAX_POOLING_1
// - in_feature_map: 193600 (64, 55, 55)
// - out_feature_map: 46656 (64, 27, 27)
//--------------------------------------------------------

SC_MODULE(MAX_POOLING_1)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 193600};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 46656};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	void run()
	{
		const int IN_CHANNELS = 64;
		const int IN_HEIGHT = 55;
		const int IN_WIDTH = 55;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 64;
		const int OUT_HEIGHT = 27;
		const int OUT_WIDTH = 27;
		const int STRIDE = 2;
		// const int PADDING = 2;
		out_valid.write(0);

		if (rst.read() == 1)
		{

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// maxpool1
		if (in_valid.read())
		{

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			sc_fixed_fast<40, 17> largest;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						largest = 0;
						for (int krow = 0; krow < KERNAL_SIZE; krow++)
						{
							for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
							{
								//[ic, wrow, wcol] -> [ic, stride*orow + krow, stride*ocol + kcol]
								i_ptr_x = STRIDE * ocol + kcol;
								i_ptr_y = STRIDE * orow + krow;
								i_ptr = oc * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
								if (in_feature_map[i_ptr] > largest)
								{
									largest = in_feature_map[i_ptr];
								}
							}
						}
						out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = largest;
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"mp1_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(MAX_POOLING_1)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// CONV_RELU_2
// - in_feature_map: 46656 (64, 27, 27)
// - out_feature_map: 139968 (192, 27, 27)
//--------------------------------------------------------

SC_MODULE(CONV_RELU_2)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 46656};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 139968};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight2{"weight2", 307200};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias2{"bias2", 192};
	vector<double> weight2;
	vector<double> bias2;

	// bool ready;

	void run()
	{
		const int IN_CHANNELS = 64;
		const int IN_HEIGHT = 27;
		const int IN_WIDTH = 27;
		const int KERNAL_SIZE = 5;
		const int OUT_CHANNELS = 192;
		const int OUT_HEIGHT = 27;
		const int OUT_WIDTH = 27;
		const int STRIDE = 1;
		const int PADDING = 2;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			// ready = 1;
			std::ifstream bfile("data/conv2_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias2 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias2.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/conv2_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight2.push_back(w_value);
				wcnt++;
			}
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight_"<<i<<": "<<weight1[i]<<std::endl;
			// }

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// conv2
		if (in_valid.read())
		{
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight2_"<<i<<": "<<weight2[i]<<std::endl;
			// }

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						partial_sum = 0;
						for (int ic = 0; ic < IN_CHANNELS; ic++)
						{
							for (int krow = 0; krow < KERNAL_SIZE; krow++)
							{
								for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
								{

									//[ic, wrow, wcol] -> [ic, stride*orow + krow -2, stride*ocol + kcol -2]
									i_ptr_x = STRIDE * ocol + kcol - PADDING;
									i_ptr_y = STRIDE * orow + krow - PADDING;
									i_ptr = ic * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
									w_ptr = oc * IN_CHANNELS * KERNAL_SIZE * KERNAL_SIZE + ic * KERNAL_SIZE * KERNAL_SIZE + krow * KERNAL_SIZE + kcol; //[oc, ic, krow, kcol]]

									if (!(i_ptr_x < 0 || i_ptr_x >= IN_WIDTH || i_ptr_y < 0 || i_ptr_y >= IN_HEIGHT))
									{
										partial_sum += (in_feature_map[i_ptr] * weight2[w_ptr]);
									}
								}
							}
						}

						if ((partial_sum + bias2[oc]) > 0)
						{
							partial_sum += bias2[oc];
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = (sc_fixed_fast<40, 17>)(partial_sum);
						}
						else
						{
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = 0;
						}
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"conv2_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(CONV_RELU_2)
	{
		// ready = 0;
		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// MAX_POOLING_2
// - in_feature_map: 139968 (192, 27, 27)
// - out_feature_map: 32448 (192, 13, 13)
//--------------------------------------------------------

SC_MODULE(MAX_POOLING_2)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 139968};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 32448};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	void run()
	{
		const int IN_CHANNELS = 192;
		const int IN_HEIGHT = 27;
		const int IN_WIDTH = 27;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 192;
		const int OUT_HEIGHT = 13;
		const int OUT_WIDTH = 13;
		const int STRIDE = 2;
		// const int PADDING = 2;
		out_valid.write(0);

		if (rst.read() == 1)
		{

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// maxpool1
		if (in_valid.read())
		{

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			sc_fixed_fast<40, 17> largest;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						largest = 0;
						for (int krow = 0; krow < KERNAL_SIZE; krow++)
						{
							for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
							{
								//[ic, wrow, wcol] -> [ic, stride*orow + krow, stride*ocol + kcol]
								i_ptr_x = STRIDE * ocol + kcol;
								i_ptr_y = STRIDE * orow + krow;
								i_ptr = oc * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
								if (in_feature_map[i_ptr] > largest)
								{
									largest = in_feature_map[i_ptr];
								}
							}
						}
						out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = largest;
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"mp2_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(MAX_POOLING_2)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// CONV_RELU_3
// - in_feature_map: 32448 (192, 13, 13)
// - out_feature_map: 64896 (384, 13, 13)
//--------------------------------------------------------

SC_MODULE(CONV_RELU_3)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 32448};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 64896};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight3{"weight3", 663552};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias3{"bias3", 384};
	vector<double> weight3;
	vector<double> bias3;

	void run()
	{
		const int IN_CHANNELS = 192;
		const int IN_HEIGHT = 13;
		const int IN_WIDTH = 13;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 384;
		const int OUT_HEIGHT = 13;
		const int OUT_WIDTH = 13;
		const int STRIDE = 1;
		const int PADDING = 1;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/conv3_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias3 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias3.push_back(b_value);
				// std::cout << "bias1[" << bcnt << "]: " << b_value << std::endl;
				bcnt++;
			}

			std::ifstream wfile("data/conv3_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight3.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// conv3
		if (in_valid.read())
		{
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight3_"<<i<<": "<<weight3[i]<<std::endl;
			// }

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						partial_sum = 0;
						for (int ic = 0; ic < IN_CHANNELS; ic++)
						{
							for (int krow = 0; krow < KERNAL_SIZE; krow++)
							{
								for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
								{

									//[ic, wrow, wcol] -> [ic, stride*orow + krow -2, stride*ocol + kcol -2]
									i_ptr_x = STRIDE * ocol + kcol - PADDING;
									i_ptr_y = STRIDE * orow + krow - PADDING;
									i_ptr = ic * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
									w_ptr = oc * IN_CHANNELS * KERNAL_SIZE * KERNAL_SIZE + ic * KERNAL_SIZE * KERNAL_SIZE + krow * KERNAL_SIZE + kcol; //[oc, ic, krow, kcol]]

									if (!(i_ptr_x < 0 || i_ptr_x >= IN_WIDTH || i_ptr_y < 0 || i_ptr_y >= IN_HEIGHT))
									{
										partial_sum += (in_feature_map[i_ptr] * weight3[w_ptr]);
									}
								}
							}
						}

						if ((partial_sum + bias3[oc]) > 0)
						{
							partial_sum += bias3[oc];
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = (sc_fixed_fast<40, 17>)(partial_sum);
						}
						else
						{
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = 0;
						}
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"conv3_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(CONV_RELU_3)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// CONV_RELU_4
// - in_feature_map: 64896 (384, 13, 13)
// - out_feature_map: 43264 (256, 13, 13)
//--------------------------------------------------------

SC_MODULE(CONV_RELU_4)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 64896};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 43264};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight4{"weight4", 884736};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias4{"bias4", 256};
	vector<double> weight4;
	vector<double> bias4;

	void run()
	{
		const int IN_CHANNELS = 384;
		const int IN_HEIGHT = 13;
		const int IN_WIDTH = 13;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 256;
		const int OUT_HEIGHT = 13;
		const int OUT_WIDTH = 13;
		const int STRIDE = 1;
		const int PADDING = 1;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/conv4_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias4 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias4.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/conv4_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight4.push_back(w_value);
				if (wcnt < 64)
				{
					// std::cout<<"w_value: "<<w_value<<std::endl;
				}
				wcnt++;
			}
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight_"<<i<<": "<<weight1[i]<<std::endl;
			// }

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// conv3
		if (in_valid.read())
		{
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight3_"<<i<<": "<<weight3[i]<<std::endl;
			// }

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						partial_sum = 0;
						for (int ic = 0; ic < IN_CHANNELS; ic++)
						{
							for (int krow = 0; krow < KERNAL_SIZE; krow++)
							{
								for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
								{

									//[ic, wrow, wcol] -> [ic, stride*orow + krow -2, stride*ocol + kcol -2]
									i_ptr_x = STRIDE * ocol + kcol - PADDING;
									i_ptr_y = STRIDE * orow + krow - PADDING;
									i_ptr = ic * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
									w_ptr = oc * IN_CHANNELS * KERNAL_SIZE * KERNAL_SIZE + ic * KERNAL_SIZE * KERNAL_SIZE + krow * KERNAL_SIZE + kcol; //[oc, ic, krow, kcol]]

									if (!(i_ptr_x < 0 || i_ptr_x >= IN_WIDTH || i_ptr_y < 0 || i_ptr_y >= IN_HEIGHT))
									{
										partial_sum += (in_feature_map[i_ptr] * weight4[w_ptr]);
									}
								}
							}
						}

						if ((partial_sum + bias4[oc]) > 0)
						{
							partial_sum += bias4[oc];
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = (sc_fixed_fast<40, 17>)(partial_sum);
						}
						else
						{
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = 0;
						}
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"conv4_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(CONV_RELU_4)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// CONV_RELU_5
// - in_feature_map: 43264 (256, 13, 13)
// - out_feature_map: 43264 (256, 13, 13)
//--------------------------------------------------------

SC_MODULE(CONV_RELU_5)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 43264};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 43264};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight5{"weight5", 589824};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias5{"bias5", 256};
	vector<double> weight5;
	vector<double> bias5;

	void run()
	{
		const int IN_CHANNELS = 256;
		const int IN_HEIGHT = 13;
		const int IN_WIDTH = 13;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 256;
		const int OUT_HEIGHT = 13;
		const int OUT_WIDTH = 13;
		const int STRIDE = 1;
		const int PADDING = 1;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/conv5_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias5 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias5.push_back(b_value);
				// std::cout << "bias1[" << bcnt << "]: " << b_value << std::endl;
				bcnt++;
			}

			std::ifstream wfile("data/conv5_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight5.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// conv3
		if (in_valid.read())
		{
			// for(int i=0; i<64; i++){
			//	std::cout<<"weight3_"<<i<<": "<<weight3[i]<<std::endl;
			// }

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						partial_sum = 0;
						for (int ic = 0; ic < IN_CHANNELS; ic++)
						{
							for (int krow = 0; krow < KERNAL_SIZE; krow++)
							{
								for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
								{

									//[ic, wrow, wcol] -> [ic, stride*orow + krow -2, stride*ocol + kcol -2]
									i_ptr_x = STRIDE * ocol + kcol - PADDING;
									i_ptr_y = STRIDE * orow + krow - PADDING;
									i_ptr = ic * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
									w_ptr = oc * IN_CHANNELS * KERNAL_SIZE * KERNAL_SIZE + ic * KERNAL_SIZE * KERNAL_SIZE + krow * KERNAL_SIZE + kcol; //[oc, ic, krow, kcol]]

									if (!(i_ptr_x < 0 || i_ptr_x >= IN_WIDTH || i_ptr_y < 0 || i_ptr_y >= IN_HEIGHT))
									{
										partial_sum += (in_feature_map[i_ptr] * weight5[w_ptr]);
									}
								}
							}
						}

						if ((partial_sum + bias5[oc]) > 0)
						{
							partial_sum += bias5[oc];
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = (sc_fixed_fast<40, 17>)(partial_sum);
						}
						else
						{
							out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = 0;
						}
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"conv5_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(CONV_RELU_5)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// MAX_POOLING_3
// - in_feature_map: 43264 (256, 13, 13)
// - out_feature_map: 9216 (256, 6, 6)
//--------------------------------------------------------

SC_MODULE(MAX_POOLING_3)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 43264};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 9216};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	void run()
	{
		const int IN_CHANNELS = 256;
		const int IN_HEIGHT = 13;
		const int IN_WIDTH = 13;
		const int KERNAL_SIZE = 3;
		const int OUT_CHANNELS = 256;
		const int OUT_HEIGHT = 6;
		const int OUT_WIDTH = 6;
		const int STRIDE = 2;
		// const int PADDING = 2;
		out_valid.write(0);

		if (rst.read() == 1)
		{

			for (int i = 0; i < OUT_CHANNELS * OUT_HEIGHT * OUT_WIDTH; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// maxpool1
		if (in_valid.read())
		{

			int i_ptr, w_ptr;
			int i_ptr_x, i_ptr_y;
			sc_fixed_fast<40, 17> largest;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				for (int orow = 0; orow < OUT_HEIGHT; orow++)
				{
					for (int ocol = 0; ocol < OUT_WIDTH; ocol++)
					{
						largest = 0;
						for (int krow = 0; krow < KERNAL_SIZE; krow++)
						{
							for (int kcol = 0; kcol < KERNAL_SIZE; kcol++)
							{
								//[ic, wrow, wcol] -> [ic, stride*orow + krow, stride*ocol + kcol]
								i_ptr_x = STRIDE * ocol + kcol;
								i_ptr_y = STRIDE * orow + krow;
								i_ptr = oc * IN_HEIGHT * IN_WIDTH + i_ptr_y * IN_WIDTH + i_ptr_x;
								if (in_feature_map[i_ptr] > largest)
								{
									largest = in_feature_map[i_ptr];
								}
							}
						}
						out_feature_map[oc * OUT_HEIGHT * OUT_WIDTH + orow * OUT_WIDTH + ocol] = largest;
					}
				}
			}
			out_valid.write(1);
			// std::cout<<"mp3_result[0,0,0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(MAX_POOLING_3)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// LINEAR_RELU_1
// - in_feature_map: 9216
// - out_feature_map: 4096
//--------------------------------------------------------

SC_MODULE(LINEAR_RELU_1)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 9216};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 4096};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> weight6{"weight6", 37748736};
	// sc_vector<sc_signal<sc_fixed_fast<40,17>>> bias6{"bias6", 4096};
	vector<double> weight6;
	vector<double> bias6;

	void run()
	{
		const int IN_CHANNELS = 9216;
		const int OUT_CHANNELS = 4096;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/fc6_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias6 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias6.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/fc6_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight6.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// fc6 + RELU
		if (in_valid.read())
		{
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				partial_sum = 0;
				for (int ic = 0; ic < IN_CHANNELS; ic++)
				{
					partial_sum += in_feature_map[ic] * weight6[oc * IN_CHANNELS + ic];
				}
				// RELU
				if ((partial_sum + bias6[oc]) > 0)
				{
					partial_sum += bias6[oc];
					out_feature_map[oc] = (sc_fixed_fast<40, 17>)(partial_sum);
				}
				else
				{
					out_feature_map[oc] = 0;
				}
			}
			out_valid.write(1);
			// std::cout<<"fc6_result[0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(LINEAR_RELU_1)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// LINEAR_RELU_2
// - in_feature_map: 4096
// - out_feature_map: 4096
//--------------------------------------------------------

SC_MODULE(LINEAR_RELU_2)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 4096};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 4096};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	vector<double> weight7;
	vector<double> bias7;

	void run()
	{
		const int IN_CHANNELS = 4096;
		const int OUT_CHANNELS = 4096;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/fc7_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias7 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias7.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/fc7_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight7.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// fc6 + RELU
		if (in_valid.read())
		{
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				partial_sum = 0;
				for (int ic = 0; ic < IN_CHANNELS; ic++)
				{
					partial_sum += in_feature_map[ic] * weight7[oc * IN_CHANNELS + ic];
				}
				// RELU
				if ((partial_sum + bias7[oc]) > 0)
				{
					partial_sum += bias7[oc];
					out_feature_map[oc] = (sc_fixed_fast<40, 17>)(partial_sum);
				}
				else
				{
					out_feature_map[oc] = 0;
				}
			}
			out_valid.write(1);
			// std::cout<<"fc7_result[0]"<<out_feature_map[0]<<std::endl;
		}
	}
	// ^^^^^ put your code here ^^^^^

	SC_CTOR(LINEAR_RELU_2)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};

//--------------------------------------------------------
// LINEAR_3
// - in_feature_map: 4096
// - out_feature_map: 1000
//--------------------------------------------------------

SC_MODULE(LINEAR_3)
{

	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_vector<sc_in<sc_fixed_fast<40, 17>>> in_feature_map{"in_feature_map", 4096};
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> out_feature_map{"out_feature_map", 1000};
	sc_in<bool> in_valid;
	sc_out<bool> out_valid;

	vector<double> weight8;
	vector<double> bias8;

	void run()
	{
		const int IN_CHANNELS = 4096;
		const int OUT_CHANNELS = 1000;

		out_valid.write(0);

		// read weight and bias
		if (rst.read() == 1)
		{
			std::ifstream bfile("data/fc8_bias.txt");
			if (!bfile.is_open())
			{
				std::cerr << "Unable to open bias8 file!" << std::endl;
				return;
			}
			double b_value;
			int bcnt = 0;
			while (bfile >> b_value)
			{
				bias8.push_back(b_value);
				bcnt++;
			}

			std::ifstream wfile("data/fc8_weight.txt");
			double w_value;
			int wcnt = 0;
			while (wfile >> w_value)
			{
				weight8.push_back(w_value);
				wcnt++;
			}

			wfile.close();
			bfile.close();

			for (int i = 0; i < OUT_CHANNELS; i++)
			{
				out_feature_map[i].write(0.0);
			}
		}
		// fc6 + RELU
		if (in_valid.read())
		{
			double partial_sum;

			for (int oc = 0; oc < OUT_CHANNELS; oc++)
			{
				partial_sum = 0;
				for (int ic = 0; ic < IN_CHANNELS; ic++)
				{
					partial_sum += in_feature_map[ic] * weight8[oc * IN_CHANNELS + ic];
				}
				partial_sum += bias8[oc];
				out_feature_map[oc] = (sc_fixed_fast<40, 17>)(partial_sum);
			}
			out_valid.write(1);
			// std::cout<<"fc7_result[0]"<<out_feature_map[0]<<std::endl;
		}
	}

	// ^^^^^ put your code here ^^^^^

	SC_CTOR(LINEAR_3)
	{

		SC_METHOD(run);

		sensitive << clk.pos();
	}
};
