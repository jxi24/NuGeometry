#pragma once

#include "geom/Camera.hh"


namespace NuGeom::Interactive {

enum class Direction {
    Forward,
    Backward,
    Right,
    Left,
    Up,
    Down,
};

class MoveableCamera {
    public:
        MoveableCamera(const NuGeom::Camera&);
        MoveableCamera operator+=(const NuGeom::Vector3D&);

        void rotate(double, double);
        void move(Direction);
};

}
