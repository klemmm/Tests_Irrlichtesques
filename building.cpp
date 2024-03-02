#include <irrlicht/irrlicht.h>

#include "building.h"
#include "assets.h"
#include "transforms.h"
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


irr::s32 Building::_next_id = 0;
std::map<irr::s32, Building*>  Building::_map;

Building::Building(irr::scene::ISceneManager *smgr) : Entity(smgr), _dimx(2), _dimy(2), _dimz(2), _blocks(irr::core::vector3df(0, 0, 0), irr::core::vector3df(8*_dimx, 8*_dimy, 8*_dimz)), _nblocks(0) {
        _chunks = new BuildingChunk[_dimx * _dimy * _dimz];

        for (int i = 0; i < _dimx * _dimy * _dimz; i++)
                _chunks[i].in_use = false;

/*
        for (int i = 0; i < _dimx; i++) 
                for (int j = 0; j < _dimy; j++)
                        for (int k = 0; k < _dimz; k++) {
                                int index = k * _dimy * _dimx + j*_dimx + i;
                                _chunks[index] 
                                _chunks[index]._mesh = new SMesh();
                                _chunks[index]._meshBuffer = new SMeshBuffer();       
                                _chunks[index]._mesh->addMeshBuffer(_chunks[index]._meshBuffer);
                                _chunks[index]._node = nullptr;  
                                _chunks[index]._selector = nullptr;   
                        }


   
        */ 
        _hasHilight = false;
        
        _building_id = _next_id;
        _next_id++;
        _map[_building_id] = this;
        
        // _chunks.push_back(std::shared_ptr<Chunk>(new Chunk(smgr, this)));
}


void Building::process(float delta) {
}


void Building::updatePositionAndOrientation(void) {
        for (int i = 0; i < _dimx * _dimy * _dimz; i++) {
                ISceneNode *node = _chunks[i]._node;
        
                node->setPosition(getPosition() + irr::core::vector3df(_rel_coords.X, _rel_coords.Y, _rel_coords.Z) * Constants::BLOCK_SIZE*2);
                Transforms::orient_node(*node, getOrientation());
        }
}

bool Building::getCollisionCoords(const irr::core::line3df &ray, float &distance, irr::core::vector3di &block_coords, irr::core::vector3di &adjacent_block_coords) {
       
       /*
        vector3df collisionPoint;
        triangle3df collisionTriangle;
        ISceneNode *collisionNode = nullptr;
        ISceneCollisionManager *collMan = _smgr->getSceneCollisionManager();
        distance = FLT_MAX;


        ITriangleSelector *selector = getSelector();
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

        if (collisionNode == nullptr)
                return false;
  
  
        collisionTriangle.pointA -= getPosition();
        collisionTriangle.pointB -= getPosition();
        collisionTriangle.pointC -= getPosition();

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
        */
       return false;
}



void Building::updateMesh(const TextureId &texture_id) {
        for (int i = 0; i < _dimx * _dimy * _dimz; i++) {
                if (_chunks[i].in_use) {
                        _chunks[i]._node->remove();
                
                        irr::scene::SMeshBuffer *_meshBuffer = _chunks[i]._meshBuffer;

                        _meshBuffer->Vertices.reallocate(24 * _chunks[i]._nblocks);
                        _meshBuffer->Vertices.set_used(24 * _chunks[i]._nblocks);

                        _meshBuffer->Indices.reallocate(36 * _chunks[i]._nblocks);
                        _meshBuffer->Indices.set_used(36 * _chunks[i]._nblocks);  
                }                       
        }




        int i = 0;
        
        vector2df tstep = TextureLoader::getStep(texture_id);
        if (_selector != nullptr)
                _selector->drop();
        
        OctreeNodeIterator octiter = OctreeNodeIterator(_blocks);
        while (Block *block = static_cast<Block*>(octiter.next())) {
                SColor color = (_hasHilight && _hilighted == block->where) ? SColor(255,255,255,255) : SColor(255,128,128,128);
                
                // Up
                _meshBuffer->Vertices[i * 24 + 0] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, block->texture_coords.X*tstep.X + 0, block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 1] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 2] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 3] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);

                // Down
                _meshBuffer->Vertices[i * 24 + 4] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 5] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 6] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 7] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + tstep.Y);

                // Right
                _meshBuffer->Vertices[i * 24 + 8] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 9] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 10] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 11] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);

                // Left
                _meshBuffer->Vertices[i * 24 + 12] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 13] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 14] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 15] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);

                // Back
                _meshBuffer->Vertices[i * 24 + 16] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 17] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 18] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 19] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);
                
                // Front
                _meshBuffer->Vertices[i * 24 + 20] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 21] = video::S3DVertex(-Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, block->texture_coords.X*tstep.X + 0,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 22] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 23] = video::S3DVertex(+Constants::BLOCK_SIZE + block->where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ block->where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + block->where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, block->texture_coords.X*tstep.X + tstep.X,block->texture_coords.Y*tstep.Y + tstep.Y);

                for (int side = 0; side < 6; side++) {
                        _meshBuffer->Indices[i * 36 + side * 6 + 0] = i * 24 + side * 4 + 0;
                        _meshBuffer->Indices[i * 36 + side * 6 + 1] = i * 24 + side * 4 + 1;
                        _meshBuffer->Indices[i * 36 + side * 6 + 2] = i * 24 + side * 4 + 2;
                        _meshBuffer->Indices[i * 36 + side * 6 + 3] = i * 24 + side * 4 + 2;
                        _meshBuffer->Indices[i * 36 + side * 6 + 4] = i * 24 + side * 4 + 3;
                        _meshBuffer->Indices[i * 36 + side * 6 + 5] = i * 24 + side * 4 + 0;
                }                                                        
                i++;
        }
        _mesh->recalculateBoundingBox();

        _node = _smgr -> addOctreeSceneNode(_mesh, 0, -1, 1024);
        irr::core::vector3df rel_pos = vector3df(_rel_coords.X, _rel_coords.Y, _rel_coords.Z) * Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel_pos, getOrientation());
        _node->setPosition(getPosition() + rel_pos);
        Transforms::orient_node(*_node, getOrientation());
        _node->setID(_building_id);

        _node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);
        _node->setAutomaticCulling(EAC_FRUSTUM_SPHERE);
        _node->setMaterialFlag(EMF_LIGHTING, false);
        _node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true); 
        _node->setMaterialTexture(0, TextureLoader::get(texture_id));
        _selector = _smgr->createOctreeTriangleSelector(_mesh, _node, 128);  
}

