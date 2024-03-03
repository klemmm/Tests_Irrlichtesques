#include "transforms.h"

void Transforms::rotate(irr::core::vector3df &vec, const irr::core::quaternion &rot) {
    irr::core::quaternion rot_inv = rot;
    rot_inv.makeInverse();
    irr::core::quaternion rotated = rot * irr::core::quaternion(vec.X, vec.Y, vec.Z, 0.0) * rot_inv;
    vec = irr::core::vector3df(rotated.X, rotated.Y, rotated.Z);
}

void Transforms::rotate(vector3dfp &vec, const irr::core::quaternion &rot) {
    irr::core::quaternion rot_inv = rot;
    rot_inv.makeInverse();
    irr::core::quaternion rotated = rot * irr::core::quaternion(vec.X, vec.Y, vec.Z, 0.0) * rot_inv;
    vec = vector3dfp(rotated.X, rotated.Y, rotated.Z);
}

void Transforms::orient_node(irr::scene::ISceneNode &node, const irr::core::quaternion &rot) {
    irr::core::quaternion rot_inv = rot;
    rot_inv.makeInverse();
    irr::core::vector3df euler;
    rot_inv.toEuler(euler);    
    node.setRotation(euler * irr::core::RADTODEG);
}

