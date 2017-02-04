#ifndef LHPP_INCLUDED
#define LHPP_INCLUDED

#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <algorithm>
#include <iterator>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <boost/lexical_cast.hpp>

#include "config.hpp"

template< typename _T >
void dump_container(_T container)
{
    std::copy(container.begin(), container.end(),
			std::ostream_iterator< typename _T::value_type >(std::cout, " "));
	std::cout << std::endl;
}

class Lattice
{
public:
    __b_vector alleles;
    __b_int fitness;

	Lattice() : alleles(__b_vector()), fitness(0) {};

    Lattice(__b_vector init, __b_int fitness) : fitness(fitness)
    {
    	alleles.clear();
        std::copy(init.begin(), init.end(), std::back_inserter(alleles));
    }

    bool operator == (Lattice & other)
    {
    	if (alleles.size() != other.alleles.size())
			return false;
		for (size_t i = 0; i < alleles.size(); i++)
			if (alleles[i] != other.alleles[i])
				return false;
		return true;
    }
};

#endif
