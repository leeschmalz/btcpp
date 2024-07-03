#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include "modular_inverse.h"

using namespace boost::multiprecision;
using std::string;

class Curve {
// y^2 = x^3 + ax + b
public:
    const cpp_int a;
    const cpp_int b;
    const cpp_int p;

    Curve(const string& a_str, const string& b_str, const string& p_str)
        : a(cpp_int(a_str)), b(cpp_int(b_str)), p(cpp_int(p_str)) {}

    bool operator==(const Curve& other) const { // check if two curves are the same
        return a == other.a && b == other.b && p == other.p;
    }

    bool operator!=(const Curve& other) const {
        return !(*this==other);
    }
};

class Point {
public:
    const cpp_int x;
    const cpp_int y;
    const Curve& curve;
    const bool is_identity; // x, y should be ignored when is_identity is true

    Point(const string& x_str, const string& y_str, const Curve& c, const bool identity = false)
        : x(cpp_int(x_str)), y(cpp_int(y_str)), curve(c), is_identity(identity) {}

    bool isOnCurve() {
        cpp_int rhs = (x * x * x + curve.a * x + curve.b) % curve.p;
        cpp_int lhs = (y * y) % curve.p;

        return rhs == lhs;
    }

    // overload + for point addition
    Point operator+(const Point& other) const {
        if (curve != other.curve) {
            throw std::invalid_argument("Cannot add points on different curves.");
        }

        if (typeid(other) != typeid(Point)) { // if other is not a point
            throw std::invalid_argument("Unsupported operand type. Expected Point object.");
        }

        if (is_identity) {
            return other;
        }

        if (other.is_identity) {
            return *this;
        }

        // if points are on a vertical line -> identity
        if (x == other.x && y == -(other.y-curve.p)) {
            Point identity_point("0x0", "0x0", curve, true);
            return identity_point;
        }

        cpp_int slope;

        if (x == other.x and y == other.y){
            // if the points are the same, we want the slope of the tanget line
            slope = (3 * x*x + curve.a) * modularInverse(2 * y, curve.p);
        }else{
            slope = (other.y - y) * modularInverse(other.x - x, curve.p);
        }

        cpp_int xr = (slope*slope - x - other.x) % curve.p;
        cpp_int yr = (-(slope*(xr - x) + y)) % curve.p;
        
        string xr_str = xr.str(16); // convert to hex to init new point
        string yr_str = yr.str(16);

        return Point(xr_str, yr_str, curve);
    }
};

int main() {
    Curve bitcoin_curve( // secp256k1
        "0x0",
        "0x7",
        "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"
    );

    Point bitcoin_G( // bitcoin generator point
        "0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798",
        "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8",
        bitcoin_curve
    );

    assert(bitcoin_G.isOnCurve());

    Curve curve1("2", "3", "7");
    Curve curve2("2", "3", "7");
    Curve curve3("2", "3", "101");

    assert(curve1 == curve2); // test same curve
    assert(curve1 != curve3); // test different curve

    Point point1("3", "4", curve1);
    Point point2("3", "3", curve1);
    Point point1addpoint2 = point1 + point2;
    assert(point1addpoint2.is_identity); // test add vertical points gives identity

    Point point3 = point2 + point1addpoint2;
    assert(point3.x == point2.x && point3.y == point2.y); // add point to identity point gives itself

    Point bitcoin_2G = bitcoin_G + bitcoin_G;
    string g_string = bitcoin_G.x.str(0, std::ios_base::hex);
    string g2_string = bitcoin_2G.x.str(0, std::ios_base::hex);
    std::cout << "G.x: " << g_string << std::endl;
    std::cout << "2G.x: " << g2_string << std::endl;
    assert(bitcoin_2G.isOnCurve());

    std::cout << "All tests passed!" << std::endl;

    return 0;
}