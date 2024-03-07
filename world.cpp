class fpnum;

#include "common.h"

#include <unordered_set>

#include "assets.h"
#include "world.h"
#include "transforms.h"
#include "constants.h"
#include "ship.h"
#include "hud.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#define ROTATION_SPEED 1.0 /* radian/sec */
#define SPEED 500.0 /* irrlicht space unit/sec */
#define MOUSE_SENSITIVITY 1.0

#define FORCE 500.0
#define MAX_ROTATION 1.0 /* radian/sec */



World::World (irr::scene::ISceneManager *smgr) : _smgr(smgr), _buildings(vector3dfp(-Constants::WORLD_SIZE, -Constants::WORLD_SIZE, -Constants::WORLD_SIZE), vector3dfp(Constants::WORLD_SIZE, Constants::WORLD_SIZE, Constants::WORLD_SIZE)),_wasLeftPressed(false), _wasRightPressed(false), _wasKeyPressed(false), _invOpen(false), _held(0), _piloting(false) {
    _camera = _smgr->addCameraSceneNode();
    _camera->setFarValue(50000);

    
    
    std::shared_ptr<Ship> testShip1 = std::shared_ptr<Ship>(new Ship(_smgr));
    int fd = open("ship.dat", O_RDONLY);
    if (fd != -1) {
        irr::s32 X,Y,Z;
        irr::s32 tx, ty;

        while (read(fd, (void*) &X, sizeof(irr::s32)) != 0) {
            read(fd, (void*) &Y, sizeof(irr::s32));
            read(fd, (void*) &Z, sizeof(irr::s32));
            read(fd, (void*) &tx, sizeof(irr::s32));
            read(fd, (void*) &ty, sizeof(irr::s32));
            
            testShip1->addBlock(vector3di(X, Y, Z), vector2di(tx, ty));
        }
        close(fd);

    } else {
    
        for (int i = 0; i < 3; i++)
            testShip1->addBlock(vector3di(0, 0, i), vector2di(0, 0));
    }
    
    testShip1->updateMesh(TextureId::DEFAULT);

    _buildings.insert(testShip1);
    
    

    std::shared_ptr<Building> testShip2 = std::shared_ptr<Building>(new Building(_smgr));

    for (int i = 0; i < 4; i++)
        testShip2->addBlock(vector3di(0, i, 0), vector2di(0, 1));
    
    testShip2->setPosition(vector3dfp(500, 500, 500));
    testShip2->setOrientation(quaternion(45, 45, 45));
    testShip2->updateMesh(TextureId::DEFAULT);
    _buildings.insert(testShip2);
    
    
    
    _me = testShip1;

    scene::ISceneNode* skybox = _smgr->addSkyBoxSceneNode(
        TextureLoader::get(SKYBOX_UP),
        TextureLoader::get(SKYBOX_DN),
        TextureLoader::get(SKYBOX_LT),
        TextureLoader::get(SKYBOX_RT),
        TextureLoader::get(SKYBOX_FT),
        TextureLoader::get(SKYBOX_BK));
  
}

void World::position_and_orient_camera(const vector3dfp &position, const quaternion &orientation) {
        auto forward = vector3df(0, 0, 100);
        auto up = vector3df(0, 1, 0);

        Transforms::rotate(forward, orientation);
        Transforms::rotate(up, orientation); 


        _camera->setPosition(irr::core::vector3df(position.X, position.Y, position.Z));
        _camera->setTarget(irr::core::vector3df(position.X, position.Y, position.Z) + forward);
        _camera->setUpVector(up);
}

void World::process(float delta, Hud &hud, IKkbdStatus &kbd, const vector2df& mouse_movement) {

    char buf[256];
    char lastbuf[256];
    auto pos = _camera->getPosition();
    bool onship = (_boarded.lock() != nullptr);

    static int bite = 0;

    bite = (bite + 1) % 8;
    bool bonk = false;
    
    OctreeNodeIterator octiterX = OctreeNodeIterator(_buildings);

    while (std::shared_ptr<Building> buildingX = std::static_pointer_cast<Building>(octiterX.next())) {
        OctreeNodeIterator octiterY = OctreeNodeIterator(_buildings);
        while (std::shared_ptr<Building> buildingY = std::static_pointer_cast<Building>(octiterY.next())) {
            if (buildingX != buildingY) {
                bonk |= buildingX->bonk(buildingY);
            }
        }
    }

    if (bite == 0) {
        snprintf(buf, 256, " %s | Pos: %.2f,%.2f,%.2f | %s%s |",  bonk ? "BONK !!!" : "", pos.X, pos.Y, pos.Z,  !onship ? "Not on any ship." : "On a ship.", onship ? (_piloting ? " (Piloting)" : " (Passenger)") : "");
        if (strcmp(buf, lastbuf)) {
            hud.setStatus(buf);
            strcpy(lastbuf, buf);
        }
    }

        vector3di block_coords, adjacent_block_coords;
    vector3di tmp_block_coords, tmp_adjacent_block_coords;

    core::line3d<f32> ray;
    ray.start = _camera->getPosition();
    ray.end = ray.start + (_camera->getTarget() - ray.start).normalize() * 1000.0f;

    float distance = FLT_MAX;
    float cur_dist;
    std::shared_ptr<Building> collisionBuilding = nullptr;
    
    vector3dfp radius = vector3dfp(10000, 10000, 10000);
    irr::core::vector3df real_camera_pos = _camera->getPosition();
    vector3dfp tmp_pos = vector3dfp(real_camera_pos.X, real_camera_pos.Y, real_camera_pos.Z);
    vector3dfp corner1 = tmp_pos - radius;
    vector3dfp corner2 = tmp_pos + radius;
    OctreeBoundedNodeIterator octiter = OctreeBoundedNodeIterator(_buildings, corner1, corner2);
    
    while (std::shared_ptr<Building> building = std::static_pointer_cast<Building>(octiter.next())) {
        
        if (building->getCollisionCoords(ray, cur_dist, tmp_block_coords, tmp_adjacent_block_coords)) {
            if (cur_dist < distance) {
                distance = cur_dist;
                block_coords = tmp_block_coords;
                adjacent_block_coords = tmp_adjacent_block_coords;
                collisionBuilding = building;                
            }
        }
    }



    OctreeNodeIterator octiter1 = OctreeNodeIterator(_buildings);



    
    vector3dfp movement;
    quaternion rotation;

    if (!_invOpen) {

        if (_piloting) {
            if (auto ship = _boarded.lock()) {
                vector3dfp accel;
                if (kbd.isKeyDown(irr::KEY_KEY_Z)) {
                    accel.Z += delta * FORCE;
                }
                if (kbd.isKeyDown(irr::KEY_KEY_S)) {
                    accel.Z -= delta * FORCE;
                }      
                if (kbd.isKeyDown(irr::KEY_KEY_Q)) {
                    accel.X -= delta * FORCE;
                } 
                if (kbd.isKeyDown(irr::KEY_KEY_D)) {
                    accel.X += delta * FORCE;
                }    
                if (kbd.isKeyDown(irr::KEY_SPACE)) {
                    accel.Y += delta * FORCE;
                }  
                if (kbd.isKeyDown(irr::KEY_KEY_C)) {
                    accel.Y -= delta * FORCE;
                }
                if (ship->canMove()) {
                    Transforms::rotate(accel, ship->getOrientation());  
                    (std::dynamic_pointer_cast<Ship>(ship))->thrust(accel);
                }

                if (kbd.isKeyDown(irr::KEY_LSHIFT)) {
                    (std::dynamic_pointer_cast<Ship>(ship))->brake(delta*FORCE);
                }

            }        
        } else {
            /* Process movements */
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

        }


        if (_piloting) {
            if (auto ship = _boarded.lock()) {
                quaternion ship_orientation = ship->getOrientation();
                quaternion ship_rotation;

                float rotation_cap = MAX_ROTATION*delta;
                float rotate_x = MOUSE_SENSITIVITY*(0.5f - mouse_movement.X);
                float rotate_y = MOUSE_SENSITIVITY*(0.5f - mouse_movement.Y);

                if (rotate_x > rotation_cap) rotate_x = rotation_cap;
                if (rotate_x < -rotation_cap) rotate_x = -rotation_cap;
                if (rotate_y > rotation_cap) rotate_y = rotation_cap;
                if (rotate_y < -rotation_cap) rotate_y = -rotation_cap;

                ship_rotation.fromAngleAxis(rotate_x, vector3df(0.0, 1.0, 0.0));
                ship_orientation = ship_orientation * ship_rotation;

                ship_rotation.fromAngleAxis(rotate_y, vector3df(1.0, 0.0, 0.0));
                ship_orientation = ship_orientation * ship_rotation;
                
                if (kbd.isKeyDown(irr::KEY_KEY_A)) {
                    ship_rotation.fromAngleAxis(-ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
                }  

                if (kbd.isKeyDown(irr::KEY_KEY_E)) {
                    ship_rotation.fromAngleAxis(ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
                }  
                ship_orientation = ship_orientation * ship_rotation;
                ship->setOrientation(ship_orientation); 
            }     
        } else {
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
        }
    }


    // PROCESS BUILDINGS
    std::vector<std::pair<std::shared_ptr<Building>, vector3dfp> > moved;
    while (std::shared_ptr<Building> building = std::static_pointer_cast<Building>(octiter1.next())) {
        vector3dfp old_pos = building->getPosition();
        building->process(delta);
        Octree *octant = octiter1.getCurrent();
        if (!octant->belongsHere(building->getPosition())) {
            moved.push_back(std::pair<std::shared_ptr<Building>, vector3dfp>(building, old_pos));
            // printf("MOOOOOOOOOOOOOOOVE %f %f %f ; %f %f %f -> %f %f %f\n", 
            // building->getPosition().X , building->getPosition().Y, building->getPosition().Z,
            // octant->_corner1.X, octant->_corner1.Y, octant->_corner1.Z,
            // octant->_corner2.X, octant->_corner2.Y, octant->_corner2.Z);
        }
    }

    for (auto iter = moved.begin(); iter != moved.end(); iter++) {
        std::shared_ptr<Building> tomove = (*iter).first;

        _buildings.erase_if([tomove](std::shared_ptr<OctreeNode> n) { 
            return std::static_pointer_cast<Building>(n) == tomove; });       
        
        _buildings.insert(tomove);
    }





    _camera_orientation = _camera_orientation * rotation;
    Transforms::rotate(movement, _camera_orientation);  
    _camera_position = _camera_position + movement;

    if (auto ship = _boarded.lock()) {
        vector3dfp rotated_position = _camera_position;
        Transforms::rotate(rotated_position, ship->getOrientation());
        position_and_orient_camera(rotated_position + ship->getPosition(), ship->getOrientation() * _camera_orientation);
    } else {
        position_and_orient_camera(_camera_position, _camera_orientation);
    }


    /* Process collision */

        
    if (!kbd.isLeftPressed() && _wasLeftPressed) {
        _wasLeftPressed = false;
    }

    if (!kbd.isRightPressed() && _wasRightPressed) {
        _wasRightPressed = false;
    }

    std::unordered_set<std::shared_ptr<Building> > meshNeedsUpdate;



    std::shared_ptr<Building> new_hilighted_building = _hilighted_building.lock();
    irr::core::vector3di new_hilighted_block = _hilighted_block;



    if (collisionBuilding) {
        new_hilighted_building = collisionBuilding;
        new_hilighted_block = block_coords;
    } else {
        new_hilighted_building = nullptr;
    }

    auto _hilighted_building_locked = _hilighted_building.lock();

    
    if (_hilighted_building_locked)
        _hilighted_building_locked->removeHilight();
    
    if (collisionBuilding)
        collisionBuilding->hilightBlock(new_hilighted_block);
    
    if (_hilighted_building_locked != new_hilighted_building) {
        if (_hilighted_building_locked)
            meshNeedsUpdate.insert(_hilighted_building_locked);
        if (collisionBuilding)
            meshNeedsUpdate.insert(collisionBuilding);            
    } else if (_hilighted_block != new_hilighted_block) {
        if (collisionBuilding)
            meshNeedsUpdate.insert(collisionBuilding);
    }
    _hilighted_building = new_hilighted_building;
    _hilighted_block = new_hilighted_block;
        
    if (kbd.isLeftPressed() && !_wasLeftPressed) {
        _wasLeftPressed = true;
        if (collisionBuilding) {
            meshNeedsUpdate.insert(collisionBuilding);
            collisionBuilding->delBlock(block_coords);
        }        
    }

    if (kbd.isRightPressed() && !_wasRightPressed) {
        _wasRightPressed = true;
        if (collisionBuilding) {
            meshNeedsUpdate.insert(collisionBuilding);
            collisionBuilding->addBlock(adjacent_block_coords, vector2di(_held/3, _held%3));
        }        
    }

    for (auto iter = meshNeedsUpdate.begin(); iter != meshNeedsUpdate.end(); iter++) {
        (*iter)->updateMesh(TextureId::DEFAULT);
    }

    if (kbd.isKeyDown(irr::KEY_LEFT)) {
        if (!_wasKeyPressed) {
            _held = (_held - 1);
            if (_held == -1) _held = 8;
            hud.barSelect(_held);
        }
        _wasKeyPressed = true;

    } else if (kbd.isKeyDown(irr::KEY_RIGHT)) {
        if (!_wasKeyPressed) {
            _held = _held + 1;
            if (_held == 9) _held = 0;
            hud.barSelect(_held);

        }
        _wasKeyPressed = true;
    } else if (kbd.isKeyDown(irr::KEY_KEY_I)) {
        if (!_wasKeyPressed) {
            _invOpen = !_invOpen;
            hud.setInventory(_invOpen);
        }
        _wasKeyPressed = true;
    } else if (kbd.isKeyDown(irr::KEY_KEY_B)) {
        if (!_wasKeyPressed) {
            auto ship = _boarded.lock();
            if (ship  == nullptr && collisionBuilding) {
                /* board */
                _boarded = collisionBuilding;
                quaternion inv_ship_rot = collisionBuilding->getOrientation();
                inv_ship_rot.makeInverse();
                _camera_orientation = inv_ship_rot * _camera_orientation;
                _camera_position = _camera_position - collisionBuilding->getPosition();
                Transforms::rotate(_camera_position, inv_ship_rot);
                
            } else if (ship != nullptr) {
                /* unboard */
                Transforms::rotate(_camera_position, ship->getOrientation());
                _camera_position = _camera_position + ship->getPosition();
                _camera_orientation = ship->getOrientation() * _camera_orientation;
                _boarded.reset();
                _piloting = false;

            }
            
        }
        _wasKeyPressed = true;
    } else if (kbd.isKeyDown(irr::KEY_KEY_P)) {
        if (!_wasKeyPressed) {
            auto ship = _boarded.lock();
            if (ship != nullptr && !_piloting) {
                _piloting  = true;
            } else _piloting = false;
        }
        _wasKeyPressed = true;        
    } else _wasKeyPressed = false;
    


}
