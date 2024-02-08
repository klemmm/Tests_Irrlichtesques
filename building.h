#ifndef BUILDING_H
#define BUILDING_H 1
#include <irrlicht/irrlicht.h>
#include <vector>
#include "entity.h"
#include "assets.h"



class Building : public Entity {
private:
    struct Block {
        irr::core::vector3di where;        
        irr::core::vector2di texture_coords;
    };
    std::vector<Block> blocks;

    irr::scene::SMesh *_mesh;
    irr::scene::SMeshBuffer *_meshBuffer;
    irr::scene::ISceneNode *_node;
public:
    Building(irr::scene::ISceneManager *smgr);
    virtual void process(float);
    inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {
        Block b;
        b.where = where;    
        b.texture_coords = texture_coords;    
        blocks.push_back(b);
    }
    void updateMesh(const TextureId&);

};

#endif