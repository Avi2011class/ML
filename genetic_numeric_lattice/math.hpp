#ifndef MHINCLUDED
#define MHINCLUDED

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <vector>
#include <set>
#include <map>
#include <exception>
#include <functional>
#include <stack>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/detail/et_ops.hpp>
#include <boost/multiprecision/detail/number_base.hpp>
#include <boost/multiprecision/detail/number_compare.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/random.hpp>

#include <boost/random/independent_bits.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/lexical_cast.hpp>

#include "config.hpp"

template< typename _T >
class CRandomT
{
protected:
	enum random_mode{ ZERO, RANGE };
	boost::random::mt11213b baseGen;
	boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T > gen;
	_T limit, begin_range, end_range;
	random_mode mode;
public:
	CRandomT(_T __limit) : limit(__limit), begin_range(0), end_range(__limit)
	{
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8, _T >(baseGen);
		mode = ZERO;
	}

	CRandomT(_T __begin_range, _T __end_range) :
			limit(__end_range - __begin_range), begin_range(__begin_range), end_range(__end_range)
	{
        baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8, _T >(baseGen);
		mode = RANGE;
	}

	_T operator ()()
	{
		_T TG = gen();
		if (TG < 0) TG *= -1;
		if (mode == ZERO)
			return static_cast< _T >(TG) % limit;
		else if (mode == RANGE)
			return (static_cast< _T >(TG) % limit) + begin_range;
		else
			throw std::runtime_error("Unknown mode of CRandomT");
		return 0;
	}

	friend std::ostream & operator << (std::ostream & ostr, CRandomT & R)
	{
        ostr << "CRandomT mode " << ((R.mode == ZERO) ? "ZERO" : "RANGE");
        if (R.mode == RANGE)
			ostr << " begin: " << R.begin_range << " end: " << R.end_range << std::endl;
		else if (R.mode == ZERO)
			ostr << " limit: " << R.limit << std::endl;
        return ostr;
	}
};

template< typename _T >
class LRandomT
{
	std::vector< _T > linear_base;
	boost::random::mt11213b baseGen;
	boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T > gen;
	_T limit;
public:
	LRandomT(_T limit) : limit(limit) {
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T >(baseGen);
		linear_base.clear();

		_T buffer = 0;
		for (_T i = 0; i < limit; i++) {
            linear_base.push_back( buffer );
            buffer += i;
		}
	}
	_T operator ()(_T lim)
	{
		if (lim >= limit)
			throw std::invalid_argument("Argument should not be large then limit");

		boost::multiprecision::cpp_int data = gen() % (lim * (lim + 1) / 2);
        return std::distance(linear_base.begin(),
					std::upper_bound(linear_base.begin(), linear_base.end(), data)) - 1;
	}
};

template< typename _T >
constexpr inline _T gcd_two(_T __1, _T __2)
{
	if (__1 == 0) return 1;
	else
	while (__2) {
		__1 %= __2;
		std::swap(__1, __2);
	}
	return __1;
}

template< typename _T >
constexpr inline _T lcm_two(_T __1, _T __2)
{ return __1 / gcd_two(__1, __2) * __2; }

template< typename _T_iter >
constexpr inline typename _T_iter::value_type gcd_range(_T_iter begin, _T_iter end)
{
	return std::accumulate(begin, end,
				static_cast< typename _T_iter::value_type >(*begin),
				gcd_two< typename _T_iter::value_type >);
}

template< typename _T_iter >
constexpr inline typename _T_iter::value_type lcm_range(_T_iter begin, _T_iter end)
{
	return std::accumulate(begin, end,
				static_cast< typename _T_iter::value_type >(1),
				lcm_two< typename _T_iter::value_type >);
}

template< typename T >
inline static constexpr bool check_equation(T container, typename T::value_type result)
{
	return (result % gcd_range< typename T::iterator >(container.begin(), container.end()) == 0);
}

template< typename T >
inline static constexpr decltype (auto) fitness_calc(T & V, T & coeff, typename T::value_type result)
{
	typename T::value_type fitness = std::inner_product(
				V.begin(), V.end(), coeff.begin(), static_cast< typename T::value_type >(0));
    return (result > fitness) ? (result - fitness) : (fitness - result);
}
/*
template< typename _T = __b_int >
class LinearDiophantine
{
private:
	std::vector < _T > coeff;

public:
    LinearDiophantine( std::vector< _T > && arg_coeff )
    {
    	coeff.clear();
    	std::copy(arg_coeff.begin(), arg_coeff.end(), std::back_inserter(coeff));
    }
    solve( _T result )
    {
        std::vector< _T > buffer( coeff );
        std::vector< _T >::iterator positive = std::find_if(coeff.begin(), coeff.end(), []( _T item) -> bool
				{ return item > });
    }
};
//*/

#endif // MHINCLUDED
