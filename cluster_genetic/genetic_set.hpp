#ifndef GSET_HPP_INCLUDED
#define GSET_HPP_INCLUDED

#include "math.hpp"
#include "gene.hpp"
#include "log.hpp"
//////////////////////////////////////////////////////////////////////////
//   mmm                         m      "            mmmm           m   //
// m"   "  mmm   m mm    mmm   mm#mm  mmm     mmm   #"   "  mmm   mm#mm //
// #   mm #"  #  #"  #  #"  #    #      #    #"  "  "#mmm  #"  #    #   //
// #    # #""""  #   #  #""""    #      #    #          "# #""""    #   //
//  "mmm" "#mm"  #   #  "#mm"    "mm  mm#mm  "#mm"  "mmm#" "#mm"    "mm //
//////////////////////////////////////////////////////////////////////////

class GeneticSet
/**
	\brief The main project class provides a standard implementation of the genetic optimization algorithm.
*/
{
protected:
	__b_vector	coeff;					///< Coefficients of diophantine equation
	__b_int		result;					///< Expected result of coefficients linear combinations

	CRandomT< size_t > G_pairs;			///< Generator to create pairs of parents to crossover
	CRandomT< size_t > G_split;			///< Generator to create split index in crossover (deprecated!)
	CRandomT< size_t > G_index;			///< Generator to create random index in genes

	CRandomT< __b_int > G_mutate_soft;	///< Generator to create weak vibrations fixed amplitude
	CRandomT< __b_int > G_mutate_norm;	///< Generator to create vibrations which amplitude are depended on result
	CRandomT< __b_int > G_mutate_hard;	///< Generator to create fatal mutation

	LRandomT< size_t > G_erase;			///< Generator to erase bad genes from set

public:

	size_t		population;				///< Size of one genetic set
	std::vector< __b_vector > genes;	///< Vector of Genes

	GeneticSet(__b_vector init, __b_int set_result, size_t set_population) :
			population(set_population), result(set_result)
	/**
		\brief The main constructor of GeneticSet
		\param init - vector of coefficients
		\param set_result - the expected result of linear combination
		\param set_population - the maximum population of genes in set
	*/
	{
		coeff.clear();
		std::copy(init.begin(), init.end(), std::back_inserter(coeff));

		G_pairs = CRandomT< size_t >(population);
		G_split = CRandomT< size_t >(coeff.size() + 1);
		G_index = CRandomT< size_t >(population);

        G_mutate_soft = CRandomT< __b_int >(-19, 20);
        G_mutate_norm = CRandomT< __b_int >(-(result / 300) - 19, (__b_int)(result / 300) + 20) ;
		G_mutate_hard = CRandomT< __b_int >(-(result / 30) - 199, (__b_int)(result / 30) + 200);

		G_erase = LRandomT< size_t >(population * 4);
	}

	void createPopulation()
	/**
		\brief This method creates started population to genetic algorithm
	*/
	{
		genes.clear();
		CRandomT< __b_int > Gen(result + 1);

		for (int i = 0; i < (int)population; i++) {
			__b_vector current_vector = {0};
			for (size_t j = 0; j < coeff.size(); j++)
				current_vector.push_back(Gen());

			__b_int fitness = GeneticMath::fitness_calc(current_vector, coeff, result);

			current_vector[0] = fitness;

			genes.push_back(current_vector);
		}

		std::sort(genes.begin(), genes.end(),
				[](__b_vector & __1, __b_vector & __2) -> bool{
                    return __1[0] < __2[0];
				});
	}

	void crossover()
	/**
		\brief This method does one iteration of crossover
	*/
	{
		std::vector< std::pair< size_t, size_t > > parents;
		__b_vector new_gene;
		while (parents.size() != genes.size()) {
			size_t a = G_pairs(), b = G_pairs();
			while (b == a)		b = G_pairs();
			if (a > b)			std::swap(a, b);
			parents.push_back(std::make_pair(a, b));
		}

		for (int p_id = 0; p_id < (int)parents.size(); p_id++) {
			__b_vector new_gene;
			__b_int fitness = 0;
			new_gene.assign(coeff.size() + 1, 0);
			for (size_t i = 1; i < coeff.size() + 1; i++)
				new_gene[i] = (genes[parents[p_id].first][i] + genes[parents[p_id].second][i]) / 2;

			fitness = GeneticMath::fitness_calc(new_gene, coeff, result);
			new_gene[0] = fitness;

			genes.push_back(new_gene);
		}

		std::sort(genes.begin(), genes.end(),
				[](__b_vector & __1, __b_vector & __2) -> bool{
                    return __1[0] < __2[0];
				});

		while (genes.size() > population)
				genes.erase(genes.begin() + G_erase(genes.size()));
	}

	void mutation()
	/**
		\brief This method does one iteration of three types mutation
	*/
	{
		for (int i = 0; i < (int)population; i++) {
			__b_vector new_gene;
			__b_int fitness = 0;
			size_t source = G_pairs();
			std::copy(genes[source].begin(), genes[source].end(),
					std::back_inserter(new_gene));

			if (genes[source][0] > 100) {
				for (__b_vector::iterator it = new_gene.begin() + 1; it != new_gene.end(); it++)
					if (rand() % 20 > 0)
						*it += G_mutate_norm();
					else
						*it += G_mutate_hard();
			}
			else if (rand() % 2 == 0) {
				for (__b_vector::iterator it = new_gene.begin() + 1; it != new_gene.end(); it++)
					*it += G_mutate_norm();
			}
			else {
				for (__b_vector::iterator it = new_gene.begin() + 1; it != new_gene.end(); it++)
					*it += G_mutate_soft();
			}

			fitness = GeneticMath::fitness_calc(new_gene, coeff, result);
			new_gene[0] = fitness;

			genes.push_back(new_gene);
		}

		std::sort(genes.begin(), genes.end(),
				[](__b_vector & __1, __b_vector & __2) -> bool{
                    return __1[0] < __2[0];
				});

		while (genes.size() > population)
			genes.erase(genes.begin() + G_erase(genes.size()));
	}

	void dump()
	/**
		\brief This method dumps GeneticSet to std::cout (stdout)
	*/
	{
        std::cout << "Genetic set (result: " << result <<
							", population: " << population << ")" << std::endl;

		Log::dump_linear_container("Coefficients", coeff);

		for (size_t i = 0; i < genes.size(); i++) {
			Log::dump_linear_container(genes[i]);
		}
		std::cout << std::endl;
	}
};

#endif // GSET_HPP_INCLUDED
