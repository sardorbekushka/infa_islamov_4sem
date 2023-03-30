#pragma once 

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
    /**
     * @param mass mass of the lensing object (gravitational lens)
     * @param z1 redshift of the lens
     * @param z2 redshift of the source
     * @param x initial horizontal coordinate of the lens
     * @param y initial vertical coordinate of the lens
    */
    LensSolver(double mass, double z1, double z2, double x=0, double y=0): lens{mass, z1, Point(x, y)}, source{z2} {
        einstAngle = einsteinAngle();
    }
    LensSolver(LensSolver&) = default;
	LensSolver(LensSolver&&) = default;

    float magnification(double angle) {
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

        *magn = *magn_;
        for (int i = 0; i < 2; i++)
            magn[i] = magn_[i];
        return imagePositions;
    }

    std::array<Point, 2> processPoint(double x, double y, double *magn) {
        return processPoint(Point(x, y), magn);
    }

    Point reverseProcessPoint(Point p, float &magn) {
        auto dp = p - lens.center;
        auto theta = dp.norm();
        auto beta = (dp * dp - einstAngle * einstAngle) / theta;
        magn = magnification(theta);
        return dp / theta * beta + lens.center;
    }

    Point reverseProcessPoint(double x, double y, float &magn) {
        return reverseProcessPoint(Point(x, y), magn);
    }

    void moveLens(double dx, double dy) {
        lens.center += Point(dx, dy);
    }

    void setLensCenter(double x, double y) {
        lens.center = Point(x, y);
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

    double getSourceRedshift() {
        return source.z;
    }

    void updateMass(double k) {
        lens.mass *= std::pow(10, k);
        einstAngle *= std::pow(10, (double)k / 2);
    }
};