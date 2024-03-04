#ifndef TRANSFORMS_H
#define TRANSFORMS_H 1
#include "common.h"




class Transforms {
public:
    static void rotate(irr::core::vector3df &vec, const irr::core::quaternion &rot);

    static void rotate(vector3dfp &vec, const irr::core::quaternion &rot);

    static void orient_node(irr::scene::ISceneNode &node, const irr::core::quaternion &rot);
};
#endif