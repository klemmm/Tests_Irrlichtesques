#include <irrlicht/irrlicht.h>

#include "assets.h"
#include "world.h"
#include "transforms.h"
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#define ROTATION_SPEED 1.0 /* radian/sec */
#define SPEED 500.0 /* irrlicht space unit/sec */
#define MOUSE_SENSITIVITY 1.0

World::World (irr::scene::ISceneManager *smgr) : _smgr(smgr), _wasLeftPressed(false), _wasRightPressed(false), _hilighted(-1) {
    _camera = _smgr->addCameraSceneNode();
    _camera->setFarValue(10000);

    _testChunk = std::unique_ptr<Chunk>(new Chunk(_smgr));
    for (int i = 0; i < 10; i++)
        _testChunk->addBlock(vector3di(0, i, 0), vector2di(0, 0));
    

    _testChunk->setPosition(vector3df(100, 100, 100));
    _testChunk->setRotation(quaternion(0, 0, 0));
    _testChunk->updateMesh(TextureId::DEFAULT);

    _testChunk2 = std::unique_ptr<Chunk>(new Chunk(_smgr));
    for (int i = 0; i < 10; i++)
        _testChunk2->addBlock(vector3di(0, i, 0), vector2di(0, 0));
    

    _testChunk2->setPosition(vector3df(500, 500, 500));
    _testChunk2->setRotation(quaternion(45, 0, 0));    
    _testChunk2->updateMesh(TextureId::DEFAULT);
    
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

    core::line3d<f32> ray;
    ray.start = _camera->getPosition();
    ray.end = ray.start + (_camera->getTarget() - ray.start).normalize() * 10000.0f;
    scene::ISceneCollisionManager* collMan = _smgr->getSceneCollisionManager();


    vector3df collisionPoint;
    triangle3df collisionTriangle;
    static ISceneNode *collisionNode = nullptr;
    
    
    if (!kbd.isLeftPressed() && _wasLeftPressed) {
        _wasLeftPressed = false;
    }

    if (!kbd.isRightPressed() && _wasRightPressed) {
        _wasRightPressed = false;
    }

    vector3di block_coords, adjacent_block_coords;
    bool collision = false;
    bool need_update_mesh = false;
    Chunk *chunk = nullptr;


    std::vector<ITriangleSelector*> selectors;
    selectors.push_back(_testChunk->getSelector());
    selectors.push_back(_testChunk2->getSelector());

    for (auto iter = selectors.begin(); iter != selectors.end(); iter++) {
        if (collMan->getCollisionPoint(ray, *iter, collisionPoint, collisionTriangle, collisionNode)) {
            collision = true;
            break;
        }
    }


    if (collision) {
        chunk = Chunk::getChunkFromId(collisionNode->getID());
        chunk->getCollisionCoords(collisionTriangle, block_coords, adjacent_block_coords);
        printf("Pointing to chunk %d block %d,%d,%d node=%p\n", collisionNode->getID(), block_coords.X, block_coords.Y, block_coords.Z, collisionNode);
        _hilighted = collisionNode->getID();
        if (chunk->hilightBlock(block_coords))
            need_update_mesh = true;
    } else {
        if (_hilighted != -1) {
            chunk = Chunk::getChunkFromId(_hilighted);
            if (chunk) {
                _hilighted = -1;
                chunk->removeHilight();
                need_update_mesh = true;
            }
        }
    }

    if (kbd.isLeftPressed() && !_wasLeftPressed) {
        _wasLeftPressed = true;
        if (collision) {
            need_update_mesh = true;
            chunk->delBlock(block_coords);
        }        
    }


    if (kbd.isRightPressed() && !_wasRightPressed) {
        _wasRightPressed = true;
        if (collision) {
            need_update_mesh = true;
            chunk->addBlock(adjacent_block_coords, vector2di(0, 0));
        }        
    }


    if (need_update_mesh) {
        chunk->updateMesh(TextureId::DEFAULT);
    }

}
