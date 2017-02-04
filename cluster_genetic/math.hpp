#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

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

/////////////////////////////////////////////////////////////
//   mmm  mmmmm                    #               mmmmmmm //
// m"   " #   "#  mmm   m mm    mmm#   mmm   mmmmm    #    //
// #      #mmmm" "   #  #"  #  #" "#  #" "#  # # #    #    //
// #      #   "m m"""#  #   #  #   #  #   #  # # #    #    //
//  "mmm" #    " "mm"#  #   #  "#m##  "#m#"  # # #    #    //
/////////////////////////////////////////////////////////////
template< typename _T >
class CRandomT
/**
	\brief This class provides methods for generating random numbers within a range with a uniform probability distribution
*/
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
	CRandomT() : limit(256), begin_range(0), end_range(256)
	/**
		\brief Constructor random number generator operating in a range [0, 256)
	*/
	{
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8, _T >(baseGen);
		mode = ZERO;
	}

	CRandomT(_T limit_of_range) : limit(limit_of_range), begin_range(0), end_range(limit_of_range)
	/**
		\brief Constructor random number generator operating in a range [0, limit)
		\param limit_of_range is the upper limit of range
	*/
	{
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8, _T >(baseGen);
		mode = ZERO;
	}

	CRandomT(_T begin_of_range, _T end_of_range) :
			limit(end_of_range - begin_of_range), begin_range(begin_of_range), end_range(end_of_range)
	/**
		\brief Constructor random number generator operating in a range [begin_of_range, end_of_range)
		\param begin_of_range is the lower end of range
		\param end_of_range is the upper end of range
	*/
	{
		if (begin_of_range >= end_of_range)
			throw std::invalid_argument("begin_of_range should not be equal or greater then end_of_range");

        baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8, _T >(baseGen);
		mode = RANGE;
	}

	_T operator ()()
	/**
		\brief Operator that generates random numbers
	*/
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
};

/////////////////////////////////////////////////////////////
// m      mmmmm                    #               mmmmmmm //
// #      #   "#  mmm   m mm    mmm#   mmm   mmmmm    #    //
// #      #mmmm" "   #  #"  #  #" "#  #" "#  # # #    #    //
// #      #   "m m"""#  #   #  #   #  #   #  # # #    #    //
// #mmmmm #    " "mm"#  #   #  "#m##  "#m#"  # # #    #    //
/////////////////////////////////////////////////////////////
template< typename _T >
class LRandomT
/**
	\brief This class provides methods for generating random numbers within a range with a linear probability distribution (rho(x) ~ x)
*/
{
	std::vector< _T > linear_base;
	boost::random::mt11213b baseGen;
	boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T > gen;
	_T limit_of_range;
public:
	LRandomT() : limit_of_range(256)
	/**
		\brief Constructor random number generator operating in a range [0, 256)
		\param limit_of_range is the upper limit of range
	*/
	{
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T >(baseGen);
		linear_base.clear();

		_T buffer = 0;
		for (_T i = 0; i < limit_of_range; i++) {
            linear_base.push_back( buffer );
            buffer += i;
		}
	}

	LRandomT(_T limit_of_range) : limit_of_range(limit_of_range)
	/**
		\brief Constructor random number generator operating in a range [0, limit)
		\param limit_of_range is the upper limit of range
	*/
	{
		baseGen = boost::random::mt11213b(clock());
		gen = boost::random::independent_bits_engine< boost::random::mt11213b,
				sizeof(_T) * 8,
				_T >(baseGen);
		linear_base.clear();

		_T buffer = 0;
		for (_T i = 0; i < limit_of_range; i++) {
            linear_base.push_back( buffer );
            buffer += i;
		}
	}
	_T operator ()(_T lim)
	/**
		\brief Operator that generates random numbers
		\param lim is upper limit of range
	*/
	{
		if (lim >= limit_of_range)
			throw std::invalid_argument("Argument should not be large then limit");

		boost::multiprecision::cpp_int data = gen() % (lim * (lim + 1) / 2);
        return std::distance(linear_base.begin(),
					std::upper_bound(linear_base.begin(), linear_base.end(), data)) - 1;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////

namespace GeneticMath
{
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
					V.begin() + 1, V.end(), coeff.begin(), static_cast< typename T::value_type >(0));
		return (result > fitness) ? (result - fitness) : (fitness - result);
	}
}


#endif // MATH_HPP_INCLUDED
