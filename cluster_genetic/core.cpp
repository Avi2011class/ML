#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

#define DEBUG
#include <boost/mpi.hpp>

#include "genetic_set.hpp"
#include "communication.hpp"

int main(int argc, char** argv, char** env)
{
	size_t population = 30;

	boost::mpi::environment mpiEnvironment(argc, argv);
	boost::mpi::communicator mpiGlobalCommunicator;

	Communicator C(mpiGlobalCommunicator, population);

    std::fstream inp;
    inp.open("input.txt", std::ios::in);
    std::vector< __b_int > data;
    std::copy(std::istream_iterator< __b_int >(inp), std::istream_iterator< __b_int >(), std::back_inserter(data));
    __b_int result = data[data.size() - 1];
    data.resize(data.size() - 1);

	GeneticSet S(data, result, population);
	S.createPopulation();

	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 30000; j++) {
			S.crossover();
			S.mutation();
		}

		if (C.dataInterchange(S))
			return 0;
	}
	inp.close();
	return 0;
}
