#ifndef BUILDING_H
#define BUILDING_H 1
#include "common.h"
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cassert>
#include <time.h>
#include "transforms.h"
#include "entity.h"
#include "assets.h"
#include "constants.h"
#include "chunk.h"
#include "octree.h"



class Building : public Entity {
private:
    Octree _chunks;

    irr::core::quaternion _orientation;

public:
    Building(irr::scene::ISceneManager *smgr);
    virtual void process(float);

    inline void save(int fd) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {
            chunk->save(fd);
        }
    }
    inline Chunk* findOrCreateChunk(const irr::core::vector3di &where) {
        irr::core::vector3di rel_coords = where;
        rel_coords.X &= ~(Constants::CHUNK_SIZE - 1);
        rel_coords.Y &= ~(Constants::CHUNK_SIZE - 1);
        rel_coords.Z &= ~(Constants::CHUNK_SIZE - 1);  

        Octree *octant = _chunks.find(vector3dfp(rel_coords.X, rel_coords.Y, rel_coords.Z));
        if (octant == nullptr)
            return nullptr;
        Chunk *chunk = nullptr;
        for (auto iter = octant->begin(); iter != octant->end(); iter++) {
            if (static_cast<Chunk*>(*iter)->belongsHere(where)) {
                chunk = static_cast<Chunk*>(*iter);
                break;
            }
        }
        if (chunk == nullptr) {
          
            chunk = new Chunk(_smgr, this, rel_coords);
            chunk->setPosition(vector3dfp(rel_coords.X, rel_coords.Y, rel_coords.Z));
           // printf("create a new chunk %d %d %d\n", rel_coords.X, rel_coords.Y, rel_coords.Z);

            _chunks.insert(*chunk);
        } 
        return chunk;
    }

   inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {
        Chunk *chunk = findOrCreateChunk(where);
        if (chunk != nullptr)
            chunk->addBlock(where - chunk->getRelCoords(), texture_coords);
    }

    inline void delBlock(const irr::core::vector3di &where) {
        Chunk *chunk = findOrCreateChunk(where);
        if (chunk != nullptr)
            chunk->delBlock(where - chunk->getRelCoords());
    }    

    inline void hilightBlock(const irr::core::vector3di &where) {
        Chunk *chunk = findOrCreateChunk(where);
        if (chunk != nullptr)
            chunk->hilightBlock(where - chunk->getRelCoords());
    }
    
    inline void removeHilight() {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {
            chunk->removeHilight(); /* FIXME */
        }
    }

    void updateMesh(const TextureId &tid) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        //printf("On lance le updatemesh\n");
        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {
            chunk->updateMesh(tid);
        }
        // printf("on finit le updatemesh\n");
    }

    void updatePositionAndOrientation() {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {
            chunk->updatePositionAndOrientation(); /* FIXME */
        }
    }    

    inline irr::core::quaternion getOrientation(void) const {
        return _orientation;
    }


    inline void setOrientation(const irr::core::quaternion &orientation) {
        _orientation = orientation;
    }
    inline bool bonk(std::shared_ptr<Building> other) {

        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (Chunk *chunk = static_cast<Chunk*>(octiter.next())) {
            OctreeNodeIterator octiter2 = OctreeNodeIterator(other->_chunks);
            while (Chunk *chunk2 = static_cast<Chunk*>(octiter2.next())) {
                //printf("testing bonk %p %p\n", chunk, chunk2);
                if (chunk->bonk(chunk2)) return true;

            }
        }
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