#ifndef GSHPP_INCLUDED
#define GSHPP_INCLUDED

#include <numeric>
#include <iterator>
#include <algorithm>
#include <future>
#include <functional>
#include <cmath>

#include <omp.h>

#include "lattice.hpp"
#include "math.hpp"

//#define DEBUG

class GeneticSet
{
protected:
	__b_vector coeff;
	size_t population;
	std::vector< Lattice > genes;

    void createPopulation(__b_int result);
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



public:
	GeneticSet(__b_vector init, size_t population) : population(population)
	{
		coeff.clear();
		std::copy(init.begin(), init.end(), std::back_inserter(coeff));
	}
	__b_vector solve(__b_int result);
	void dump();
};
///////////////////////////////////////////////////////////////
__b_vector GeneticSet::solve(__b_int result)
{
	//std::cout << "Solve started " << std::endl;

    if (!check_equation(coeff, result)) {
		std::cout << "Solution does not exists" << std::endl;
		return __b_vector();
    }

	while (1) {
		createPopulation(result);
		//std::cout << "Population created " << std::endl;
		//dump();
		CRandomT< size_t > G_pairs(genes.size());
		CRandomT< size_t > G_split(genes.size() + 1);
		CRandomT< size_t > G_index(coeff.size());

		CRandomT< __b_int > G_mutate_soft(-19, 20);
		CRandomT< __b_int > G_mutate_norm(-(result / 400) - 19, (__b_int)(result / 400) + 20);
		CRandomT< __b_int > G_mutate_hard(-(result / 40) - 199, (__b_int)(result / 40) + 200);

		#if defined(DEBUG)
		for (int i = 0; i < 30; i++)
			std::cout << G_mutate_norm() << std::endl;
		return __b_vector();
		#endif // defined

		LRandomT< size_t > G_erase(population * 4);

		for (int step = 0; step < 50000; step++) {
			// potential parents
			std::vector< std::pair< size_t, size_t > > parents;
			while (parents.size() != genes.size()) {
				size_t a = G_pairs(), b = G_pairs();
				while (b == a)		b = G_pairs();
				if (a > b)			std::swap(a, b);
				parents.push_back(std::make_pair(a, b));
			}

			// crossover
			#pragma omp parallel for
			for (int p_id = 0; p_id < (int)parents.size(); p_id++) {
				__b_vector new_gene;
				__b_int fitness = 0;
				new_gene.assign(coeff.size(), 0);

				for (size_t i = 0; i < coeff.size(); i++)
					new_gene[i] = (genes[parents[p_id].first].alleles[i] + genes[parents[p_id].second].alleles[i]) / 2;

				fitness = fitness_calc(new_gene, coeff, result);
				#pragma omp critical
				genes.push_back(Lattice(new_gene, fitness));
			}

			// mutation
			#pragma omp parallel for
			for (int i = 0; i < (int)population; i++) {
				__b_vector new_gene;
				__b_int fitness = 0;
				size_t source = G_pairs();
				std::copy(genes[source].alleles.begin(), genes[source].alleles.end(),
						std::back_inserter(new_gene));

				if (genes[source].fitness > 100)
					for (auto & iter: new_gene)
						if (rand() % 100 > 0)
							iter += G_mutate_norm();
						else
							iter += G_mutate_hard();
				else if (rand() % 2 == 0)
					for (auto & iter: new_gene)
						iter += G_mutate_norm();
				else
					for (auto & iter: new_gene)
						iter += G_mutate_soft();

				fitness = fitness_calc(new_gene, coeff, result);

				#pragma omp critical
				genes.push_back(Lattice(new_gene, fitness));
			}

			std::sort(genes.begin(), genes.end(), [](Lattice & __1, Lattice & __2) -> bool
						{ return __1.fitness < __2.fitness; });

			while (genes.size() > population)
				genes.erase(genes.begin() + G_erase(genes.size()));

			if (step % 100 == 0) {
				//dump();
				//std::cout << "step: " << step << std::endl;
			}

			if (genes[0].fitness == 0) {
				//dump();
				//std::cout << "Solution found, step: " << step << " ";
				std::cout << step << std::endl;
                //dump_container(genes[0].alleles);
				//std::cout << std::endl;
				return genes[0].alleles;
			}
		}
	}
	return genes[0].alleles;
}


void GeneticSet::dump()
{
	std::cout << "Size: " << population << std::endl;
	std::cout << "Coef: ";
	dump_container(coeff);
	for (size_t i = 0; i < genes.size(); i += 5) {
		std::cout << i << ": " << genes[i].fitness << ": ";
		dump_container(genes[i].alleles);
	}
}

void GeneticSet::createPopulation(__b_int result)
{
    genes.clear();
	CRandomT< __b_int > Gen(result + 1);

	#pragma omp parallel for
    for (int i = 0; i < (int)population; i++) {
		__b_vector current_vector;
        for (size_t i = 0; i < coeff.size(); i++)
			current_vector.push_back(Gen());
		__b_int fitness = std::inner_product(coeff.begin(), coeff.end(),
									current_vector.begin(), __b_int(0));
		fitness = abs(result - fitness);

		#pragma omp critical
		genes.push_back(Lattice(current_vector, fitness));
	}
}
#endif
