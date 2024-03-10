#ifndef DEBUG_H
#define DEBUG_H 1
#include "common.h"

class DebugCuboid  {
private:
    irr::scene::ISceneNode *_wireNode;
    irr::scene::ISceneNode *_plainNode;
    irr::scene::SMesh *_mesh;
    irr::scene::SMeshBuffer *_meshBuffer;
public:    
    DebugCuboid(irr::scene::ISceneManager *smgr, irr::core::vector3df top, irr::core::vector3df bottom);
    
    inline void setPosition(irr::core::vector3df pos) {
        _wireNode->setPosition(pos);
        _plainNode->setPosition(pos);
    }

    inline void setRotation(irr::core::vector3df rot) {
        _wireNode->setRotation(rot);
        _plainNode->setRotation(rot);
    }

    inline ~DebugCuboid() {      
        _wireNode->remove();
        _plainNode->remove();  
        _meshBuffer->drop();
        _mesh->drop();
    }

};
#endif
