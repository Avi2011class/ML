#include <iostream>

#include "genetic_set.hpp"
#include "math.hpp"
using namespace std;

int main()
{
	std::ios::sync_with_stdio(false);

    __b_vector coeff = {1329, 1327, 1293, 2499};
    GeneticSet G(coeff, 100);
    G.solve(106211);

	//std::copy(A.begin(), A.end(), std::ostream_iterator< int >(std::cout, " "));
	return 0;
}
