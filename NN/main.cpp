#include <iostream>
#include <fstream>

#include "neural.hpp"

using namespace std;

int main()
{
	srand(time(0));
	std::vector< int > LS = {3, 1};
    NeuralNetwork N(2, LS);

	std::vector< double > data(2);
	std::vector< double > res = {0};
	std::cout << N << std::endl;

	for (int i = 0; i < 100; i++)
	{
		data[0] = (double)rand() / RAND_MAX;
		data[1] = (double)rand() / RAND_MAX;
		res[0] = (data[0] - data[1] > 10) ? 1 : 0;
	//	res[1] = data[0];


		std::cout << N << std::endl;
		std::vector< double > ans = N(data);
		std::cout << "Data: " << data[0] << " " << res[0] << std::endl;
		std::cout << ans[0] <<  " " << ans[1] << std::endl;
        N.backtrace(res, 1);
	}

	double dx, dy;
	while(true) {
		cin >> dx >> dy;
		data[0] = dx;
		data[1] = dy;
		std::vector< double > ans = N(data);
		std::cout << ans[0] << std::endl;
	}

    return 0;
}
