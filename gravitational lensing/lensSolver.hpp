#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <gsl/gsl_integration.h>
#include "math.hpp"

template <typename T>
struct Lens {
    double mass;
    double z;
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

    double einsteinAngle() {
        double D_ls = angularDiameterDistanceBetween(lens.z, source.z);
        double D_s = angularDiameterDistance(source.z);
        double D_l = angularDiameterDistance(lens.z);
        return std::sqrt(4 * G * lens.mass * D_ls / D_s / D_l / 3 / 1e7) / c;
    }

    double magnification(double angle) {
        // auto beta2 = std::pow(beta, 2);
        // return (beta2 + 2) / (2 * beta * std::sqrt(beta2 + 4)) + 0.5
        return 1 / (1 - 1 / std::pow(angle / einstAngle, 4));
    }

public:
    LensSolver(double mass, double z1, double z2, T x, T y): lens{mass, z1, Point<T>(x, y)}, source{z2} {
        // source.z = z2;
        einstAngle = einsteinAngle();
    }
    LensSolver(LensSolver&) = default;
	LensSolver(LensSolver&&) = default;

    std::array<Point<T>, 2> processPoint(Point<T> p, double *magn) {
        auto dp = p - lens.center;
        auto beta2 = dp * dp;
        auto beta = std::sqrt(beta2);
        double angle1 = (beta + std::sqrt(beta2 + 4 * std::pow(einstAngle, 2))) / 2;
        double angle2 = std::abs(beta - std::sqrt(beta2 + 4 * std::pow(einstAngle, 2))) / 2;

        Point<T> imagePos1 = dp * angle1 / beta + lens.center;
        Point<T> imagePos2 = dp * (-angle2) / beta + lens.center;
        std::array<Point<T>, 2> imagePositions = {imagePos1, imagePos2};

        double magn_[2] {magnification(angle1), magnification(angle2)};
        // if (magn_[0] > 1)
            // std::cout << magn_[0] << std::endl;
        *magn = *magn_;
        // if (magn_[0] > 1)
            // std::cout << magn_[0] << std::endl;
        // magn[0] = magnification(angle1);
        // magn[1] = magnification(angle2);
        // static Point<T> imagePositions[2] (imagePos1, imagePos2);
        // imagePositions[0] = imagePos1;
        // imagePositions[1] = imagePos2;
        // std::cout << "a" << std::endl;
        return imagePositions;
    }

    std::array<Point<T>, 2> processPoint(T x, T y, double *magn) {
        return processPoint(Point<T>(x, y), magn);
    }

    void moveLens(T dx, T dy) {
        lens.center += Point<T>(dx, dy);
    }

    void setLensCenter(T x, T y) {
        lens.center = Point<T>(x, y);
        // std::cout << "lens: " << lens.center.x << ' ' << lens.center.y << std::endl;
    }

    double getEinstainAngle() {
        return einstAngle;
    }
};