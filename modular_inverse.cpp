#include "modular_inverse.h"

cpp_int modularInverse(const cpp_int& x, const cpp_int& p) {
    cpp_int old_r = x;
    cpp_int r = p;

    cpp_int old_s = 1;
    cpp_int s = 0;

    while (r != 0) {
        cpp_int quotient = old_r / r;

        cpp_int r_copy = r;
        r = old_r - quotient * r;
        old_r = r_copy;

        cpp_int s_copy = s;
        s = old_s - quotient * s;
        old_s = s_copy;
    }

    return old_s % p;
}