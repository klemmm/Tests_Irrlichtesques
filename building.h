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



class Building : public Entity {
private:
    std::vector<std::shared_ptr<Chunk> > _chunks;

    irr::core::vector3df _position;
    irr::core::quaternion _orientation;

public:
    Building(irr::scene::ISceneManager *smgr);
    virtual void process(float);

   inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {
        _chunks[0]->addBlock(where, texture_coords);

    }

    inline void delBlock(const irr::core::vector3di &where) {
        _chunks[0]->delBlock(where);
    }    

    inline void hilightBlock(const irr::core::vector3di &where) {
        _chunks[0]->hilightBlock(where);
    }
    
    inline void removeHilight() {
        _chunks[0]->removeHilight();
    }

    void updateMesh(const TextureId &tid) {
        _chunks[0]->updateMesh(tid);
    }

    void updatePositionAndOrientation() {
        _chunks[0]->updatePositionAndOrientation();
    }    

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
            for (auto iter = _chunks.begin(); iter != _chunks.end(); iter++) {
                for (auto iter2 = other->_chunks.begin(); iter2 != other->_chunks.end(); iter2++) {
                    if ((*iter)->bonk(*iter2)) return true;
                }
            }
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