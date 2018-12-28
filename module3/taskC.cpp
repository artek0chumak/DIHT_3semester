#include <iostream>
#include <array>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <vector>
#include <functional>


struct Vector{
public:
    Vector() : x(0), y(0)
    {}
    Vector(const Vector& v) = default;
    Vector(Vector& v) = default;
    Vector(Vector&& v) = default;
    Vector& operator= (Vector v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vector(double _x, double _y): x(_x), y(_y)
    {}

    double cross(Vector b) {
        return x * b.y - y * b.x;
    }

    double x;
    double y;
};

Vector operator+(const Vector a, const Vector b) {
    return Vector(a.x + b.x, a.y + b.y);
}

Vector operator-(const Vector a) {
    return Vector(-a.x, -a.y);
}

Vector operator-(const Vector a, const Vector b) {
    return Vector(a.x - b.x, a.y - b.y);
}

Vector operator*(const Vector a, double b) {
    return Vector(a.x * b, a.y * b);
}

Vector operator/(const Vector a, double b) {
    return Vector(a.x / b, a.y / b);
}


class Polygon{
public:
    Polygon() = default;

    void addPoint(const Vector& a) {
        vectors.push_back(a);
    }

    friend Polygon operator-(Polygon a);

    friend Polygon operator+(Polygon a, Polygon b);

    bool contain(Vector point) {
        std::function<int(Vector a, Vector b, Vector c)> leftTurn = []
                (Vector a, Vector b, Vector c){
            if((b - a).cross(c - a) > 0) {
                return 1;
            }
            else if ((b - a).cross(c - a) < 0) {
                return  -1;
            }
            else {
                return 0;
            }
        };

        for(size_t i = 0; i < vectors.size(); ++i) {
            if(leftTurn(vectors[(i + 1) % vectors.size()], vectors[i], point) < 0) {
                return false;
            }
        }

        return true;
    }
private:
    std::vector<Vector> vectors;

    void m_leftDownToFirst() {
        auto min = vectors.begin();
        for(auto i = vectors.begin(); i != vectors.end(); ++i) {
            if(i->y < min->y) {
                min = i;
            }
            else if(i->y == min->y) {
                if(i->x < min->x) {
                    min = i;
                }
            }
        }
        std::rotate(vectors.begin(), min, vectors.end());
    }
};

Polygon operator-(Polygon a) {
    Polygon res;
    for(auto& v: a.vectors) {
        res.addPoint(-v);
    }
    return res;
}

Polygon operator+(Polygon a, Polygon b) {
    Polygon res;
    size_t it_1 = 0, it_2 = 0;
    a.m_leftDownToFirst();
    b.m_leftDownToFirst();

    while((it_1 < a.vectors.size()) && (it_2 < b.vectors.size())) {
        auto v1 = a.vectors[(it_1 + 1) % a.vectors.size()] - a.vectors[it_1];
        auto v2 = b.vectors[(it_2 + 1) % b.vectors.size()] - b.vectors[it_2];
        res.addPoint(a.vectors[it_1] + b.vectors[it_2]);
        if(v1.cross(v2) < -1e-10) {
            ++it_1;
        }
        else if (v1.cross(v2) > 1e-10){
            ++it_2;
        }
        else {
            ++it_1;
            ++it_2;
        }
    }

    for(; it_1 < a.vectors.size(); ++it_1) {
        res.addPoint(a.vectors[it_1] + b.vectors[0]);
    }

    for(; it_2 < b.vectors.size(); ++it_2) {
        res.addPoint(a.vectors[0] + b.vectors[it_2]);
    }

    return res;
}


int main() {
    Polygon a, b;
    size_t aSize, bSize;

    std::cin>>aSize;
    for(size_t i = 0; i < aSize; ++i) {
        Vector temp;
        std::cin >> temp.x >> temp.y;
        a.addPoint(temp);
    }

    std::cin>>bSize;
    for(size_t i = 0; i < bSize; ++i) {
        Vector temp;
        std::cin >> temp.x >> temp.y;
        b.addPoint(temp);
    }

    b = -b;
    Polygon res = a + b;

    if(res.contain(Vector(0, 0))) {
        std::cout << "Yes" << std::endl;
    }
    else {
        std::cout << "No" << std::endl;
    }

    return 0;
}