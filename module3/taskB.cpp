#include <cmath>
#include <array>
#include <algorithm>
#include <set>
#include <queue>
#include <vector>
#include <iostream>

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


class GiftWrap {
public:
    GiftWrap() = default;
    explicit GiftWrap(std::vector<Vector>& _points): points(_points), iternalPoint() {
        for(auto p: points) {
            iternalPoint = iternalPoint + p;
        }
        iternalPoint = iternalPoint / points.size();
    }

    void run() {
        auto triangle = m_findTriangleOnHull();
        triangle = m_makeRightTriangle(triangle);
        std::queue<std::pair<size_t,  size_t>> q;

        for(int i = 0; i < 3; ++i) {
            q.emplace(triangle[(i + 1) % 3], triangle[i]);
        }

        coverPoints.insert(triangle);
        std::vector<std::vector<bool>> checked(points.size(), std::vector<bool>(points.size()));

        while(!q.empty()) {
            auto e = q.front();
            q.pop();
            if(!checked[std::get<0>(e)][std::get<1>(e)]) {
                auto pivot = m_pivotAroundEdge({std::get<0>(e), std::get<1>(e)});
                triangle = {std::get<0>(e), std::get<1>(e), pivot};
                triangle = m_makeRightTriangle(triangle);
                coverPoints.insert(triangle);

                for(int i = 0; i < 3; ++i) {
                    if(!checked[triangle[(i + 1) % 3]][triangle[i]]) {
                        q.emplace(triangle[(i + 1) % 3], triangle[i]);
                    }
                }

                checked[std::get<0>(e)][std::get<1>(e)] = true;
            }
        }
    }

    std::vector<std::array<size_t, 3>> answer() const {
        std::vector<std::array<size_t, 3>> answer(coverPoints.size());
        auto it = coverPoints.begin();
        for(auto& a: answer) {
            for(int i = 0; i < 3; ++i) {
                a[i] = (*it)[i];
            }
            ++it;
        }
        return answer;
    }
private:
    Vector iternalPoint;
    std::vector<Vector> points;
    std::set<std::array<size_t, 3>> coverPoints;

    size_t m_pivotAroundEdge(std::array<size_t, 2> edge) {
        size_t p = 0;
        while(p == edge[0] || p == edge[1]) {
            ++p;
        }
        for(size_t i = 0; i < points.size(); ++i) {
            if(i == edge[0] || i == edge[1]) {
                continue;
            }
            double volume = (points[i] - points[edge[0]]).triple(points[edge[1]] - points[edge[0]],
                    points[p] - points[edge[0]]);
            if(volume > 0) {
                p = i;
            }
        }
        return p;
    }

    std::array<size_t, 2> m_findEdgeOnHull() {
        auto p = m_bottomLeftBack();

        size_t q = 0;
        if(q == p) {
            ++q;
        }
        for(size_t i = 0; i < points.size(); ++i) {
            if(i == p) {
                continue;
            }
            double volume = (points[i] - points[p]).triple(Vector(0, 0, 1), points[q] - points[p]);
            if(volume > 0) {
                q = i;
            }
        }

        return {p, q};
    }

    std::array<size_t, 3> m_findTriangleOnHull() {
        auto pq = m_findEdgeOnHull();
        auto r = m_pivotAroundEdge(pq);
        return {pq[0], pq[1], r};
    }

    size_t m_bottomLeftBack() {
        size_t min = 0;

        for(size_t i = 0; i < points.size(); ++i) {
            if(points[i].x < points[min].x) {
                min = i;
            }
        }

        return min;
    }

    std::array<size_t, 3> m_makeRightTriangle(std::array<size_t, 3> triangle) {
        Vector d = iternalPoint - points[triangle[0]];
        if(d.triple(points[triangle[1]] - points[triangle[0]], points[triangle[2]] - points[triangle[0]]) > 0) {
            std::reverse(triangle.begin(), triangle.end());
        }

        auto min = triangle.begin();
        for(auto i = triangle.begin(); i != triangle.end(); ++i) {
            if(*min > *i) {
                min = i;
            }
        }
        std::rotate(triangle.begin(), min, triangle.end());

        return triangle;
    }
};

int main() {
    size_t m = 1, numPoints;
    std::vector<Vector> points;

    std::cin >> m;
    for(size_t numTest = 0; numTest < m; ++numTest) {
        std::cin >> numPoints;
        points.assign(numPoints, Vector(0, 0, 0));
        for(auto& point: points) {
            std::cin >> point.x >> point.y >> point.z;
        }

        GiftWrap algorithm(points);
        algorithm.run();
        auto answer = algorithm.answer();
        std::printf("%ld\n", answer.size());
        for(auto face: answer) {
            std::cout << "3 " << face[0] << " " << face[1] << " " << face[2] << std::endl;
        }

        points.clear();
    }
}
