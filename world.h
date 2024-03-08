#ifndef WORLD_H
#define WORLD_H 1

#include <memory>
#include <fcntl.h>
#include "common.h"
#include "kbd_status.h"
#include "transforms.h"
#include "building.h"
#include "octree.h"
#include "hud.h"

class World {

public:

    World(irr::scene::ISceneManager*);
    void process(float, Hud &hud, IKkbdStatus&, const irr::core::vector2df&);
    void updateHud(Hud &hud);

    void handleInputs(float delta, Hud &hud, IKkbdStatus &kbd, const irr::core::vector2df& mouse_movement);

    inline bool isInvOpen(void) { return _invOpen; }

    inline void end(void) {
        
        int fd = open("ship.dat", O_CREAT|O_TRUNC|O_WRONLY, 0644);
        _me->save(fd);
        close(fd);
        

    }

private:
    std::shared_ptr<Building> collisionBuilding;
    irr::core::vector3di block_coords, adjacent_block_coords;

    void position_and_orient_camera(const vector3dfp &position, const irr::core::quaternion &orientation);
    void prepareScene(void);
    void updatePointedAt(void);
    void updateOctree(void);
    void processBuildings(float);

    irr::scene::ISceneManager *_smgr;

    irr::scene::ICameraSceneNode *_camera;

    irr::core::quaternion _camera_orientation;
    vector3dfp _camera_position;

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

    std::shared_ptr<Building> _me;
};

#endif