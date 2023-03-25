#include <iostream>
#include <cmath>
#include "constants.hpp"
#include <gsl/gsl_integration.h>


double hubbleConstant(double z, void *params) {
    double alpha = *(double *) params;
    return 1 / (H0 * std::sqrt(omegaM * std::pow(1 + z, alpha)) + omegaL);
}

double angularDiameterDistance(double z) {
    gsl_integration_workspace * w = gsl_integration_workspace_alloc(1000);
    double result, error;
    double alpha = 3.0;

    gsl_function F;
    F.function = &hubbleConstant;
    F.params = &alpha;
    
    gsl_integration_qags(&F, 0.0, z, 0.0, 1e-7, 1000, w, &result, &error);

    return c / (1 + z) * result;
}

double angularDiameterDistanceBetween(double z1, double z2) {
    return angularDiameterDistance(z2) - (1 + z1) / (1 + z2) * angularDiameterDistance(z1);
}

struct Point {
    double x, y;

    Point(double x, double y): x(x), y(y) {}
    Point(const Point& rhs) = default;
    Point& operator=(const Point& rhs) = default;
    Point() = delete;

    Point operator + (const Point &dp) {
        return Point(x + dp.x, y + dp.y);
    }

    Point operator += (const Point &dp) {
        *this = *this + Point(dp.x, dp.y);
        return *this;
    }

    Point operator - (const Point &dp) {
        return Point(x - dp.x, y - dp.y);
    }

    Point operator -= (const Point &dp) {
        *this = *this - Point(dp.x, dp.y);
        return *this;
    }

    double operator * (const Point &dp) {
        return (x * dp.x + y * dp.y);
    }

    Point operator * (const double numerator) {
        return Point(x * numerator, y * numerator);
    }

    Point operator / (const double &denominator) {
        return Point(x / denominator, y / denominator);
    }
    
    Point operator /= (const double &denominator) {
        *this = *this / denominator;
        return *this;
    }

    double norm() {
        return std::sqrt(x * x + y * y);
    }
};
