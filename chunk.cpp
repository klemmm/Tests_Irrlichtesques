#include "common.h"


#include "building.h"
#include "assets.h"
#include "transforms.h"
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

irr::s32 Chunk::_next_id = 0;
std::map<irr::s32, Chunk*>  Chunk::_map;

Chunk::Chunk(irr::scene::ISceneManager *smgr, Building* building, const irr::core::vector3di &rel_coords) : Entity(smgr), _building(building), _rel_coords(rel_coords), _blocks(vector3dfp(0, 0, 0), vector3dfp(8, 8, 8)), _nblocks(0) {
        
        _mesh = new SMesh();
        _meshBuffer = new SMeshBuffer();       
        _mesh->addMeshBuffer(_meshBuffer);
        _node = nullptr;  
        _selector = nullptr;       
        _chunk_id = _next_id;
        _next_id++;
        _map[_chunk_id] = this;
}


void Chunk::updateMesh(const TextureId &texture_id, bool _hilight, const irr::core::vector3di &_hilightedBlock) {
       // printf("Chunk::updateMesh rel_coords=%d %d %d\n", _rel_coords.X, _rel_coords.Y, _rel_coords.Z);
        if (_node != nullptr)
                _node->remove();

        _meshBuffer->Vertices.reallocate(24 * _nblocks);
        _meshBuffer->Vertices.set_used(24 * _nblocks);

        _meshBuffer->Indices.reallocate(36 * _nblocks);
        _meshBuffer->Indices.set_used(36 * _nblocks);

        int i = 0;
        
        vector2df tstep = TextureLoader::getStep(texture_id);
        if (_selector != nullptr)
                _selector->drop();
        
        OctreeNodeIterator octiter = OctreeNodeIterator(_blocks);
        while (std::shared_ptr<Block> block = std::static_pointer_cast<Block>(octiter.next())) {
                SColor color = (_hilight && (_hilightedBlock == block->where)) ? SColor(255,255,255,255) : SColor(255,128,128,128);
               // printf("put block in mesh\n");
                
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
        vector3dfp rel_pos = vector3dfp(_rel_coords.X, _rel_coords.Y, _rel_coords.Z) * Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel_pos, _building->getOrientation());
        vector3dfp newPos = _building->getPosition() + rel_pos;

        _node->setPosition(irr::core::vector3df(newPos.X, newPos.Y, newPos.Z));

        Transforms::orient_node(*_node, _building->getOrientation());
        //printf("node pos: %f %f %f, node rotation: %f %f %f, rel_coords=%d %d %d\n", _node->getPosition().X, _node->getPosition().Y, _node->getPosition().Z, _node->getRotation().X, _node->getRotation().Y, _node->getRotation().Z,_rel_coords.X, _rel_coords.Y, _rel_coords.Z);
        fflush(stdout);
        _node->setID(_chunk_id);

        _node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);
        _node->setAutomaticCulling(EAC_FRUSTUM_SPHERE);
        _node->setMaterialFlag(EMF_LIGHTING, false);
        _node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true); 
        _node->setMaterialTexture(0, TextureLoader::get(texture_id));

        _selector = _smgr->createOctreeTriangleSelector(_mesh, _node, 128);  
}

void Chunk::process(float delta) {

}

void Chunk::updatePositionAndOrientation(void) {
        vector3dfp rel_pos = vector3dfp(_rel_coords.X, _rel_coords.Y, _rel_coords.Z) * Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel_pos, _building->getOrientation());
        vector3dfp newPos = _building->getPosition() + rel_pos;
        _node->setPosition(irr::core::vector3df(newPos.X, newPos.Y, newPos.Z));
        Transforms::orient_node(*_node, _building->getOrientation());
}

Chunk* Chunk::getChunkFromId(irr::s32 id) {
        if (Chunk::_map.find(id) != Chunk::_map.end()) {
                return Chunk::_map[id];
        } else return nullptr;
}


bool Chunk::bonk(std::shared_ptr<Chunk> other) {
        OctreeNodeIterator octiter = OctreeNodeIterator(_blocks);
        

        vector3dfp rel_pos = vector3dfp(_rel_coords.X, _rel_coords.Y, _rel_coords.Z) * Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel_pos, _building->getOrientation());
        vector3dfp abs_pos = _building->getPosition() + rel_pos;

        vector3dfp rel_pos2 = vector3dfp(other->_rel_coords.X, other->_rel_coords.Y, other->_rel_coords.Z) * Constants::BLOCK_SIZE*2;
        Transforms::rotate(rel_pos2, other->_building->getOrientation());
        vector3dfp abs_pos2 = other->_building->getPosition() + rel_pos2;

        int i, j;
        i = 0;
        while (std::shared_ptr<Block> block = std::static_pointer_cast<Block>(octiter.next())) {

                vector3dfp rel = vector3dfp(block->where.X, block->where.Y, block->where.Z)*Constants::BLOCK_SIZE*2;
                Transforms::rotate(rel, _building->getOrientation());
                vector3dfp center = abs_pos + rel;

                vector3dfp rel2 = center - abs_pos2;
                irr::core::quaternion rot2 = other->_building->getOrientation();
                rot2.makeInverse();
                Transforms::rotate(rel2, rot2);
                rel2 /= (Constants::BLOCK_SIZE*2);
                


                //printf("bonking %d pos %f,%f,%f rel2 %f %f %f\n", i, center.X, center.Y, center.Z, rel2.X, rel2.Y, rel2.Z);
                vector3dfp radius = vector3dfp(1.5f, 1.5f, 1.5f);
                vector3dfp corner1 = vector3dfp(rel2 - radius);
                vector3dfp corner2 = vector3dfp(rel2 + radius);
                OctreeBoundedNodeIterator octiter2 = OctreeBoundedNodeIterator(other->_blocks, corner1, corner2);  
                j = 0;      
                while (std::shared_ptr<Block> block2 = std::static_pointer_cast<Block>(octiter2.next())) {
                        //printf("testing with abspos=%f %f %f, abspos2= %f %f %f\n", abs_pos.X, abs_pos.Y, abs_pos.Z, abs_pos2.X, abs_pos2.Y, abs_pos2.Z);
                        fflush(stdout);
                        if (block->bonk(abs_pos, abs_pos2, _building->getOrientation(), other->_building->getOrientation(), block2)) return true;
                        j++;
                }
                i++;
        }
        return false;
}