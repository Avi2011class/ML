#include <iostream>
#include <fstream>

#include "neural.hpp"
#include "bmp.hpp"
using namespace std;

int core(int argc, char** argv, char** env)
{
	system("rm /media/oldhome/NN/*");
	srand(time(0));

	std::vector< int > LS = {550, 30, 1};
    NeuralNetwork N(2, LS);
	BmpCalc BC("andrey.bmp");

	std::vector< double > data(2);
	std::vector< double > res = {0};
	std::vector< std::pair< double, double > > control = getControlPoints(BC);
	//std::cout << "contr.size = " << control.size() << std::endl;
	//return 0;
	int ro_tmp = -1, ro = 0;
	const int rounds = 15000000;


	for (int i = 0; i < rounds; i++)
	{
		if (i % (rounds / 20) == 0) {
			saveMapBmp("/media/oldhome/NN/" + std::to_string(i / (rounds / 20)) + "map.bmp", N, 400);
		}

		ro = i * 100 / rounds;
		if (ro != ro_tmp) {
			std::cout << std::setprecision(2) << "\rProgress: " << ((double)(ro)) << "%";
			std::cout.flush();
			ro_tmp = ro;
		}

		if (ro % 10 > 5) {
			data[0] = (double)rand() / RAND_MAX;
			data[1] = (double)rand() / RAND_MAX;
		}
		else {
			int ind = rand() % control.size();
			data[0] = control[ind].first + ((double)rand() / RAND_MAX / 25 - 0.02);
			data[1] = control[ind].second + ((double)rand() / RAND_MAX / 25 - 0.02);
			if (data[0] > 1)
				data[0] = 1;
			if (data[0] < 0)
				data[0] = 0;
			if (data[1] > 1)
				data[1] = 1;
			if (data[1] < 0)
				data[1] = 0;
		}


		BC(data[0], data[1], res);
		//res[0] = (data[1] > data[0] * data[0]) ? 1 : 0;
		//*/

		std::vector< double > ans = N(data);

		if (i * 32 < rounds)
			N.backtrace(res, 16);
		else if (i * 6 < rounds)
			N.backtrace(res, 4);
		else if (i * 2 < rounds)
			N.backtrace(res, 2);
		else
			N.backtrace(res, 0.5);
	}
	//testGnuplot(N);

	std::fstream fs("dump", std::ios::out);
	fs << N;
	fs.close();
	//saveMapBmp("map.bmp", N, 500);
	std::cout << std::endl << "Completed! " << std::endl;
	return 0;
}

int main(int argc, char** argv, char**env)
{
	return core(argc, argv, env);
	return 0;
}
