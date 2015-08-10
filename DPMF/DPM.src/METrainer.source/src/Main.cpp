#include <iostream>
#include <cmath>
#include "Trainer.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cerr << "Usage: train_file feat_file" << endl;
		exit(1);
	}

	CTrainer trainer;

	trainer.Train(argv[1], argv[2]);
	return 0;
}
