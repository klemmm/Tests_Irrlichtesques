#ifndef SHIP_H
#define SHIP_H 1
#include "building.h"

class Ship: public Building {
    irr::core::vector3df _velocity;
public:
    inline Ship(irr::scene::ISceneManager *smgr) : Building(smgr) {}

    inline void thrust(irr::core::vector3df accel) {
        _velocity += accel;
        printf("apply thr %f %f %f, vel now %f %f %f\n", accel.X, accel.Y, accel.Z, _velocity.X, _velocity.Y, _velocity.Z);
    }
    inline void brake(float brake_amount) {
        if (_velocity.getLength() <= brake_amount) {
            _velocity = irr::core::vector3df(0, 0, 0);
        } else {
            irr::core::vector3df brake_vector = _velocity;
            brake_vector.normalize();
            brake_vector *= brake_amount;
            _velocity = _velocity - brake_vector;
        }
    }
    virtual void process(float delta);
    virtual bool canMove(void) const { return true; }
};

#endif