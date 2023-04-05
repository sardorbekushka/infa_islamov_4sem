#pragma once 

#include <iostream>
#include <array>
#include "math.hpp"

struct Lens {
    double mass;                    // mass of the lens in kg
    double z;                       // redshift of the lens
    Point center;                   // center of the lens in radians
};

struct Source {
    double z;                       // redshift of the source
};

class LensSolver {
protected:
    Lens lens;                      // lens in the system
    Source source;                  // source in the system
    double einstAngle;              // einstein angle of the system in radians

    /**
     * @return calculated einstein angle for the system in radians
    */
    double einsteinAngle() {
        double D_ls = angularDiameterDistanceBetween(lens.z, source.z);
        double D_s = angularDiameterDistance(source.z);
        double D_l = angularDiameterDistance(lens.z);
        return std::sqrt(4 * G0 * lens.mass * D_ls / D_s / D_l / 3 / 1e7) / c0;
    }

public:
    /**
     * @param mass mass of the lensing object (gravitational lens) in kg
     * @param z1 redshift of the lens
     * @param z2 redshift of the source
     * @param x initial horizontal coordinate of the lens in radians
     * @param y initial vertical coordinate of the lens in radians
    */
    LensSolver(double mass, double z1, double z2, double x=0, double y=0): lens{mass, z1, Point(x, y)}, source{z2} {
        einstAngle = einsteinAngle();
    }
    LensSolver(LensSolver&) = default;
	LensSolver(LensSolver&&) = default;

    /**
	 * magnification value at this angle
     * 
     * @param angle the angle in radians where is the point about the lens center
     * 
     * @return the magnification (absolute value)
	*/
    float magnification(double angle) {
        return std::abs(1 / (1 - std::pow(angle / einstAngle, -4)));
    }

    /**
	 * processes the point in straight way. the point splits to the calculated positions
     * 
     * @param[in] p the point will be refracted
     * @param[out] magn pointer to the array where two magnification values of images will be set
     * 
     * @return two refracted points
	*/
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

    /**
	 * processes the point in straight way. the point splits to the calculated positions
     * 
     * @overload
	*/
    std::array<Point, 2> processPoint(double x, double y, double *magn) {
        return processPoint(Point(x, y), magn);
    }

    /**
	 * processes the point in reverse way. for the point is calculated where is the origin position in the source
     * 
     * @param[in] p the refracted position og the point
     * @param[out] magn reference to the magn where the magnification value will be set
     * 
     * @return position of the original point
	*/
    Point reverseProcessPoint(Point p, float &magn) {
        auto dp = p - lens.center;
        auto theta = dp.norm();
        auto beta = (dp * dp - einstAngle * einstAngle) / theta;
        magn = magnification(theta);
        return dp / theta * beta + lens.center;
    }

    /**
	 * processes the point in reverse way. for the point is calculated where is the origin position in the source
     * 
     * @overload
	*/
    Point reverseProcessPoint(double x, double y, float &magn) {
        return reverseProcessPoint(Point(x, y), magn);
    }

    /**
     * moves the lens
     * 
     * @param dx the horizontal move in radians
     * @param dy the vertical move in radians
    */
    void moveLens(double dx, double dy) {
        lens.center += Point(dx, dy);
    }

    /**
     * sets the lens center to the point (x, y)
     * 
     * @param x the horizontal coordinate of center in radians
     * @param y the vertical coordinate of center in radians
    */
    void setLensCenter(double x, double y) {
        lens.center = Point(x, y);
    }

    /**
     * @return the lens center in radians
    */
    Point getLensCenter() {
        return lens.center;
    }

    /**
     * @return einstein angle of the system in radians
    */
    double getEinstainAngle() {
        return einstAngle;
    }

    /**
     * @return the lens mass in kg
    */
    double getMass() {
        return lens.mass;
    }

    /**
     * @return redshift of the lens
    */
    double getLensRedshift() {
        return lens.z;
    }

    /**
     * @return redshift of the source
    */
    double getSourceRedshift() {
        return source.z;
    }

    /**
     * increases or decreases lens mass
     * 
     * @param k mass change parameter (newMass = 10^k * oldMass)
    */
    void updateMass(double k) {
        lens.mass *= std::pow(10, k);
        einstAngle *= std::pow(10, (double)k / 2);
    }
};