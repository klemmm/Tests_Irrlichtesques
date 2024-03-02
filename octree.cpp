#include <irrlicht/irrlicht.h>
#include "octree.h"

void Octree::insert(OctreeNode &node) {
    _nodes.push_back(&node);

    if (_nodes.size() > _max_size) {

        irr::core::vector3df middle = (_corner1 + _corner2)/2;
        
        _octants[0] = 
            new Octree(irr::core::vector3df(_corner1.X, _corner1.Y, _corner1.Z), irr::core::vector3df(middle.X, middle.Y, middle.Z));
        _octants[Octant::X_PLUS] = 
            new Octree(irr::core::vector3df(middle.X, _corner1.Y, _corner1.Z), irr::core::vector3df(_corner2.X, middle.Y, middle.Z));
        _octants[Octant::Y_PLUS] = 
            new Octree(irr::core::vector3df(_corner1.X, middle.Y, _corner1.Z), irr::core::vector3df(middle.X, _corner2.Y, middle.Z));
        _octants[Octant::X_PLUS | Octant::Y_PLUS] = 
            new Octree(irr::core::vector3df(middle.X, middle.Y, _corner1.Z), irr::core::vector3df(_corner2.X, _corner2.Y, middle.Z));
        _octants[Octant::Z_PLUS] = 
            new Octree(irr::core::vector3df(_corner1.X, _corner1.Y, middle.Z), irr::core::vector3df(middle.X, middle.Y, _corner2.Z));
        _octants[Octant::Z_PLUS | Octant::X_PLUS] = 
            new Octree(irr::core::vector3df(middle.X, _corner1.Y, middle.Z), irr::core::vector3df(_corner2.X, middle.Y, _corner2.Z));
        _octants[Octant::Z_PLUS | Octant::Y_PLUS] = 
            new Octree(irr::core::vector3df(_corner1.X, middle.Y, middle.Z), irr::core::vector3df(middle.X, _corner2.Y, _corner2.Z));
        _octants[Octant::Z_PLUS | Octant::X_PLUS | Octant::Y_PLUS] = 
            new Octree(irr::core::vector3df(middle.X, middle.Y, middle.Z), irr::core::vector3df(_corner2.X, _corner2.Y, _corner2.Z));
    }

    if (isSplitted()) {
        for (int i = 0; i < 8; i++) {
            for (auto node_iter = _nodes.begin(); node_iter != _nodes.end(); node_iter++) {
                if (_octants[i]->belongsHere((*node_iter)->getPosition())) {
                    _octants[i]->insert(*(*node_iter));
                }
            }
        }
        _nodes.clear();
    } 
}


OctreeBoundedIterator::OctreeBoundedIterator(const Octree &octree, const irr::core::vector3df &corner1, const irr::core::vector3df &corner2) : _octree(octree), _corner1(corner1), _corner2(corner2) {
    _stack.push(std::make_pair<const Octree*, int>(&_octree, 0));
} 

Octree *OctreeBoundedIterator::next(void) {
    const Octree *current; 
    int index;

    do {
        current = _stack.top().first;
        index = _stack.top().second;
        _stack.pop();
        if (index != 8)
            while (isDisjoint(current->_octants[index]) && index != 8) index++;
    } while ((index == 8) && !_stack.empty());


    if (index == 8) return nullptr;

    _stack.push(std::make_pair<const Octree*, int>(std::move(current), index + 1));
    if (current->_octants[index] != nullptr) {        
        _stack.push(std::make_pair<const Octree*, int>(std::move((const Octree*)current->_octants[index]), 0));
    }
    return current->_octants[index];
}

OctreeIterator::OctreeIterator(const Octree &octree) : _octree(octree) {
    _stack.push(std::make_pair<const Octree*, int>(&_octree, 0));
} 

Octree *OctreeIterator::next(void) {
    const Octree *current; 
    int index;

    do {
        current = _stack.top().first;
        index = _stack.top().second;
        _stack.pop();
        if (index != 8)
            while (current->_octants[index] == nullptr && index != 8) index++;
    } while ((index == 8) && !_stack.empty());


    if (index == 8) return nullptr;

    _stack.push(std::make_pair<const Octree*, int>(std::move(current), index + 1));
    if (current->_octants[index] != nullptr) {        
        _stack.push(std::make_pair<const Octree*, int>(std::move((const Octree*)current->_octants[index]), 0));
    }
    return current->_octants[index];
}


Octree *Octree::find(const irr::core::vector3df &point) {
    if (!isSplitted()) {
        if (belongsHere(point)) {
            return this;
        } else return nullptr;
    }

    irr::core::vector3df middle = (_corner1 + _corner2)/2;
    int i = 0;

    i |= (middle.X <= point.X) ? X_PLUS : 0;
    i |= (middle.Y <= point.Y) ? Y_PLUS : 0;
    i |= (middle.Z <= point.Z) ? Z_PLUS : 0;

    return _octants[i]->find(point);
}
