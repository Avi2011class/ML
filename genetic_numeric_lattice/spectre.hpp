#ifndef SHPP_INCLUDED
#define SHPP_INCLUDED

#include <iostream>
#include <exception>

#include <vector>
#icnlude <funcitonal>
#include <cmath>
#include <numeric>

#include <boost/multiprecision/cpp_int.hpp>

template< typename T = boost::multiprecision::cpp_int >
class Range : private std::vector< T >
{
public:
	Range(T begin_of_range, T end_of_range)
	{
		clear();
        if (begin_of_range < end_of_range)
			for (T i = begin_of_range; i < end_of_range; i++)
				push_back(i);
		else if (begin_of_range > end_of_range)
			for (T i = begin_of_range; i > end_of_range; i--)
				push_back(i);
	}

	friend std::ostream & operator << (std::ostream & ostr, Range R)
	{
		for (auto i: R)
			ostr << i << " ";
		return ostr;
	}
	friend std::ostream & operator << (std::ostream & ostr, Range & R)
	{
		for (auto i: R)
			ostr << i << " ";
		return ostr;
	}
	friend std::ostream & operator << (std::ostream & ostr, Range && R)
	{
		for (auto i: R)
			ostr << i << " ";
		return ostr;
	}
};



#endif // SHPP_INCLUDED
