#include <irrlicht/irrlicht.h>

#include "assets.h"
#include "world.h"
#include "transforms.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#define ROTATION_SPEED 1.0 /* radian/sec */
#define SPEED 500.0 /* irrlicht space unit/sec */
#define MOUSE_SENSITIVITY 1.0

World::World (irr::scene::ISceneManager *smgr) : _smgr(smgr) {
    _camera = _smgr->addCameraSceneNode();
    _camera->setFarValue(10000);

    _testBuilding = std::unique_ptr<Building>(new Building(_smgr));
    for (int k = 20; k > 10; k = k - 2) {
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                _testBuilding->addBlock(vector3di(i, j, k/2), vector2di(1 - ((i == 0) || (i == k-1) || (j == 0) || (j == k-1)), 0));
            }
        }
    }
    
    
    _testBuilding->updateMesh(TextureId::DEFAULT);
    
}

void World::position_and_orient_camera(const vector3df &position, const quaternion &orientation) {
        auto forward = vector3df(0, 0, 1);
        auto up = vector3df(0, 1, 0);

        Transforms::rotate(forward, orientation);
        Transforms::rotate(up, orientation); 


        _camera->setPosition(position);
        _camera->setTarget(position + forward);
        _camera->setUpVector(up);
}

void World::process(float delta, IKkbdStatus &kbd, const vector2df& mouse_movement) {
    vector3df movement;
    quaternion rotation;

    if (kbd.isKeyDown(irr::KEY_KEY_Z)) {
        movement.Z += delta * SPEED;
    }
    if (kbd.isKeyDown(irr::KEY_KEY_S)) {
        movement.Z -= delta * SPEED;
    }      
    if (kbd.isKeyDown(irr::KEY_KEY_Q)) {
        movement.X -= delta * SPEED;
    } 
    if (kbd.isKeyDown(irr::KEY_KEY_D)) {
        movement.X += delta * SPEED;
    }    
    if (kbd.isKeyDown(irr::KEY_SPACE)) {
        movement.Y += delta * SPEED;
    }  
    if (kbd.isKeyDown(irr::KEY_KEY_C)) {
        movement.Y -= delta * SPEED;
    }

    if (mouse_movement.X < 0.5f) {
        rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.X), vector3df(0.0, 1.0, 0.0));
    }  
    if (mouse_movement.X > 0.5f) {
        rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.X), vector3df(0.0, 1.0, 0.0));
    } 
    _camera_orientation = _camera_orientation * rotation;

    if (mouse_movement.Y < 0.5f) {
        rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.Y), vector3df(1.0, 0.0, 0.0));
    }  
    if (mouse_movement.Y > 0.5f) {
        rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.Y), vector3df(1.0, 0.0, 0.0));
    }     
    _camera_orientation = _camera_orientation * rotation;    

    if (kbd.isKeyDown(irr::KEY_KEY_A)) {
        rotation.fromAngleAxis(-ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
    }  

    if (kbd.isKeyDown(irr::KEY_KEY_E)) {
        rotation.fromAngleAxis(ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
    }  
    _camera_orientation = _camera_orientation * rotation;    
            
    Transforms::rotate(movement, _camera_orientation);  
    _camera_position = _camera_position + movement;

    position_and_orient_camera(_camera_position, _camera_orientation);
}
