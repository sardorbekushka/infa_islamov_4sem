#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <gsl/gsl_integration.h>
#include "math.hpp"

template <typename T>
struct Lens {
    double mass;
    double z;
    double velocityDispersion;
    Point<T> center;
};

struct Source {
    double z;
};

template <typename T>
class LensSolver {
protected:
    Lens<T> lens;
    Source source;
    double einstAngle;

    std::array<Point<T>, 2> processPoint(Point<T> p) {
        auto dp = p - lens.center;
        auto beta = dp * dp;
        double angle1 = (beta + std::sqrt(std::pow(beta, 2) + 4 * std::pow(einstAngle, 2))) / 2;
        double angle2 = std::abs(beta - std::sqrt(std::pow(beta, 2) + 4 * std::pow(einstAngle, 2))) / 2;

        Point<T> imagePos1 = dp * angle1 / beta + lens.center;
        Point<T> imagePos2 = dp * (-angle1) / beta + lens.center;
        std::array<Point<T>, 2> imagePositions = {imagePos1, imagePos2};
        // static Point<T> imagePositions[2] (imagePos1, imagePos2);
        // imagePositions[0] = imagePos1;
        // imagePositions[1] = imagePos2;
        // std::cout << "a" << std::endl;
        return imagePositions;
    }

    double einsteinAngle() {
        double D_ls = angularDiameterDistanceBetween(lens.z, source.z);
        double D_s = angularDiameterDistance(source.z);
        return std::sqrt(4 * pi * std::pow(lens.velocityDispersion / c, 2) * D_ls / D_s * std::pow(10, 6));
    }

public:
    LensSolver(double mass, double z1, double z2, double velocityDispersion, T x, T y): lens{mass, z1, velocityDispersion, Point<T>(x, y)}, source{z2} {
        // source.z = z2;
        einstAngle = einsteinAngle();
    }
    LensSolver(LensSolver&) = default;
	LensSolver(LensSolver&&) = default;

    std::array<Point<T>, 2> processPoint(T x, T y) {
        return processPoint(Point<T>(x, y));
    }

    void moveLens(T dx, T dy) {
        lens.center += Point<T>(dx, dy);
    }

    void setLensCenter(T x, T y) {
        lens.center = Point<T>(x, y);
    }
};