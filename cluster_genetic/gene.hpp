#ifndef GENHPP_INCLUDED
#define GENHPP_INCLUDED

#include <iterator>
#include <algorithm>

#include "config.hpp"

class Gene : public __b_vector
/**
	\brief Gene class provides one gene structure and fitness function of allele
*/
{
public:
    __b_int fitness;

	Gene() : __b_vector(), fitness(0)
	/**
		\brief Constructor of free gene
    */
	{};

	Gene(__b_vector init, __b_int fitness) : __b_vector(init), fitness(fitness)
    /**
		\brief Gene constructor from data vector
		\param init is vector of data
		\param fitness is of fitness function on "init" vector
    */
    {};

    bool operator == (Gene & other)
    /**
		\brief this operator check two genes for equality
    */
    {
    	if (size() != other.size())
			return false;
		for (size_t i = 0; i < size(); i++)
			if ((*this)[i] != other[i])
				return false;
		return true;
    }

    bool operator < (Gene & other)
    /**
		\brief this operator compare fitnesses of two genes, it needs to std::sort
    */
    {
    	return fitness < other.fitness;
    }
};

#endif // GENHPP_INCLUDED
