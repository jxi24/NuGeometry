#pragma once

#include <cmath>

namespace NuGeom {

template<typename T>
bool is_close(const T &a, const T &b, const T &eps = T(1e-8)) {
    return std::abs(a - b) < eps;
}

}
