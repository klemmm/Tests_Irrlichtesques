#ifndef WORLD_H
#define WORLD_H 1

#include <memory>
#include <fcntl.h>
#include <irrlicht/irrlicht.h>
#include "kbd_status.h"
#include "building.h"
#include "octree.h"
#include "hud.h"

class World {

public:

    World(irr::scene::ISceneManager*);
    void process(float, Hud &hud, IKkbdStatus&, const irr::core::vector2df&);

    inline bool isInvOpen(void) { return _invOpen; }

    inline void end(void) {
        int fd = open("ship.dat", O_CREAT|O_TRUNC|O_WRONLY, 0644);
        _me->save(fd);
        close(fd);




    }

private:
    void position_and_orient_camera(const irr::core::vector3df &position, const irr::core::quaternion &orientation);

    irr::scene::ISceneManager *_smgr;

    irr::scene::ICameraSceneNode *_camera;

    irr::core::quaternion _camera_orientation;
    irr::core::vector3df _camera_position;

    Octree _buildings;

    bool _wasLeftPressed;
    bool _wasRightPressed;
    bool _wasKeyPressed;
    bool _invOpen;

    int _held;
    std::weak_ptr<Building> _boarded;
    bool _piloting;
    std::weak_ptr<Building> _hilighted_building; /* building containing block currently hilighted */
    irr::core::vector3di _hilighted_block; /* block currently hilighted */

    Building *_me;
};

#endif