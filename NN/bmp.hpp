#ifndef BMP_HPP_INCLUDED
#define BMP_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <exception>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

#include "neural.hpp"

int saveMapBmp(std::string filename, NeuralNetwork & N, int mapSize)
{
	sf::Color color = sf::Color::White;
	sf::Image img;
	img.create(mapSize, mapSize, color);
	std::vector< double > data(2);
	std::vector< double > res(1);
    for (int i = 0; i < mapSize; i++)
		for (int j = 0; j < mapSize; j++) {
            data[0] = (double)i / mapSize;
            data[1] = (double)j / mapSize;
            res = N(data);
            sf::Color color;
            color.r = res[0] * 256;
            color.g = res[0] * 256;
            color.b = res[0] * 256;
            img.setPixel(i, mapSize - j - 1, color);
		}
	img.saveToFile(filename.c_str());
	return 0;
}

class BmpCalc
{
public:
	sf::Image image;
	uint32_t height, width;
	BmpCalc(std::string filename) {
		image.loadFromFile(filename);
		height = image.getSize().x;
		width = image.getSize().y;
	}

	double operator ()(double x, double y, std::vector< double > & result)
	{
		y = 1 - y;
        sf::Color color = image.getPixel((uint32_t)(x * (height - 1)),
														(uint32_t)(y * (width - 1)));
        result[0] = ((double)color.r + (double)color.b + (double)color.g) / 256 / 3;
        //result[1] = (double)(color.g) / 256;
        //result[2] = (double)(color.b) / 256;
        return result[0];
	}
};

std::vector< std::pair< double, double > > getControlPoints(BmpCalc & BC)
{
	std::vector< double > res = {0};
	std::set< std::pair< double, double > > control;
    for(double i = 0; i < 1; i += 0.01)
		for(double j = 0; j < 1 - 0.02; j += 0.01)
			if (abs(BC(i, j, res) - BC(i, j + 0.02, res)) > 0.4) {
				control.insert(std::make_pair(i, j + 0.01));
				control.insert(std::make_pair(i, j + 0.03));
				j += 0.04;
			}
    for(double i = 0; i < 1; i += 0.01)
		for(double j = 0; j < 1 - 0.02; j += 0.01)
			if (abs(BC(j, i, res) - BC(j + 0.02, i, res)) > 0.4) {
				control.insert(std::make_pair(j + 0.01, i));
				control.insert(std::make_pair(j + 0.03, i));
				j += 0.04;
			}
	std::vector< std::pair< double, double > > result;
	std::copy(control.begin(), control.end(), std::back_inserter(result));
	return result;
}

#endif // BMP_HPP_INCLUDED

















