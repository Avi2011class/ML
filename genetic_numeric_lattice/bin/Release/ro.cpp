#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int main(int argc, char** argv)
{
	int step = 10;
	if (argc > 1)
		step = atoi(argv[1]);
	
	std::fstream in, out;
	in.open("time.txt", std::ios::in);
	out.open("stat.txt", std::ios::out);

	std::vector< int > data;
	std::copy(std::istream_iterator< int >(in), std::istream_iterator< int >(), std::back_inserter(data));

	std::vector< int > result(*std::max_element(data.begin(), data.end()) / step + 1);
	for (int cur: data)
		result[cur / step]++;
	
	for (int i = 0; i < MIN(result.size(), 200); i++)
		out << i << " " << result[i] << std::endl;

	in.close();
	out.close();

	return 0;
}
