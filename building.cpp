#include "common.h"

#include "building.h"
#include "assets.h"
#include "transforms.h"
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


Building::Building(irr::scene::ISceneManager *smgr) : Entity(smgr), _chunks(vector3dfp(-512, -512, -512), vector3dfp(512, 512, 512)) {
}

void Building::process(float delta) {
}


bool Building::getCollisionCoords(const irr::core::line3df &ray, float &distance, irr::core::vector3di &block_coords, irr::core::vector3di &adjacent_block_coords) {
        vector3df collisionPoint;
        triangle3df collisionTriangle;
        ISceneNode *collisionNode = nullptr;
        ISceneCollisionManager *collMan = _smgr->getSceneCollisionManager();
        distance = FLT_MAX;

        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {

                ITriangleSelector *selector = chunk->getSelector();
                vector3df tmpCollisionPoint;
                triangle3df tmpCollisionTriangle;
                ISceneNode *tmpCollisionNode = nullptr;
                
                if (collMan->getCollisionPoint(ray, selector, tmpCollisionPoint, tmpCollisionTriangle, tmpCollisionNode)) {
                        
                        if (distance > ray.start.getDistanceFrom(tmpCollisionPoint)) {
                                distance = ray.start.getDistanceFrom(tmpCollisionPoint);
                                collisionPoint = tmpCollisionPoint;
                                collisionTriangle = tmpCollisionTriangle;
                                collisionNode = tmpCollisionNode;
                        }

                }
        }

        if (collisionNode == nullptr)
                return false;
  
        vector3dfp pos = getPosition();
        irr::core::vector3df position = irr::core::vector3df(pos.X, pos.Y, pos.Z);
        collisionTriangle.pointA -= position;
        collisionTriangle.pointB -= position;
        collisionTriangle.pointC -= position;

        irr::core::quaternion inv_rotation = getOrientation();

        inv_rotation.makeInverse();

        Transforms::rotate(collisionTriangle.pointA, inv_rotation);
        Transforms::rotate(collisionTriangle.pointB, inv_rotation);
        Transforms::rotate(collisionTriangle.pointC, inv_rotation);

        irr::core::vector3df u = irr::core::vector3df(collisionTriangle.pointC - collisionTriangle.pointB);
        irr::core::vector3df v = irr::core::vector3df(collisionTriangle.pointA - collisionTriangle.pointB);

        irr::core::vector3df center = (collisionTriangle.pointA + collisionTriangle.pointC)/2 + v.crossProduct(u)/(Constants::BLOCK_SIZE*4);
        irr::core::vector3df adjacent_center = (collisionTriangle.pointA + collisionTriangle.pointC)/2 - v.crossProduct(u)/(Constants::BLOCK_SIZE*4);


        block_coords = irr::core::vector3di(round(center.X/(Constants::BLOCK_SIZE*2)), round(center.Y/(Constants::BLOCK_SIZE*2)), round(center.Z/(Constants::BLOCK_SIZE*2)));
        adjacent_block_coords = irr::core::vector3di(round(adjacent_center.X/(Constants::BLOCK_SIZE*2)), round(adjacent_center.Y/(Constants::BLOCK_SIZE*2)), round(adjacent_center.Z/(Constants::BLOCK_SIZE*2)));

        return true;
}
