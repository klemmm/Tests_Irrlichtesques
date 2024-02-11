#ifndef BUILDING_H
#define BUILDING_H 1
#include <irrlicht/irrlicht.h>
#include <vector>
#include <map>
#include <algorithm>
#include "transforms.h"
#include "entity.h"
#include "assets.h"
#include "constants.h"



class Chunk : public Entity {
private:
    struct Block {
        irr::core::vector3di where;        
        irr::core::vector2di texture_coords;
    };
    std::vector<Block> blocks;

    irr::scene::SMesh *_mesh;
    irr::scene::SMeshBuffer *_meshBuffer;
    irr::scene::ISceneNode *_node;
    irr::scene::ITriangleSelector *_selector;
    
    irr::core::vector3di _hilighted;
    bool _hasHilight;

    irr::core::vector3df _position;
    irr::core::quaternion _rotation;

    irr::s32  _chunk_id;

    static irr::s32 _next_id;
    static std::map<irr::s32, Chunk*> _map;

public:
    Chunk(irr::scene::ISceneManager *smgr);
    inline virtual ~Chunk(void) {
        _map.erase(_chunk_id);
    }
    virtual void process(float);
    inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {
        Block b;
        b.where = where;    
        b.texture_coords = texture_coords;    
        blocks.push_back(b);
    }

    inline void delBlock(const irr::core::vector3di &where) {
        blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [&where](Block& b) { return b.where == where; }), blocks.end());
    }

    inline bool hilightBlock(const irr::core::vector3di &where) {
        if (_hasHilight && _hilighted == where) return false;
        _hilighted = where;
        _hasHilight = true;
        return true;
    }
    inline void removeHilight() {
        _hasHilight = false;
    }


    void updateMesh(const TextureId&);

    inline irr::scene::ITriangleSelector *getSelector(void) const {
        return _selector;
    }

    inline void setRotation(const irr::core::quaternion &rotation) {
        _rotation = rotation;
    }

    inline void setPosition(const irr::core::vector3df &position) {
        _position = position;
    }    

    inline void getCollisionCoords(irr::core::triangle3df collisionTriangle, irr::core::vector3di &block_coords, irr::core::vector3di &adjacent_block_coords) {
        collisionTriangle.pointA -= _node->getPosition();
        collisionTriangle.pointB -= _node->getPosition();
        collisionTriangle.pointC -= _node->getPosition();

        irr::core::quaternion inv_rotation = _rotation;

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

    }

    static Chunk* getChunkFromId(irr::s32 id);

};

#endif