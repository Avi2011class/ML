#include <iostream>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>
#include <functional>
#include <cmath>
#include <fstream>
#include <ctime>
#include <random>
#include <sstream>
#include <cstring>

template< typename _T >
void dump(_T container)
{
	for (auto & iter: container)
		std::cout << iter << " ";
	std::cout << std::endl;
}


template< typename _T >
void dump_gnuplot(_T container)
{
	std::fstream gnu;
	gnu.open("gnu.log", std::ios::out);
	for (auto & iter: container)
		gnu << iter.x << " " << iter.y << std::endl;
	gnu << container[0].x << " " << container[0].y << std::endl;
	gnu.close();
	system("gnuplot gnuplot.gnu");
}

void generate_map(int size)
{
	std::set< std::pair< int, int > > result;
    while(result.size() < size)
		result.insert(std::make_pair(rand() % 200, rand() % 200));
	std::fstream output_stream;
	output_stream.open("points.data", std::ios::out);
	for (auto & iter: result)
		output_stream << iter.first << " " << iter.second << std::endl;
	output_stream.close();
}

class Point
{
public:
	int x, y;

	Point(int x_coord, int y_coord) : x(x_coord), y(y_coord) {};
	Point() : x(0), y(0) {};

	Point & operator = (const Point & other)
	{
		x = other.x;
		y = other.y;
		return (*this);
	}

	double dist(Point & other)
	{
		return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}
	friend std::ostream & operator << (std::ostream & ostr, Point & P)
	{
		ostr << "(" << P.x << ", " << P.y << ")";
		return ostr;
	}
};

class Way: public std::vector< Point >
{
public:
    double getWayLength()
    {
    	double result = 0;
        for (size_t i = 0; i + 1 < size(); i++)
			result += (*this)[i].dist((*this)[i + 1]);
		result += (*this)[0].dist((*this)[size() - 1]);
		return result;
    }
};

double acceptanceProbability(double energy, double newEnergy, double temperature)
{
    if (newEnergy < energy)
		return 1.0;
	else
		return exp((energy - newEnergy) / temperature);
}

inline int area(Point a, Point b, Point c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline bool intersect_1 (int a, int b, int c, int d)
{
	if (a > b)  std::swap (a, b);
	if (c > d)  std::swap (c, d);
	return std::max(a,c) < std::min(b,d);
}

bool intersect (Point a, Point b, Point c, Point d)
{
	return intersect_1 (a.x, b.x, c.x, d.x)
		&& intersect_1 (a.y, b.y, c.y, d.y)
		&& area(a,b,c) * area(a,b,d) < 0
		&& area(c,d,a) * area(c,d,b) < 0;
}

bool clear_intersection(Way & W)
{
	bool flag = false;
	W.push_back(W[0]);
    for (int i = 0; i + 1 < W.size(); i++)
		for (int j = i + 2; j + 1 < W.size(); j++)
			if (intersect(W[i], W[i + 1], W[j], W[j + 1])) {
				double lb = W.getWayLength();
                std::reverse(W.begin() + i + 1, W.begin() + j + 1);
				double la = W.getWayLength();
				if (la > lb)
					std::cout << W[i] << W[i + 1] << W[j] << W[j + 1] << std::endl;
                flag = true;
			}
	W.resize(W.size() - 1);
	return flag;
}
//*/



int main(int argc, char** argv)
{
	std::srand(time(0));
	if (argc > 2 && !strcmp("-regen", argv[1]))
		generate_map(atoi(argv[2]));

	Way cities;
	Way solution;

    std::fstream input_stream;
	input_stream.open("points.data", std::ios::in);
	int x_tmp, y_tmp;
	while (input_stream >> x_tmp >> y_tmp) {
		cities.push_back(Point(x_tmp, y_tmp));
	}
	input_stream.close();

	//dump(cities);
    solution = cities;

    double temp = 15000;
	while (temp > 0.0001) {
		cities = solution;
        int index1 = (int) (std::rand() % cities.size());
        int index2 = (int) (std::rand() % cities.size());
        if (index2 < index1)
			std::swap(index1, index2);

		if (rand() % 3 == 0) {
            Point buffer = cities[index1];
            for (int i = index1; i < index2; i++)
				cities[i] = cities[i + 1];
			cities[index2] = buffer;
		}
		else if (rand() % 2 == 0) {
			Point buffer = cities[index2];
            for (int i = index2; i > index1; i--)
				cities[i] = cities[i - 1];
			cities[index1] = buffer;
		}
        else if (rand() % 3 == 0)
			std::reverse(cities.begin() + index1, cities.begin() + index2 + 1);
		else if (rand() % 2 == 0)
			std::swap(cities[index1], cities[index2]);
		else
			std::random_shuffle(cities.begin() + index1, cities.begin() + index2 + 1);

        if (rand() % 50 == 0)
			clear_intersection(solution);

		double energy = solution.getWayLength();
		double newEnergy = cities.getWayLength();

		if (acceptanceProbability(energy, newEnergy, temp) > (double)std::rand() / RAND_MAX) {
            solution = cities;
		}
		temp *= 0.99997;
    }



	while(clear_intersection(solution)) ;
		//std::cout << solution.getWayLength() << std::endl;

	std::cout << solution.getWayLength() << std::endl;

	dump(solution);
	dump_gnuplot(solution);
	return 0;
}
