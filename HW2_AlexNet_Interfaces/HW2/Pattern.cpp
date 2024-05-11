#include "Pattern.h"
#include <time.h>
#include <string>
#include <conv_functions.h>
#include <LAYER_PARAM.h>
#include <utils.h>

void Pattern::run()
{
	if (rst.read() == 1)
		return;

	// Declare tensor3dImgi
	std::string file1_dir = "./data/";
	std::string file1_name = "cat.txt";
	std::string file1_path = file1_dir + file1_name;

	std::string file2_dir = "./data/";
	std::string file2_name = "dog.txt";
	std::string file2_path = file2_dir + file2_name;

	// declare a 1d double data type array which stores cat_img
	static double* cat_img = readAndResizeWeights1d(file1_path, INPUT_IMG_CHANNEL * INPUT_IMG_WIDTH * INPUT_IMG_HEIGHT);
	static double* dog_img = readAndResizeWeights1d(file2_path, INPUT_IMG_CHANNEL * INPUT_IMG_WIDTH * INPUT_IMG_HEIGHT);

	// Give start signal, tensor3dImgi
	if (cycle == 4)
	{
		start.write(1);
		tensor3dImgi.write(cat_img);
	}
	else if(cycle == 5)
	{
		start.write(1);
		tensor3dImgi.write(dog_img);
	}
	else
	{
		start.write(0);
		tensor3dImgi.write(dog_img);
	}

	temp_uint = temp_uint.to_uint() + 1;

	cycle++;
	if (cycle == CYCLE)
		sc_stop();
}
