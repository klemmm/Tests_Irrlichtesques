#include <irrlicht/irrlicht.h>

#include "chunk.h"
#include "assets.h"
#include "transforms.h"
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

irr::s32 Chunk::_next_id = 0;
std::map<irr::s32, Chunk*>  Chunk::_map;

Chunk::Chunk(irr::scene::ISceneManager *smgr) : Entity(smgr) {
        
        _mesh = new SMesh();
        _meshBuffer = new SMeshBuffer();       
        _mesh->addMeshBuffer(_meshBuffer);
        _node = nullptr;  
        _selector = nullptr;       
        _hasHilight = false;
        _chunk_id = _next_id;
        _next_id++;
        _map[_chunk_id] = this;
}


void Chunk::updateMesh(const TextureId &texture_id) {
        if (_node != nullptr)
                _node->remove();

        _meshBuffer->Vertices.reallocate(24 * blocks.size());
        _meshBuffer->Vertices.set_used(24 * blocks.size());

        _meshBuffer->Indices.reallocate(36 * blocks.size());
        _meshBuffer->Indices.set_used(36 * blocks.size());

        int i = 0;
        
        vector2df tstep = TextureLoader::getStep(texture_id);
        if (_selector != nullptr)
                _selector->drop();
        
                
        for (auto iter = blocks.begin(); iter != blocks.end(); iter++, i++) {
                SColor color = (_hasHilight && _hilighted == (*iter).where) ? SColor(255,255,255,255) : SColor(255,128,128,128);
                
                // Up
                _meshBuffer->Vertices[i * 24 + 0] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, (*iter).texture_coords.X*tstep.X + 0, (*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 1] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 2] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 3] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,1,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);

                // Down
                _meshBuffer->Vertices[i * 24 + 4] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 5] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 6] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 7] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,-1,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + tstep.Y);

                // Right
                _meshBuffer->Vertices[i * 24 + 8] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 9] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 10] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 11] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 1,0,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);

                // Left
                _meshBuffer->Vertices[i * 24 + 12] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 13] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 14] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 15] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, -1,0,0, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);

                // Back
                _meshBuffer->Vertices[i * 24 + 16] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 17] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 18] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 19] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,1, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);
                
                // Front
                _meshBuffer->Vertices[i * 24 + 20] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + tstep.Y);
                _meshBuffer->Vertices[i * 24 + 21] = video::S3DVertex(-Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, (*iter).texture_coords.X*tstep.X + 0,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 22] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,+Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + 0);
                _meshBuffer->Vertices[i * 24 + 23] = video::S3DVertex(+Constants::BLOCK_SIZE + (*iter).where.X * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE+ (*iter).where.Y * Constants::BLOCK_SIZE * 2,-Constants::BLOCK_SIZE + (*iter).where.Z * Constants::BLOCK_SIZE * 2, 0,0,-1, color, (*iter).texture_coords.X*tstep.X + tstep.X,(*iter).texture_coords.Y*tstep.Y + tstep.Y);

                for (int side = 0; side < 6; side++) {
                        _meshBuffer->Indices[i * 36 + side * 6 + 0] = i * 24 + side * 4 + 0;
                        _meshBuffer->Indices[i * 36 + side * 6 + 1] = i * 24 + side * 4 + 1;
                        _meshBuffer->Indices[i * 36 + side * 6 + 2] = i * 24 + side * 4 + 2;
                        _meshBuffer->Indices[i * 36 + side * 6 + 3] = i * 24 + side * 4 + 2;
                        _meshBuffer->Indices[i * 36 + side * 6 + 4] = i * 24 + side * 4 + 3;
                        _meshBuffer->Indices[i * 36 + side * 6 + 5] = i * 24 + side * 4 + 0;
                }                                                        
        }
        _mesh->recalculateBoundingBox();

        
        _node = _smgr -> addOctreeSceneNode(_mesh, 0, -1, 1024);
        _node->setPosition(_position);
        _node->setID(_chunk_id);
        Transforms::orient_node(*_node, _rotation);

        _node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);
        _node->setAutomaticCulling(EAC_FRUSTUM_SPHERE);
        _node->setMaterialFlag(EMF_LIGHTING, false);
        _node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true); 
        _node->setMaterialTexture(0, TextureLoader::get(texture_id));
        _selector = _smgr->createOctreeTriangleSelector(_mesh, _node, 128);  
}

void Chunk::process(float delta) {

}


Chunk* Chunk::getChunkFromId(irr::s32 id) {
        if (Chunk::_map.find(id) != Chunk::_map.end()) {
                return Chunk::_map[id];
        } else return nullptr;
}
