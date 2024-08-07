#include <iostream>
#include <vector>
#include <cmath>

struct Point3D
{
    double x, y, z;
};

// 计算两点之间的距离
double distance(const Point3D& p1, const Point3D& p2)
{
    // 使用欧几里得距离公式：sqrt((x2 - x1)^2 + (y2 - y1)^2 + (z2 - z1)^2)
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y) +
        (p1.z - p2.z) * (p1.z - p2.z));
}

// 计算向量的叉积
Point3D crossProduct(const Point3D& v1, const Point3D& v2)
{
    Point3D result;
    result.x = v1.y * v2.z - v1.z * v2.y; // x = (y1 * z2 - z1 * y2)
    result.y = v1.z * v2.x - v1.x * v2.z; // y = (z1 * x2 - x1 * z2)
    result.z = v1.x * v2.y - v1.y * v2.x; // z = (x1 * y2 - y1 * x2)
    return result;
}

// 计算点的向量
Point3D vectorFromPoints(const Point3D& p1, const Point3D& p2)
{
    Point3D result;
    result.x = p2.x - p1.x; // x = x2 - x1
    result.y = p2.y - p1.y; // y = y2 - y1
    result.z = p2.z - p1.z; // z = z2 - z1
    return result;
}

// 计算点积
double dotProduct(const Point3D& v1, const Point3D& v2)
{
    // 点积公式：v1 · v2 = (x1 * x2) + (y1 * y2) + (z1 * z2)
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 计算平面方程的参数
std::vector<double> calculatePlaneEquation(const Point3D& N, const Point3D& O, const Point3D& O1)
{
    // 使用叉积得到法向量，然后利用法向量和一点构建平面方程 Ax + By + Cz + D = 0
    std::vector<double> planeEquation(4);
    Point3D v1 = vectorFromPoints(O, N);
    Point3D v2 = vectorFromPoints(O, O1);
    Point3D normal = crossProduct(v1, v2);
    planeEquation[0] = normal.x;
    planeEquation[1] = normal.y;
    planeEquation[2] = normal.z;
    planeEquation[3] = -1 * (normal.x * O.x + normal.y * O.y + normal.z * O.z); // Ax + By + Cz + D = 0
    return planeEquation;
}

// 求解线与平面的交点
Point3D findIntersection(const Point3D& T, const Point3D& N, const Point3D& O, const Point3D& O1)
{
    // 首先计算平面方程，然后利用参数方程求解交点
    std::vector<double> planeEquation = calculatePlaneEquation(N, O, O1);
    double a = planeEquation[0];
    double b = planeEquation[1];
    double c = planeEquation[2];
    double d = planeEquation[3];
    double t = (-a * T.x - b * T.y - c * T.z - d) / (a * a + b * b + c * c);
    Point3D T1;
    T1.x = T.x + a * t; // x = x0 + at
    T1.y = T.y + b * t; // y = y0 + bt
    T1.z = T.z + c * t; // z = z0 + ct
    return T1;
}

int main()
{
    // 示例用法
    Point3D O = { 0, 0, 0 }; // 平面上的一点
    Point3D N = { 1, 1, 1 }; // 平面的法向量
    Point3D O1 = { 1, 2, 3 }; // 平面上的另一点
    Point3D T = { 1, 2, 3 }; // 线上的一点
    Point3D T1 = findIntersection(T, N, O, O1); // 计算线与平面的交点
    std::cout << "Intersection point T1: (" << T1.x << ", " << T1.y << ", " << T1.z << ")" << std::endl;
    return 0;
}
