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

template <typename T>
struct Point {
    T x, y;

    Point(T x, T y): x(x), y(y) {}
    Point(const Point& rhs) = default;
    Point& operator=(const Point& rhs) = default;
    Point() = delete;

    Point<T> operator + (const Point<T> &dp) {
        return Point<T>(x + dp.x, y + dp.y);
    }

    Point<T> operator += (const Point<T> &dp) {
        *this = *this + Point<T>(dp.x, dp.y);
        return *this;
    }

    Point<T> operator - (const Point<T> &dp) {
        return Point<T>(x - dp.x, y - dp.y);
    }

    Point<T> operator -= (const Point<T> &dp) {
        *this = *this - Point<T>(dp.x, dp.y);
        return *this;
    }

    T operator * (const Point<T> &dp) {
        return (x * dp.x + y * dp.y);
    }

    Point<T> operator * (const T numerator) {
        return Point<T>(x * numerator, y * numerator);
    }

    Point<double> operator / (const double &denominator) {
        return Point<T>(x / denominator, y / denominator);
    }
    
    Point<double> operator /= (const double &denominator) {
        *this = *this / denominator;
        return *this;
    }

    T norm() {
        return std::sqrt(x * x + y * y);
    }
};
