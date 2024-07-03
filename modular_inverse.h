#ifndef MOD_INV_H
#define MOD_INV_H

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

cpp_int modularInverse(const cpp_int& x, const cpp_int& p);

#endif // MOD_INV_H