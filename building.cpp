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


bool Building::getCollisionCoords(const irr::core::line3df &ray, float &distance, irr::core::vector3di &_pointedAtBlockCoords, irr::core::vector3di &_pointedAtAdjacentBlockCoords) {
        vector3df collisionPoint;
        triangle3df collisionTriangle;
        ISceneNode *collisionNode = nullptr;
        ISceneCollisionManager *collMan = _smgr->getSceneCollisionManager();
        distance = FLT_MAX;

        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {

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


        _pointedAtBlockCoords = irr::core::vector3di(round(center.X/(Constants::BLOCK_SIZE*2)), round(center.Y/(Constants::BLOCK_SIZE*2)), round(center.Z/(Constants::BLOCK_SIZE*2)));
        _pointedAtAdjacentBlockCoords = irr::core::vector3di(round(adjacent_center.X/(Constants::BLOCK_SIZE*2)), round(adjacent_center.Y/(Constants::BLOCK_SIZE*2)), round(adjacent_center.Z/(Constants::BLOCK_SIZE*2)));

        return true;
}


std::pair<vector3dfp, vector3dfp> Building::prepareOtherBbox(const Building &other) {
        irr::core::quaternion rotation = getOrientation();
        rotation.makeInverse();
        vector3dfp translation = -getPosition();

        /* compute absolute position of other's bbox */
        auto tmp = other.getBoundingBox();
        // first scale from block unit to point unit
        vector3dfp tmpMin = vector3dfp(tmp.first.X * Constants::BLOCK_SIZE*2 - Constants::BLOCK_SIZE, tmp.first.Y* Constants::BLOCK_SIZE*2 - Constants::BLOCK_SIZE, tmp.first.Z* Constants::BLOCK_SIZE*2 - Constants::BLOCK_SIZE);
        vector3dfp tmpMax = vector3dfp(tmp.second.X* Constants::BLOCK_SIZE*2 + Constants::BLOCK_SIZE, tmp.second.Y* Constants::BLOCK_SIZE*2 + Constants::BLOCK_SIZE, tmp.second.Z* Constants::BLOCK_SIZE*2 + Constants::BLOCK_SIZE);
        // apply translation
        tmpMin += other.getPosition();
        tmpMax += other.getPosition();
        // apply rotation
        Transforms::rotate(tmpMin, other.getOrientation());
        Transforms::rotate(tmpMax, other.getOrientation());
        
        std::pair<vector3dfp, vector3dfp> bbox = std::pair<vector3dfp, vector3dfp>(tmpMin, tmpMax);

        
        vector3dfp transformedBboxMin = vector3dfp(1, 1, 1) * std::numeric_limits<std::int32_t>::max();
        vector3dfp transformedBboxMax = vector3dfp(1, 1, 1) * std::numeric_limits<std::int32_t>::min(); 
        
        for (int i = 0; i < 8; i++) {
                vector3dfp point(
                        (i & 1) ? bbox.first.X : bbox.second.X,
                        (i & 2) ? bbox.first.Y : bbox.second.Y,
                        (i & 4) ? bbox.first.Z : bbox.second.Z                       
                );
                point += translation;
                Transforms::rotate(point, rotation);
                transformedBboxMin.X = std::min(transformedBboxMin.X, point.X);
                transformedBboxMin.Y = std::min(transformedBboxMin.Y, point.Y);
                transformedBboxMin.Z = std::min(transformedBboxMin.Z, point.Z);

                transformedBboxMax.X = std::max(transformedBboxMax.X, point.X);
                transformedBboxMax.Y = std::max(transformedBboxMax.Y, point.Y);
                transformedBboxMax.Z = std::max(transformedBboxMax.Z, point.Z);

                
        }

        auto transformedBbox = std::pair<vector3dfp, vector3dfp>(transformedBboxMin, transformedBboxMax);
        return transformedBbox;

}