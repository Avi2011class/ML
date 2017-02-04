#ifndef NEURAL_HPP_INCLUDED
#define NEURAL_HPP_INCLUDED

#include <functional>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <set>
#include <iterator>
#include <algorithm>
#include <random>
#include <numeric>
#include <exception>
#include <iomanip>

class Neuron
{
public:
    int inputCount;
    std::vector< double > inputWeight;
    double fictiveWeight;
    double outputCache;
    double inputCache;

	Neuron() : inputCount(-1)
	{
	};

	static inline double activationFunction(double arg)
	{
		return 1 / (1 + exp(-arg)) + 0.01 * arg;
	}
	static inline double derivativeFunction(double arg)
	{
		double ex = exp(arg);
		return ex / (1 + ex) / (1 + ex) + 0.01;
	}
    Neuron(int argInputCount) : inputCount(argInputCount)
    {
    	outputCache = 0;

		std::random_device rd;
		std::mt19937 randomGenerator(rd());
        std::uniform_real_distribution< double > defaultDistribution(-0.2, 0.2);
        std::function< double() > weightGenerator = [&defaultDistribution, &randomGenerator]() -> double
															{ return defaultDistribution(randomGenerator); };
        std::generate_n(std::back_inserter(inputWeight), inputCount, weightGenerator);
        fictiveWeight = weightGenerator();
    };

    double operator () (std::vector< double > & inputData)
    {
        if ((int)inputData.size() != inputCount)
			throw std::range_error("Input vector has bad size");

        double summarWeight = std::inner_product(inputWeight.begin(), inputWeight.end(), inputData.begin(), fictiveWeight);
        inputCache = summarWeight;

        outputCache = activationFunction(summarWeight);
        return outputCache;
    }

    friend std::ostream & operator << (std::ostream & os, Neuron & This)
    {
    	os << This.inputCount << "   " << This.fictiveWeight << "    ";
        for (auto & weight: This.inputWeight)
			os << std::fixed << std::setprecision(8) << weight << " ";
		return os;
    }

    friend std::istream & operator >> (std::istream & is, Neuron & This)
    {
        is >> This.inputCount >> This.fictiveWeight;
        This.inputWeight.resize(This.inputCount);
        for (auto & weight: This.inputWeight)
			is >> weight;
		return is;
    }
};

class NeuralNetwork
{
public:
	int layerCount;
	int inputSize;
    std::vector< int > layerSize;
    std::vector< std::vector< Neuron > > Neurons;
    std::vector< double > inputCache;

	NeuralNetwork() {};
    NeuralNetwork(int argInputSize, std::vector< int > argLayerSize)
    {
    	inputSize = argInputSize;
    	layerSize = argLayerSize;
    	layerCount = layerSize.size();

        Neurons.resize(layerCount);

        for (int i = 0; i < layerCount; i++) {
            Neurons[i].resize(layerSize[i]);
        }

        #pragma omp parallel for schedule(static)
        for (int i = 0; i < layerCount; i++) {
			for (int j = 0; j < layerSize[i]; j++)
				Neurons[i][j] = Neuron((i > 0) ? layerSize[i - 1] : inputSize);
        }
    }

	std::vector< double > operator () (std::vector< double > data)
	{
		if ((int)data.size() != inputSize)
			throw std::range_error("#__LINE__");

        inputCache = data;

        for (int layerIndex = 0; layerIndex < layerCount; layerIndex++) {
            std::vector< double > tmp(layerSize[layerIndex]);

            #pragma omp parallel for schedule(static)
            for (int i = 0; i < layerSize[layerIndex]; i++) {
                tmp[i] = Neurons[layerIndex][i](data);
            }
            data = tmp;
        }

        return data;
	}

	void backtrace(std::vector< double > correctOutput, double alpha)
	{
		if (correctOutput.size() != Neurons[Neurons.size() - 1].size())
			throw std::range_error("back-trace size error");

		std::vector< double > previousErrors;
		std::vector< double > previousErrorsBuffer;

        for(int i = layerCount - 1; i >= 0; i--) {
			// cache for errors transactions
			previousErrorsBuffer.assign(  	(i > 0) ? layerSize[i - 1] : inputSize , 0);

			//cache for layers input
			std::vector< double > previousTransactions;
			if (i > 0)
				for (int j = 0; j < layerSize[i - 1]; j++)
					previousTransactions.push_back(Neurons[i - 1][j].outputCache);
			else
				previousTransactions = inputCache;


			// if final layer
			if (i == layerCount - 1) {
				for (int j = 0; j < layerSize[i]; j++) {
					double sigmaError = (correctOutput[i] - Neurons[i][j].outputCache) *
										Neuron::derivativeFunction(Neurons[i][j].inputCache);
					std::cout << "SE: " << sigmaError << "; ";
                    for(int from = 0; from < previousTransactions.size(); from++) {
						double dw = previousTransactions[from] * sigmaError * alpha;

						previousErrorsBuffer[from] += sigmaError * Neurons[i][j].inputWeight[from];
						Neurons[i][j].inputWeight[from] += dw;

						std::cout << "dw[" << from << "]=" << dw << "; ";
                    }
                    std::cout << "fw=" << sigmaError * alpha << "; " << std::endl;
                    Neurons[i][j].fictiveWeight += sigmaError * alpha;
				}
			}
			// middle layers
			else {
				for (int j = 0; j < layerSize[i]; j++) {
					double sigmaError = previousErrors[j] *
										Neuron::derivativeFunction(Neurons[i][j].inputCache);
					std::cout << "SE: " << sigmaError << "; ";
                    for(int from = 0; from < previousTransactions.size(); from++) {
						double dw = previousTransactions[from] * sigmaError * alpha;

						previousErrorsBuffer[from] += sigmaError * Neurons[i][j].inputWeight[from];
						Neurons[i][j].inputWeight[from] += dw;

						std::cout << "dw[" << from << "]=" << dw << "; ";
                    }
                    std::cout << "fw=" << sigmaError * alpha << "; " << std::endl;
                    Neurons[i][j].fictiveWeight += sigmaError * alpha;
				}
			}
			previousErrors = previousErrorsBuffer;
        }
	}

    friend std::ostream & operator << (std::ostream & os, NeuralNetwork & This)
    {
        os << This.layerCount << std::endl;
        for (auto & layer: This.layerSize)
			os << layer << " ";
		os << std::endl;
        for (auto & layer: This.Neurons)
			for (auto & neuron: layer)
				os << neuron << std::endl;
		return os;
    }

    friend std::istream & operator >> (std::istream & is, NeuralNetwork & This)
    {
        is >> This.layerCount;
        This.layerSize.resize(This.layerCount);
        for (auto & layer: This.layerSize)
			is >> layer;

        This.Neurons.resize(This.layerCount);

        for (int i = 0; i < This.layerCount; i++) {
			This.Neurons[i].resize(This.layerSize[i]);
        }

		for (auto & layer: This.Neurons)
			for (auto & neuron: layer)
				is >> neuron;
		return is;
    }
};

#endif // NEURAL_HPP_INCLUDED
