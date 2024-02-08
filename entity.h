#ifndef ENTITY_H
#define ENTITY_H 1
#include <irrlicht/irrlicht.h>

class Entity {

public:
    Entity(irr::scene::ISceneManager *smgr) : _smgr(smgr) {}
    virtual void process(float) = 0;

protected:
    irr::scene::ISceneManager *_smgr;

};

#endif