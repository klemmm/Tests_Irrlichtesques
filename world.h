#ifndef WORLD_H
#define WORLD_H 1

#include <memory>
#include <irrlicht/irrlicht.h>
#include "kbd_status.h"
#include "building.h"

class World {

public:

    World(irr::scene::ISceneManager*);
    void process(float, IKkbdStatus&, const irr::core::vector2df&);

private:
    void position_and_orient_camera(const irr::core::vector3df &position, const irr::core::quaternion &orientation);

    irr::scene::ISceneManager *_smgr;

    irr::scene::ICameraSceneNode *_camera;

    irr::core::quaternion _camera_orientation;
    irr::core::vector3df _camera_position;

    std::unique_ptr<Building> _testBuilding;


};

#endif