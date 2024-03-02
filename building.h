#ifndef BUILDING_H
#define BUILDING_H 1
#include <irrlicht/irrlicht.h>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <time.h>
#include "transforms.h"
#include "entity.h"
#include "assets.h"
#include "constants.h"
#include "chunk.h"


struct BuildingChunk {
    size_t _nblocks;
    irr::scene::SMesh *_mesh;
    irr::scene::SMeshBuffer *_meshBuffer;
    irr::scene::ISceneNode *_node;
    irr::scene::ITriangleSelector *_selector;  
    bool in_use;
};

class Building : public Entity {
private:
    irr::core::vector3df _rel_coords; /* FIXME remove */

    irr::core::vector3df _position;
    irr::core::quaternion _orientation;

    irr::s32 _dimx, _dimy, _dimz;
    
    Octree _blocks;
    size_t _nblocks;
    
    irr::core::vector3di _hilighted;
    bool _hasHilight;

    irr::s32  _building_id;

    static irr::s32 _next_id;
    static std::map<irr::s32, Building*> _map;

    BuildingChunk *_chunks;

public:
    Building(irr::scene::ISceneManager *smgr);
    virtual void process(float);

    inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {

        Block *b = new Block();
        b->setPosition(irr::core::vector3df(where.X, where.Y, where.Z));

        if (!_blocks.belongsHere(b->getPosition())) {
            delete b;
            return;
        }        
        b->where = where;    
        b->texture_coords = texture_coords;    
        _blocks.insert(*b);  
        _nblocks++;
    }

    inline void delBlock(const irr::core::vector3di &where) {
        Octree *octree = _blocks.find(irr::core::vector3df(where.X, where.Y, where.Z));
        std::vector<OctreeNode*> &_nodes = octree->getNodes();
        _nodes.erase(std::remove_if(_nodes.begin(), _nodes.end(), [&where](OctreeNode*n) { return static_cast<Block*>(n)->where == where; }), _nodes.end());
        _nblocks--;
    }

    inline void hilightBlock(const irr::core::vector3di &where) {
        _hilighted = where;
        _hasHilight = true;
    }
    
    inline void removeHilight() {
        _hasHilight = false;
    }

/*
    inline irr::scene::ITriangleSelector *getSelector(void) const {
        return _selector;
    }
*/

    void updateMesh(const TextureId &tid);

    void updatePositionAndOrientation();

    inline irr::core::vector3df getPosition(void) const {
        return _position;
    }

    inline irr::core::quaternion getOrientation(void) const {
        return _orientation;
    }

    inline void setPosition(const irr::core::vector3df &position) {
        _position = position;
    }

    inline void setOrientation(const irr::core::quaternion &orientation) {
        _orientation = orientation;
    }
    inline bool bonk(std::shared_ptr<Building> other) {

#define NUMBONKS 10000L

        //struct timespec start, stop;
        //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        //for (int lol = 0; lol < NUMBONKS; lol++) {

            /*
            for (auto iter = _chunks.begin(); iter != _chunks.end(); iter++) {
                for (auto iter2 = other->_chunks.begin(); iter2 != other->_chunks.end(); iter2++) {
                    if ((*iter)->bonk(*iter2)) return true;
                }
            }
            */
        //}
        //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

        //unsigned long long elapsed = (stop.tv_sec - start.tv_sec)*1000000;
        //elapsed += (stop.tv_nsec - start.tv_nsec)/1000;
        //elapsed++;
        //printf("%lu bonks in %llu usec (%llu bonks/sec)\n", NUMBONKS, elapsed, (NUMBONKS*1000000)/elapsed);
        return false;
    }
    

    
    virtual bool canMove(void) const { return false; }
    bool getCollisionCoords(const irr::core::line3df &ray, float &distance, irr::core::vector3di &block_coords, irr::core::vector3di &adjacent_block_coords);
};

#endif