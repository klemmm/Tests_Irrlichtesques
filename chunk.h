#ifndef CHUNK_H
#define CHUNK_H 1
#include "common.h"
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <unistd.h>
#include <limits>
#include "transforms.h"
#include "entity.h"
#include "assets.h"
#include "constants.h"
#include "octree.h"

class Building;


struct Block : public OctreeNode {
    irr::core::vector3di where;        
    irr::core::vector2di texture_coords;

    inline bool bonk(const vector3dfp &pos, vector3dfp &pos2, const irr::core::quaternion &rot, const irr::core::quaternion &rot2, const std::shared_ptr<Block> other) {
        vector3dfp rel = vector3dfp(where.X, where.Y, where.Z)*Constants::BLOCK_SIZE*2;
        vector3dfp rel2 = vector3dfp(other->where.X, other->where.Y, other->where.Z)*Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel, rot);
        Transforms::rotate(rel2, rot2);
        vector3dfp center = pos + rel;
        vector3dfp center2 = pos2 + rel2;

        
        if ((center/256).getDistanceFromSQ(center2/256) > fpnum(Constants::BLOCK_SIZE*Constants::BLOCK_SIZE*12)/65536) return false;


        vector3dfp base[3] = {vector3dfp(1.0, 0, 0), vector3dfp(0, 1.0, 0), vector3dfp(0, 0, 1.0)};
        vector3dfp axes[15];
        for (int i = 0; i < 3; i++) {
            axes[i] = base[i];
            Transforms::rotate(axes[i], rot);
        }
        for (int i = 0; i < 3; i++) {
            axes[i + 3] = base[i];
            Transforms::rotate(axes[i + 3], rot2);
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                axes[i*3 + j + 6] = axes[i].crossProduct(axes[j + 3]);
            }
        }

        vector3dfp points[8];
        vector3dfp points2[8];
        float half = Constants::BLOCK_SIZE;
        for (int i = 0; i < 8; i++) {
            points[i] = vector3dfp(
                (i & 1) ? +half : -half,
                (i & 2) ? +half : -half,
                (i & 4) ? +half : -half
            );
            Transforms::rotate(points[i], rot);
            points[i] += center;
            points2[i] = vector3dfp(
                (i & 1) ? +half : -half,
                (i & 2) ? +half : -half,
                (i & 4) ? +half : -half
            );
            Transforms::rotate(points2[i], rot2);   
            points2[i] += center2;         
        }

        for (int i = 0; i < 15; i++) {
            float min1 = FLT_MAX;
            float min2 = FLT_MAX;
            float max1 = -FLT_MAX;
            float max2 = -FLT_MAX;
            for (int j = 0; j < 8; j++) {
                float proj = points[j].dotProduct(axes[i]);
                float proj2 = points2[j].dotProduct(axes[i]);
                min1 = std::min(min1, proj);
                max1 = std::max(max1, proj);
                min2 = std::min(min2, proj2);
                max2 = std::max(max2, proj2);
                if ((max1 >= min2) && (max2 >= min1))
                    break;
            }
            if ((max1 < min2) || (max2 < min1)) {
                return false;
            }
            
        }
        return true;
        //return (center.getDistanceFrom(center2) < Constants::BLOCK_SIZE*2);

    }
};

class Chunk : public Entity {
private:
    Building *_building;
    irr::core::vector3di _rel_coords;



    Octree _blocks;


    irr::scene::SMesh *_mesh;
    irr::scene::SMeshBuffer *_meshBuffer;
    irr::scene::ISceneNode *_node;
    irr::scene::ITriangleSelector *_selector;
    
    irr::s32  _chunk_id;

    size_t _nblocks;

    irr::core::vector3di bboxMin;
    irr::core::vector3di bboxMax;
    static irr::s32 _next_id;
    static std::map<irr::s32, Chunk*> _map;

    


public:
    Chunk(irr::scene::ISceneManager *smgr, Building*, const irr::core::vector3di &);


    inline virtual ~Chunk(void) {
        _mesh->drop();
        _meshBuffer->drop();
        if (_selector != nullptr)
                _selector->drop();        
        _map.erase(_chunk_id);
    }

    inline void updateBoundingBox(void) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_blocks);
        bboxMin = irr::core::vector3di(1, 1, 1) * std::numeric_limits<std::int32_t>::max();
        bboxMax = irr::core::vector3di(1, 1, 1) * std::numeric_limits<std::int32_t>::min();
        while (std::shared_ptr<Block> block = std::static_pointer_cast<Block>(octiter.next())) {
            bboxMin.X = std::min(bboxMin.X, block->where.X);
            bboxMin.Y = std::min(bboxMin.Y, block->where.Y);
            bboxMin.Z = std::min(bboxMin.Z, block->where.Z);
            bboxMax.X = std::max(bboxMax.X, block->where.X);
            bboxMax.Y = std::max(bboxMax.Y, block->where.Y);
            bboxMax.Z = std::max(bboxMax.Z, block->where.Z);        
        }        
    }
    
    inline std::pair<irr::core::vector3di, irr::core::vector3di> getBoundingBox(void) {
        return std::pair<irr::core::vector3di, irr::core::vector3di>(bboxMin, bboxMax);
    }

    inline void save(int fd) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_blocks);

        while (std::shared_ptr<Block> block = std::static_pointer_cast<Block>(octiter.next())) {
            irr::s32 X = block->where.X + _rel_coords.X;
            irr::s32 Y = block->where.Y + _rel_coords.Y;
            irr::s32 Z = block->where.Z + _rel_coords.Z;
            write(fd, (void*) &X, sizeof(irr::s32));
            write(fd, (void*) &Y, sizeof(irr::s32));
            write(fd, (void*) &Z, sizeof(irr::s32));
            write(fd, (void*) &block->texture_coords.X, sizeof(irr::s32));
            write(fd, (void*) &block->texture_coords.Y, sizeof(irr::s32));
        }
    }


    virtual void process(float);
    irr::core::vector3di getRelCoords(void) { return _rel_coords; }

    bool belongsHere(const irr::core::vector3di &where) {
        return ((where.X >= _rel_coords.X) &&
            (where.Y >= _rel_coords.Y) &&
            (where.Z >= _rel_coords.Z) &&
            (where.X < _rel_coords.X + Constants::CHUNK_SIZE) &&
            (where.Y < _rel_coords.Y + Constants::CHUNK_SIZE) &&
            (where.Z < _rel_coords.Z + Constants::CHUNK_SIZE));
    }
    inline void addBlock(const irr::core::vector3di &where, const irr::core::vector2di texture_coords) {

        std::shared_ptr<Block> b = std::shared_ptr<Block>(new Block());
        b->setPosition(vector3dfp(where.X, where.Y, where.Z));

        if (!_blocks.belongsHere(b->getPosition())) {
            return;
        }        
        b->where = where;    
        b->texture_coords = texture_coords;    


        const vector3dfp c1 = vector3dfp(where.X, where.Y, where.Z);
        OctreeBoundedNodeIterator octiter(_blocks, c1, c1);
        
        
        while (std::shared_ptr<Block> iter_block = std::static_pointer_cast<Block>(octiter.next())) {
            if (iter_block->getPosition() == b->getPosition())
                return;
        }

        
          _blocks.insert(b);  
          _nblocks++;
    }

    inline void delBlock(const irr::core::vector3di &where) {

        _blocks.erase_if([&where](std::shared_ptr<OctreeNode> n) { return std::static_pointer_cast<Block>(n)->where == where; });
        _nblocks--;
    }

    void updateMesh(const TextureId&, bool _hilight, const irr::core::vector3di &_hilightedBlock);

    void updatePositionAndOrientation(void);


    inline irr::scene::ITriangleSelector *getSelector(void) const {
        return _selector;
    }

    bool bonk(std::shared_ptr<Chunk> other);

    static Chunk* getChunkFromId(irr::s32 id);

};

#endif