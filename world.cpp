class fpnum;

#include "common.h"

#include <unordered_set>

#include "assets.h"
#include "world.h"
#include "transforms.h"
#include "constants.h"
#include "ship.h"
#include "debug.h"
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



World::World (irr::scene::ISceneManager *smgr) : _smgr(smgr), _buildings(vector3dfp(-Constants::WORLD_SIZE, -Constants::WORLD_SIZE, -Constants::WORLD_SIZE), vector3dfp(Constants::WORLD_SIZE, Constants::WORLD_SIZE, Constants::WORLD_SIZE)),_wasLeftPressed(false), _wasRightPressed(false), _wasKeyPressed(false), _invOpen(false), _held(0), _piloting(false), _dc_me(nullptr), _dc_sta(nullptr) {
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
    _sta = testShip2;

    scene::ISceneNode* skybox = _smgr->addSkyBoxSceneNode(
        TextureLoader::get(SKYBOX_UP),
        TextureLoader::get(SKYBOX_DN),
        TextureLoader::get(SKYBOX_LT),
        TextureLoader::get(SKYBOX_RT),
        TextureLoader::get(SKYBOX_FT),
        TextureLoader::get(SKYBOX_BK));


    _me->updateBoundingBox();
    _sta->updateBoundingBox();
   // auto bbox = _sta->getBoundingBox();
    

   // printf("%d %d %d   %d %d %d\n", bbox.first.X, bbox.first.Y, bbox.first.Z, bbox.second.X, bbox.second.Y, bbox.second.Z); 
  //  _dc_sta = new DebugCuboid(_smgr, irr::core::vector3df(bbox.first.X, bbox.first.Y, bbox.first.Z) * Constants::BLOCK_SIZE*2 - irr::core::vector3df(Constants::BLOCK_SIZE, Constants::BLOCK_SIZE, Constants::BLOCK_SIZE), Constants::BLOCK_SIZE*2 * irr::core::vector3df(bbox.second.X, bbox.second.Y, bbox.second.Z) + irr::core::vector3df(Constants::BLOCK_SIZE, Constants::BLOCK_SIZE, Constants::BLOCK_SIZE));
    

    //_dc->setPosition(irr::core::vector3df(300, 300, 300));
    //_dc->setRotation(irr::core::vector3df(45, 45, 45));
 
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


void World::updateHud(Hud &hud) {
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
}

void World::prepareScene(void) {
    // PREPARE SCENE

    std::shared_ptr<Building> hilightedBuildingLocked = _hilightedBuilding.lock();
    for (auto iter = _meshNeedsUpdate.begin(); iter != _meshNeedsUpdate.end(); iter++) {        
        (*iter)->updateMesh(TextureId::DEFAULT, hilightedBuildingLocked == (*iter), _hilightedBlock);
    }

    if (auto ship = _boarded.lock()) {
        vector3dfp rotated_position = _relativeCameraPosition;
        Transforms::rotate(rotated_position, ship->getOrientation());
        position_and_orient_camera(rotated_position + ship->getPosition(), ship->getOrientation() * _relativeCameraOrientation);
    } else {
        position_and_orient_camera(_relativeCameraPosition, _relativeCameraOrientation);
    }


    OctreeNodeIterator octiter2 = OctreeNodeIterator(_buildings);
    while (std::shared_ptr<Building> building = std::static_pointer_cast<Building>(octiter2.next())) {
        vector3dfp old_pos = building->getPosition();
        building->updatePositionAndOrientation();
    }
}

void World::handleInputs(float delta, Hud &hud, IKkbdStatus &kbd, const vector2df& mouse_movement) {
    // HANDLE MISC INPUT
        
    if (!kbd.isLeftPressed() && _wasLeftPressed) {
        _wasLeftPressed = false;
    }

    if (!kbd.isRightPressed() && _wasRightPressed) {
        _wasRightPressed = false;
    }




    std::shared_ptr<Building> new_hilighted_building = _hilightedBuilding.lock();
    irr::core::vector3di new_hilighted_block = _hilightedBlock;



    if (_pointedAtBuilding) {
        new_hilighted_building = _pointedAtBuilding;
        new_hilighted_block = _pointedAtBlockCoords;
    } else {
        new_hilighted_building = nullptr;
    }

    auto _hilighted_building_locked = _hilightedBuilding.lock();
    
    if (_hilighted_building_locked != new_hilighted_building) {
        if (_hilighted_building_locked)
            _meshNeedsUpdate.insert(_hilighted_building_locked);
        if (_pointedAtBuilding)
            _meshNeedsUpdate.insert(_pointedAtBuilding);            
    } else if (_hilightedBlock != new_hilighted_block) {
        if (_pointedAtBuilding)
            _meshNeedsUpdate.insert(_pointedAtBuilding);
    }
    _hilightedBuilding = new_hilighted_building;
    _hilightedBlock = new_hilighted_block;
        
    if (kbd.isLeftPressed() && !_wasLeftPressed) {
        _wasLeftPressed = true;
        if (_pointedAtBuilding) {
            _meshNeedsUpdate.insert(_pointedAtBuilding);
            _pointedAtBuilding->delBlock(_pointedAtBlockCoords);
        }        
    }

    if (kbd.isRightPressed() && !_wasRightPressed) {
        _wasRightPressed = true;
        if (_pointedAtBuilding) {
            _meshNeedsUpdate.insert(_pointedAtBuilding);
            _pointedAtBuilding->addBlock(_pointedAtAdjacentBlockCoords, vector2di(_held/3, _held%3));
        }        
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
            if (ship  == nullptr && _pointedAtBuilding) {
                /* board */
                _boarded = _pointedAtBuilding;
                quaternion inv_ship_rot = _pointedAtBuilding->getOrientation();
                inv_ship_rot.makeInverse();
                _relativeCameraOrientation = inv_ship_rot * _relativeCameraOrientation;
                _relativeCameraPosition = _relativeCameraPosition - _pointedAtBuilding->getPosition();
                Transforms::rotate(_relativeCameraPosition, inv_ship_rot);
                
            } else if (ship != nullptr) {
                /* unboard */
                Transforms::rotate(_relativeCameraPosition, ship->getOrientation());
                _relativeCameraPosition = _relativeCameraPosition + ship->getPosition();
                _relativeCameraOrientation = ship->getOrientation() * _relativeCameraOrientation;
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

  /* HANDLE INPUTS: MOVEMENTS */
    
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
            _relativeCameraOrientation = _relativeCameraOrientation * rotation;

            if (mouse_movement.Y < 0.5f) {
                rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.Y), vector3df(1.0, 0.0, 0.0));
            }  
            if (mouse_movement.Y > 0.5f) {
                rotation.fromAngleAxis(MOUSE_SENSITIVITY*(0.5f - mouse_movement.Y), vector3df(1.0, 0.0, 0.0));
            }     
            _relativeCameraOrientation = _relativeCameraOrientation * rotation;    

            if (kbd.isKeyDown(irr::KEY_KEY_A)) {
                rotation.fromAngleAxis(-ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
            }  

            if (kbd.isKeyDown(irr::KEY_KEY_E)) {
                rotation.fromAngleAxis(ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
            }    
        }
    }

    _relativeCameraOrientation = _relativeCameraOrientation * rotation;
    Transforms::rotate(movement, _relativeCameraOrientation);  
    _relativeCameraPosition = _relativeCameraPosition + movement;

}

void World::updatePointedAt(void) {
    /* PROCESS COLLISION */
    _pointedAtBuilding = nullptr;
    
    vector3di tmp__pointedAtBlockCoords, tmp__pointedAtAdjacentBlockCoords;

    core::line3d<f32> ray;
    ray.start = _camera->getPosition();
    ray.end = ray.start + (_camera->getTarget() - ray.start).normalize() * 1000.0f;

    float distance = FLT_MAX;
    float cur_dist;
    
    vector3dfp radius = vector3dfp(10000, 10000, 10000);
    irr::core::vector3df real_camera_pos = _camera->getPosition();
    vector3dfp tmp_pos = vector3dfp(real_camera_pos.X, real_camera_pos.Y, real_camera_pos.Z);
    vector3dfp corner1 = tmp_pos - radius;
    vector3dfp corner2 = tmp_pos + radius;
    OctreeBoundedNodeIterator octiter = OctreeBoundedNodeIterator(_buildings, corner1, corner2);
    
    while (std::shared_ptr<Building> building = std::static_pointer_cast<Building>(octiter.next())) {
        
        if (building->getCollisionCoords(ray, cur_dist, tmp__pointedAtBlockCoords, tmp__pointedAtAdjacentBlockCoords)) {
            if (cur_dist < distance) {
                distance = cur_dist;
                _pointedAtBlockCoords = tmp__pointedAtBlockCoords;
                _pointedAtAdjacentBlockCoords = tmp__pointedAtAdjacentBlockCoords;
                _pointedAtBuilding = building;                
            }
        }
    }
}

void World::processBuildings(float delta) {
    std::vector<std::shared_ptr<Building> > to_migrate;

    OctreeNodeIterator octiter1 = OctreeNodeIterator(_buildings);
    while (std::shared_ptr<Building> building = std::static_pointer_cast<Building>(octiter1.next())) {
        building->process(delta);
        if (!octiter1.belongsHere(building->getPosition())) {
            to_migrate.push_back(std::shared_ptr<Building>(building));
        }
    }

    for (auto iter = to_migrate.begin(); iter != to_migrate.end(); iter++) {
        std::shared_ptr<Building> building = (*iter);
        _buildings.erase_if([building](std::shared_ptr<OctreeNode> n) { 
            return std::static_pointer_cast<Building>(n) == building; });       
        
        _buildings.insert(building);
    }       
}

void World::process(float delta, Hud &hud, IKkbdStatus &kbd, const vector2df& mouse_movement) {

    updateHud(hud);

    updatePointedAt();

    handleInputs(delta, hud, kbd, mouse_movement);

    processBuildings(delta);

/*    auto stapos = _sta->getPosition();
    auto staorientation = _sta->getOrientation();
    _dc_sta->setPosition(irr::core::vector3df(stapos.X, stapos.Y, stapos.Z));
    Transforms::orient_node(*_dc_sta, staorientation);

    _me->updateBoundingBox();
    std::pair<vector3dfp, vector3dfp> tbbox = _sta->prepareOtherBbox(*_me);

    if (_dc_me) delete _dc_me;
    _dc_me = new DebugCuboid(_smgr, irr::core::vector3df(tbbox.first.X, tbbox.first.Y, tbbox.first.Z), irr::core::vector3df(tbbox.second.X, tbbox.second.Y, tbbox.second.Z));    
    irr::core::vector3df top = irr::core::vector3df(tbbox.first.X, tbbox.first.Y, tbbox.first.Z);
    irr::core::vector3df bot = irr::core::vector3df(tbbox.second.X, tbbox.second.Y, tbbox.second.Z);
    printf("%f %f %f    %f %f %f\n", top.X, top.Y, top.Z, bot.X, bot.Y,bot.Z);
    //exit(1);

    */

    prepareScene();
}
