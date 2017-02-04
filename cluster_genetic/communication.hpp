#ifndef COMMUNICATION_HPP_INCLUDED
#define COMMUNICATION_HPP_INCLUDED

#include <boost/multiprecision/cpp_int/serialize.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


#include "genetic_set.hpp"

class Communicator
{
private:
	boost::mpi::communicator	clusterCommonWorld;
	LRandomT< size_t >			G_erase;

public:
    Communicator(boost::mpi::communicator comm, size_t population)
    {
        clusterCommonWorld = comm;
        G_erase = LRandomT< size_t >(population * clusterCommonWorld.size() + 1);
    }

    bool dataInterchange(GeneticSet & currentSet)
    {
		Log::dump_linear_container(std::to_string(clusterCommonWorld.rank()), currentSet.genes[0]);

        std::vector< std::vector< __b_vector > > AllSets;

        boost::mpi::all_gather(clusterCommonWorld, currentSet.genes, AllSets);

        std::vector< __b_vector > GeneSet;
        for (size_t i = 0; i < AllSets.size(); i++)
			for (size_t j = 0; j < AllSets[i].size(); j++)
                GeneSet.push_back(AllSets[i][j]);

		std::sort(GeneSet.begin(), GeneSet.end());

		if (GeneSet[0][0] == 0) {
			if (clusterCommonWorld.rank() == 0)
				Log::dump_linear_container("Solved", GeneSet[0]);
			return true;
		}

		if (clusterCommonWorld.rank() != 0 || clusterCommonWorld.size() < 3) {
			while (GeneSet.size() > currentSet.population)
					GeneSet.erase(GeneSet.begin() + G_erase(GeneSet.size()));
			for (size_t i = 0; i < currentSet.population; i++) {
				currentSet.genes[i] = GeneSet[i];
			}
		}
		else
			currentSet.createPopulation();

        if (clusterCommonWorld.rank() == 0)
			std::cout << "Data interchanged, best fit = " << GeneSet[0][0] << std::endl;

        return false;
    }
};

#endif // COMMUNICATION_HPP_INCLUDED
