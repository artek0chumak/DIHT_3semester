#include <iostream>
#include <array>
#include <initializer_list>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>


struct Vector {
public:
    Vector(): x(0), y(0), z(0)
    {}
    Vector(const Vector& v) = default;
    Vector(Vector&& v) noexcept = default;
    Vector& operator=(const Vector& v) = default;
    Vector& operator=(Vector&& v) noexcept = default;

    Vector(double _x, double _y, double _z): x(_x), y(_y), z(_z)
    {}

    double dot(Vector b) const {
        return x * b.x + y * b.y + z * b.z;
    }
    Vector cross(Vector b) const {
        return Vector( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }
    double triple(Vector b, Vector c) const {
        return dot(b.cross(c));
    }
    double abs() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    double x;
    double y;
    double z;
};

Vector operator+(Vector a, Vector b) {
    return Vector(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector operator-(Vector a) {
    return Vector(-a.x, -a.y, -a.z);
}

Vector operator-(Vector a, Vector b) {
    return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector operator*(Vector a, double b) {
    return Vector(a.x * b, a.y * b, a.z * b);
}

Vector operator/(Vector a, double b) {
    return Vector(a.x / b, a.y / b, a.z / b);
}


class Segment{
public:
    Segment(Vector& begin, Vector& end): begin(begin), end(end)
    {}

    static double closetDistance(Segment seg1, Segment seg2, double epsilon) {
        double a = seg1.direction().dot(seg1.direction());
        double b = seg1.direction().dot(seg2.direction());
        double c = seg2.direction().dot(seg2.direction());
        double d = seg1.direction().dot((seg1.begin - seg2.begin));
        double e = seg2.direction().dot((seg1.begin - seg2.begin));

        double D = a * c - b * b;
        double sc, sN, sD = D;
        double tc, tN, tD = D;

        if(D <= epsilon) {
            sN = 0;
            sD = 1;
            tN = e;
            tD = c;
        }
        else {
            sN = (b * e - c * d);
            tN = (a * e - b * d);
            if(sN <= 0) {
                sN = 0;
                tN = e;
                tD = c;
            }
            else if(sN >= sD) {
                sN = sD;
                tN = e + b;
                tD = c;
            }
        }

        if(tN <= 0) {
            tN = 0;
            if(d >= 0) {
                sN = 0;
            }
            else if(d <= -a) {
                sN = sD;
            }
            else {
                sN = -d;
                sD = a;
            }
        }
        else if(tN >= tD) {
            tN = tD;
            if(b - d <= 0.0) {
                sN = 0;
            }
            else if(b - d >= a) {
                sN = sD;
            }
            else {
                sN = b - d;
                sD = a;
            }
        }

        sc = (std::abs(sN) <= epsilon ? 0 : sN / sD);
        tc = (std::abs(tN) <= epsilon ? 0 : tN / tD);

        Vector dP = seg1.begin - seg2.begin + seg1.direction() * sc - seg2.direction() * tc;

        return dP.abs();
    }

    Vector direction() {
        return end - begin;
    }

    Vector begin;
    Vector end;
};


int main() {
    std::array<Vector, 4> points;
    for(int i = 0; i < 4; ++i) {
        std::cin >> points[i].x >> points[i].y >> points[i].z;
    }

    Segment segment1(points[0], points[1]);
    Segment segment2(points[2], points[3]);

    std::cout << std::setprecision(10) << Segment::closetDistance(segment1, segment2, 1e-10) << std::endl;

    return 0;
}