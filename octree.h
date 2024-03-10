#ifndef OCTREE_H
#define OCTREE_H 1

#include <vector>
#include <stack>
#include <algorithm>
#include <memory>
#include "common.h"
#include "transforms.h"

class OctreeNode {
public:
    friend class Octree; /* FIXME */
    inline void setPosition(const vector3dfp position) {
        _position = position;
    }    

    inline vector3dfp getPosition(void) const {
        return _position;
    }
private:
    vector3dfp _position;

};

enum Octant {
    Z_PLUS = 1,
    Y_PLUS = 2,
    X_PLUS = 4,
};


enum OctreeDefaults {
    MAX_SIZE = 1,
    MAX_DEPTH = -1
};

class OctreeIterator;

class Octree
{
    friend class OctreeIterator;
    friend class OctreeBoundedIterator;
public:
    inline Octree(const vector3dfp &corner1, const vector3dfp &corner2) : _corner1(corner1), _corner2(corner2), _max_size(OctreeDefaults::MAX_SIZE), _max_depth(OctreeDefaults::MAX_DEPTH) {
        for (int i = 0; i < 8; i++) {
            _octants[i] = nullptr;
        }
    }
    inline ~Octree(void) {
        //printf("bye\n");
        for (int i = 0; i < 8; i++) {
            if (_octants[i] != nullptr)
                delete _octants[i];
        }
    }

    inline std::vector<std::shared_ptr<OctreeNode> > &getNodes(void) {
        return _nodes;
    }

    inline bool belongsHere(const vector3dfp &point) {
        return ((_corner1.X <= point.X && point.X < _corner2.X) &&
            (_corner1.Y <= point.Y && point.Y < _corner2.Y) &&
            (_corner1.Z <= point.Z && point.Z < _corner2.Z));
    }

    void insert(std::shared_ptr<OctreeNode> node);


    template <typename Predicate>
    inline void erase_if(Predicate p) {
        _nodes.erase(std::remove_if(_nodes.begin(), _nodes.end(), p), _nodes.end());
        for (int i = 0; i < 8; i++) {
            if (_octants[i]) {
                _octants[i]->erase_if(p);
            }
        }
    }
    

    inline std::vector<std::shared_ptr<OctreeNode> >::const_iterator begin(void) {
        return _nodes.begin();
    }

    inline std::vector<std::shared_ptr<OctreeNode> >::const_iterator end(void) {
        return _nodes.end();
    }

    inline bool isSplitted(void) {
        return _octants[0] != nullptr;
    }



private:

    vector3dfp _corner1, _corner2;

    Octree *_octants[8];
    std::vector<std::shared_ptr<OctreeNode> > _nodes;

    int _max_size;
    int _max_depth;
};

class OctreeIterator {
private:
    Octree &_octree;
    std::stack<std::pair<Octree*, int> > _stack;
    bool _me;

public:
    OctreeIterator(Octree &octree);
    Octree *next();
    inline bool hasNext(void) {
        return !_stack.empty();
    }
};

class OctreeBoundedIterator {
private:
    Octree &_octree;
    std::stack<std::pair<Octree*, int> > _stack;
    const vector3dfp &_corner1, &_corner2;
    bool _me;

public:
    OctreeBoundedIterator(Octree &octree, const vector3dfp &corner1, const vector3dfp &corner2);
    Octree *next();
    inline bool hasNext(void) {
        return !_stack.empty();
    }
    inline bool isDisjoint(Octree *octree) {
        return (!octree) || (octree->_corner2.X <= _corner1.X) ||
            (octree->_corner1.X > _corner2.X) ||
            (octree->_corner2.Y <= _corner1.Y) ||
            (octree->_corner1.Y > _corner2.Y) ||
            (octree->_corner2.Z <= _corner1.Z) ||
            (octree->_corner1.Z > _corner2.Z);
    }
};


class OctreeNodeIterator {
   private:
    OctreeIterator _iter;
    Octree *_current;
    std::vector<std::shared_ptr<OctreeNode> >::const_iterator _node_iter;
    bool _empty;
public:
    OctreeNodeIterator(Octree &octree) : _iter(OctreeIterator(octree)), _current(_iter.next())  {
        if (_current) {
            _empty = false;
            _node_iter = _current->begin();
        } else {
            _empty = true;
        }    
    }
    inline std::shared_ptr<OctreeNode> next() {
    
        if (_empty) return nullptr;

        while (_node_iter == _current->end()) {
            _current = _iter.next();
            if (!_current) return nullptr;
            _node_iter = _current->begin();
        }
        std::shared_ptr<OctreeNode> node = *_node_iter;
        _node_iter++;
        return node;
    } 
    inline bool belongsHere(const vector3dfp &point) { return _current->belongsHere(point); }
};

class OctreeBoundedNodeIterator {
   private:
    OctreeBoundedIterator _iter;
    Octree *_current;
    std::vector<std::shared_ptr<OctreeNode> >::const_iterator _node_iter;
    bool _empty;
public:
    OctreeBoundedNodeIterator(Octree &octree, const vector3dfp &corner1, const vector3dfp &corner2) : _iter(OctreeBoundedIterator(octree, corner1, corner2)), _current(_iter.next()) {
        if (_current) {
            _empty = false;
            _node_iter = _current->begin();
        } else {
            _empty = true;
        }
    }

    inline std::shared_ptr<OctreeNode> next() {
        if (_empty) return nullptr;
        while (_node_iter == _current->end()) {
            _current = _iter.next();
            if (!_current) return nullptr;
            _node_iter = _current->begin();
        }
        std::shared_ptr<OctreeNode> node = *_node_iter;
        _node_iter++;
        return node;
    } 

    inline bool belongsHere(const vector3dfp &point) { return _current->belongsHere(point); }
};





#endif