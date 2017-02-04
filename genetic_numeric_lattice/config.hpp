#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED

#include <vector>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

//typedef boost::multiprecision::int1024_t __b_int;
typedef long long int __b_int;
typedef std::vector< __b_int > __b_vector;
typedef boost::multiprecision::cpp_dec_float_100 __b_double;


#endif // CONFIG_HPP_INCLUDED
