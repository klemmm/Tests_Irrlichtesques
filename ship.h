#ifndef SHIP_H
#define SHIP_H 1
#include "building.h"

class Ship: public Building {
    vector3dfp _velocity;
public:
    inline Ship(irr::scene::ISceneManager *smgr) : Building(smgr) {}

    inline void thrust(vector3dfp accel) {
        _velocity += accel;
        //printf("apply thr %f %f %f, vel now %f %f %f\n", accel.X, accel.Y, accel.Z, _velocity.X, _velocity.Y, _velocity.Z);
    }
    inline void brake(float brake_amount) {
        if (_velocity.getLength() <= brake_amount) {
            _velocity = vector3dfp(0, 0, 0);
        } else {
            vector3dfp brake_vector = _velocity;
            //printf("1. braking by %f %f %f\n", (double)_velocity.X, (double)_velocity.Y, (double)_velocity.Z);
            //printf("2. braking by %f %f %f\n", (double)brake_vector.X, (double)brake_vector.Y, (double)brake_vector.Z);

            brake_vector.normalize();
            //brake_vector = brake_vector / brake_vector.getLength();
            //printf("3. braking by %f %f %f\n", (double)brake_vector.X, (double)brake_vector.Y, (double)brake_vector.Z);

            brake_vector *= brake_amount;
            //printf("4. braking by %f %f %f\n", (double)brake_vector.X, (double)brake_vector.Y, (double)brake_vector.Z);
            _velocity = _velocity - brake_vector;
        }
        
    }
    virtual void process(float delta);
    virtual bool canMove(void) const { return true; }
};

#endif