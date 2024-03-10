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

    irr::core::vector3di bboxMin;
    irr::core::vector3di bboxMax;


    

public:
    std::pair<vector3dfp, vector3dfp> prepareOtherBbox(const Building &);
    Building(irr::scene::ISceneManager *smgr);
    virtual void process(float);

    inline void save(int fd) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {
            chunk->save(fd);
        }
    }

    inline void updateBoundingBox(void) {
        bboxMin = irr::core::vector3di(1, 1, 1) * std::numeric_limits<std::int32_t>::max();
        bboxMax = irr::core::vector3di(1, 1, 1) * std::numeric_limits<std::int32_t>::min();        
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {            
            chunk->updateBoundingBox();
            auto chunkBbox = chunk->getBoundingBox();
            chunkBbox.first += chunk->getRelCoords();
            chunkBbox.second += chunk->getRelCoords();
            bboxMin.X = std::min(bboxMin.X, chunkBbox.first.X);
            bboxMin.Y = std::min(bboxMin.Y, chunkBbox.first.Y);
            bboxMin.Z = std::min(bboxMin.Z, chunkBbox.first.Z)
            ;
            bboxMax.X = std::max(bboxMax.X, chunkBbox.second.X);
            bboxMax.Y = std::max(bboxMax.Y, chunkBbox.second.Y);
            bboxMax.Z = std::max(bboxMax.Z, chunkBbox.second.Z);

        }        
    }

    inline std::pair<irr::core::vector3di, irr::core::vector3di> getBoundingBox(void) const {
        return std::pair<irr::core::vector3di, irr::core::vector3di>(bboxMin, bboxMax);
    }

    inline std::shared_ptr<Chunk> findOrCreateChunk(const irr::core::vector3di &where) {
        irr::core::vector3di rel_coords = where;
        rel_coords.X &= ~(Constants::CHUNK_SIZE - 1);
        rel_coords.Y &= ~(Constants::CHUNK_SIZE - 1);
        rel_coords.Z &= ~(Constants::CHUNK_SIZE - 1);  

        std::shared_ptr<Chunk> chunk = nullptr;
        // printf("look for chunk %d %d %d\n", rel_coords.X, rel_coords.Y, rel_coords.Z);
        const vector3dfp c1 = vector3dfp(rel_coords.X, rel_coords.Y, rel_coords.Z);
        OctreeBoundedNodeIterator octiter(_chunks, c1, c1);
        
        
        while (std::shared_ptr<Chunk> iter_chunk = std::static_pointer_cast<Chunk>(octiter.next())) {
            //abort();
            if (iter_chunk->belongsHere(where)) {
                // printf("reuse chunk %d %d %d\n", rel_coords.X, rel_coords.Y, rel_coords.Z);
                
                chunk = iter_chunk;
                break;
            }
        }
        if (chunk == nullptr) {
          
            chunk = std::shared_ptr<Chunk>(new Chunk(_smgr, this, rel_coords));
            chunk->setPosition(vector3dfp(rel_coords.X, rel_coords.Y, rel_coords.Z));
            // printf("create a new chunk %d %d %d\n", rel_coords.X, rel_coords.Y, rel_coords.Z);

            _chunks.insert(chunk);
        }
        return chunk;
    }

   inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {
        std::shared_ptr<Chunk> chunk = findOrCreateChunk(where);
        if (chunk != nullptr)
            chunk->addBlock(where - chunk->getRelCoords(), texture_coords);
    }

    inline void delBlock(const irr::core::vector3di &where) {
        std::shared_ptr<Chunk> chunk = findOrCreateChunk(where);
        if (chunk != nullptr)
            chunk->delBlock(where - chunk->getRelCoords());
    }    

    void updateMesh(const TextureId&tid, bool hilight = false, const irr::core::vector3di &_hilightedBlock = irr::core::vector3di()) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        //printf("On lance le updatemesh\n");
        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {            
            if (hilight && chunk->belongsHere(_hilightedBlock)) {
                chunk->updateMesh(tid, true, _hilightedBlock - chunk->getRelCoords());
            } else {
                chunk->updateMesh(tid, false, irr::core::vector3di());
 
            }
        }
        // printf("on finit le updatemesh\n");
    }

    void updatePositionAndOrientation() {
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);
        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {
            chunk->updatePositionAndOrientation();
        }
    }    

    inline irr::core::quaternion getOrientation(void) const {
        return _orientation;
    }


    inline void setOrientation(const irr::core::quaternion &orientation) {
        _orientation = orientation;
    }
    inline bool bonk(std::shared_ptr<Building> other) {
        return collision(other);
/*
        OctreeNodeIterator octiter = OctreeNodeIterator(_chunks);

        while (std::shared_ptr<Chunk> chunk = std::static_pointer_cast<Chunk>(octiter.next())) {
            OctreeNodeIterator octiter2 = OctreeNodeIterator(other->_chunks);
            while (std::shared_ptr<Chunk> chunk2 = std::static_pointer_cast<Chunk>(octiter2.next())) {
                //printf("testing bonk %p %p\n", chunk, chunk2);
                if (chunk->bonk(chunk2)) return true;

            }
        }
        */
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
    

    bool collision(std::shared_ptr<Building> other);


    
    virtual bool canMove(void) const { return false; }
    bool getCollisionCoords(const irr::core::line3df &ray, float &distance, irr::core::vector3di &_pointedAtBlockCoords, irr::core::vector3di &_pointedAtAdjacentBlockCoords);
};

#endif