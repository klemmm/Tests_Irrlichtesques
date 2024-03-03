#ifndef TRANSFORMS_H
#define TRANSFORMS_H 1
#include <irrlicht/irrlicht.h>

#define POINT_BITS 16

class fpnum {
public:
    inline fpnum(void) : _val(0) {}

    inline fpnum(const irr::s64 &val) {
        _val = val << POINT_BITS;
    }
    inline fpnum(const int &val) {
        _val = val << POINT_BITS;
    }    

    inline fpnum(const irr::f64 &val) {
        _val = val * (1 << POINT_BITS);
    }    

    inline irr::s64 getInt(void) const {
        return _val >> POINT_BITS;
    }

    inline irr::s64 getVal(void) const {
        return _val;
    }

    inline operator float(void) const {
        return ((irr::f64)_val) / ((irr::f64) (1 << POINT_BITS));
    }

    inline operator double(void) const {
        return ((irr::f64)_val) / ((irr::f64) (1 << POINT_BITS));
    }    
    inline operator int(void) const {
        return _val >> POINT_BITS;
    }      

    inline fpnum operator+=(const fpnum &other) {
        fpnum res;
        res._val = _val + other._val;
        _val = res._val;
        return res;
    }

    inline fpnum operator+(const fpnum &other) const {
        fpnum res;
        res._val = _val + other._val;
        return res;
    }

    inline fpnum operator-(const fpnum &other) const {
        fpnum res;
        res._val = _val - other._val;
        return res;
    }

    inline fpnum operator+(const int &other) const {
        fpnum res;
        res._val = _val + (other << POINT_BITS);
        return res;
    }

    inline fpnum operator+=(const int &other) {
        fpnum res;
        res._val = _val + (other << POINT_BITS);
        _val = res._val;
        return res;
    }    

    inline fpnum operator-(const int &other) const {
        fpnum res;
        res._val = _val - (other << POINT_BITS);
        return res;
    }


    inline fpnum operator-(void) const {
        fpnum res;
        res._val = -_val;
        return res;
    }    


    inline fpnum operator-=(const fpnum &other) {
        fpnum res;
        res._val = _val - other._val;
        _val = res._val;
        return res;
    }

    inline fpnum operator-=(const int &other) {
        fpnum res;
        res._val = _val - (other << POINT_BITS);
        _val = res._val;
        return res;
    }



    inline fpnum operator*(const fpnum &other) const {
        fpnum res;
        res._val = (_val * other._val) >> POINT_BITS;
        return res;
    }


    inline fpnum operator*(const int &other) const {
        fpnum res;
        res._val = _val * other;
        
        return res;
    }    

    inline fpnum operator/(const fpnum &other) const {
        fpnum res;
        res._val = (_val << POINT_BITS) / other._val;
        return res;
    }    

    inline fpnum operator/(const int &other) const {
        fpnum res;
        res._val = _val / other;
        return res;
    }        



private:    
    irr::s64 _val;
};


typedef irr::core::vector3d<fpnum> vector3dfp;



class Transforms {
public:
    static void rotate(irr::core::vector3df &vec, const irr::core::quaternion &rot);

    static void rotate(vector3dfp &vec, const irr::core::quaternion &rot);

    static void orient_node(irr::scene::ISceneNode &node, const irr::core::quaternion &rot);
};
#endif