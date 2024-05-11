#include "Monitor.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

void Monitor::run()
{

	if (rst.read() == 1)
	{
		cout << "reset" << endl;
		// read imagenet_classes.txt
		ifstream file("data/imagenet_classes.txt");
		string class_name;

		while (getline(file, class_name))
		{
			imagenet_classes.push_back(class_name);
		}
	}

	else if (cycle != 0)
	{
		cout << "------------------------------------------------------------" << endl;
		cout << "cycle: " << cycle << endl;
		cout << "in_valid: \t" << in_valid << endl;
		cout << "conv1_valid: \t" << conv1_valid << "\t" << "conv1_valid: \t" << conv1_result << endl;
		cout << "mp1_valid: \t" << mp1_valid << "\t" << "mp1_result[0,0,0]: \t" << mp1_result << endl;
		cout << "conv2_valid: \t" << conv2_valid << "\t" << "conv2_result[0,0,0]: \t" << conv2_result << endl;
		cout << "mp2_valid: \t" << mp2_valid << "\t" << "mp2_result[0,0,0]: \t" << mp2_result << endl;
		cout << "conv3_valid: \t" << conv3_valid << "\t" << "conv3_result[0,0,0]: \t" << conv3_result << endl;
		cout << "conv4_valid: \t" << conv4_valid << "\t" << "conv4_result[0,0,0]: \t" << conv4_result << endl;
		cout << "conv5_valid: \t" << conv5_valid << "\t" << "conv5_result[0,0,0]: \t" << conv5_result << endl;
		cout << "mp3_valid: \t" << mp3_valid << "\t" << "mp3_result[0,0,0]: \t" << mp3_result << endl;
		cout << "linear1_valid: \t" << linear1_valid << "\t" << "linear1_result[0,0,0]: \t" << linear1_result << endl;
		cout << "linear2_valid: \t" << linear2_valid << "\t" << "linear2_result[0,0,0]: \t" << linear2_result << endl;
		cout << "linear3_valid: \t" << linear3_valid << "\t" << "linear3_result[0,0,0]: \t" << linear3_result[0] << endl;
		cout << "------------------------------------------------------------" << endl;
	}

	// SoftMax
	if (linear3_valid.read() == 1)
	{
		cout << "SoftMax: " << endl;
		double sum = 0;
		vector<double> softmax_result(1000);
		vector<pair<int, double>> softmax_result_pair(1000);

		for (int i = 0; i < 1000; i++)
		{
			sum += exp(linear3_result[i].read());
		}
		for (int i = 0; i < 1000; i++)
		{
			softmax_result[i] = exp(linear3_result[i].read()) / sum;
			softmax_result_pair[i] = make_pair(i, softmax_result[i]);
		}

		sort(softmax_result_pair.begin(), softmax_result_pair.end(), [](const pair<int, double> &left, const pair<int, double> &right)
			 { return left.second > right.second; });

		cout << " Top \t idx \t val \t        possibility \t class name" << endl;
		cout << "------------------------------------------------------------" << endl;
		for (int i = 0; i < 5; i++)
		{
			cout << fixed << setprecision(6) << "  " << i + 1 << "\t " << softmax_result_pair[i].first << "\t" << (double)(linear3_result[softmax_result_pair[i].first].read()) << "\t" << 100 * softmax_result_pair[i].second << "\t" << imagenet_classes[softmax_result_pair[i].first] << endl;
			// cout << "[Top " << i+1 << "] idx: " << softmax_result_pair[i].first << "\t val: " << linear3_result[softmax_result_pair[i].first] << "\t possibility: " << 100*softmax_result_pair[i].second << "\t class name: " << imagenet_classes[softmax_result_pair[i].first] << endl;
		}
	}

	cycle++;
}
