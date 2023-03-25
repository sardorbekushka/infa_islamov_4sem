#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <gsl/gsl_integration.h>
#include "math.hpp"

struct Lens {
    double mass;
    double z;
    Point center;
};

struct Source {
    double z;
};

class LensSolver {
protected:
    Lens lens;
    Source source;
    double einstAngle;

    double einsteinAngle() {
        double D_ls = angularDiameterDistanceBetween(lens.z, source.z);
        double D_s = angularDiameterDistance(source.z);
        double D_l = angularDiameterDistance(lens.z);
        return std::sqrt(4 * G * lens.mass * D_ls / D_s / D_l / 3 / 1e7) / c;
    }

public:
    LensSolver(double mass, double z1, double z2, double x, double y): lens{mass, z1, Point(x, y)}, source{z2} {
        // source.z = z2;
        einstAngle = einsteinAngle();
    }
    LensSolver(LensSolver&) = default;
	LensSolver(LensSolver&&) = default;

    float magnification(double angle) {
        // auto beta2 = std::pow(beta, 2);
        // return (beta2 + 2) / (2 * beta * std::sqrt(beta2 + 4)) + 0.5
        return std::abs(1 / (1 - std::pow(angle / einstAngle, -4)));
    }

    std::array<Point, 2> processPoint(Point p, double *magn) {
        auto dp = p - lens.center;
        auto beta2 = dp * dp;
        auto beta = std::sqrt(beta2);
        double angle1 = (beta + std::sqrt(beta2 + 4 * std::pow(einstAngle, 2))) / 2;
        double angle2 = std::abs(beta - std::sqrt(beta2 + 4 * std::pow(einstAngle, 2))) / 2;
        
        Point imagePos1 = dp * angle1 / beta + lens.center;
        Point imagePos2 = dp * (-angle2) / beta + lens.center;
        std::array<Point, 2> imagePositions = {imagePos1, imagePos2};

        double magn_[2] {magnification(angle1), magnification(angle2)};

        // std::cout << angle2 << ' ' << magn_[1] << std::endl;

        // if (magn_[0] > 1)
            // std::cout << magn_[0] << std::endl;
        *magn = *magn_;
        for (int i = 0; i < 2; i++)
            magn[i] = magn_[i];
        // if (magn_[0] > 1)
            // std::cout << magn[0] << std::endl;
        // magn[0] = magnification(angle1);
        // magn[1] = magnification(angle2);
        // static Point imagePositions[2] (imagePos1, imagePos2);
        // imagePositions[0] = imagePos1;
        // imagePositions[1] = imagePos2;
        // std::cout << "a" << std::endl;
        return imagePositions;
    }

    Point reverseProcessPoint(Point p, float &magn) {
        auto dp = p - lens.center;
        // auto theta = std::sqrt(dp * dp);
        auto theta = dp.norm();
        auto beta = (dp * dp - einstAngle * einstAngle) / theta;
        magn = magnification(theta);
        // std::cout << theta << ' ' << magn << std::endl;
        // double angle = (beta + std::sqrt(beta * beta + 4 * std::pow(einstAngle, 2))) / 2;
        // magn = magnification(angle);
        return dp / theta * beta + lens.center;
    }

    std::array<Point, 2> processPoint(double x, double y, double *magn) {
        return processPoint(Point(x, y), magn);
    }

    Point reverseProcessPoint(double x, double y, float &magn) {
        return reverseProcessPoint(Point(x, y), magn);
    }

    void moveLens(double dx, double dy) {
        lens.center += Point(dx, dy);
    }

    void setLensCenter(double x, double y) {
        lens.center = Point(x, y);
        // std::cout << "lens: " << lens.center.x << ' ' << lens.center.y << std::endl;
    }

    Point getLensCenter() {
        return lens.center;
    }

    double getEinstainAngle() {
        return einstAngle;
    }

    double getMass() {
        return lens.mass;
    }

    double getLensRedshift() {
        return lens.z;
    }

    void updateMass(double k) {
        lens.mass *= std::pow(10, k);
        einstAngle *= std::pow(10, (double)k / 2);
    }

    double getSourceRedshift() {
        return source.z;
    }
};