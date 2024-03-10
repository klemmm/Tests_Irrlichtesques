#ifndef WORLD_H
#define WORLD_H 1

#include <memory>
#include <fcntl.h>
#include <unordered_set>
#include "common.h"
#include "kbd_status.h"
#include "transforms.h"
#include "building.h"
#include "octree.h"
#include "hud.h"
#include "debug.h"

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

    inline ~World() {
        delete _dc_me;
        delete _dc_sta;
    }

private:
    void position_and_orient_camera(const vector3dfp &position, const irr::core::quaternion &orientation);
    void prepareScene(void);
    void updatePointedAt(void);
    void updateOctree(void);
    void processBuildings(float);

    std::shared_ptr<Building> _pointedAtBuilding;
    irr::core::vector3di _pointedAtBlockCoords, _pointedAtAdjacentBlockCoords;

    irr::scene::ISceneManager *_smgr;
    irr::scene::ICameraSceneNode *_camera;

    irr::core::quaternion _relativeCameraOrientation;
    vector3dfp _relativeCameraPosition;

    Octree _buildings;

    bool _wasLeftPressed;
    bool _wasRightPressed;
    bool _wasKeyPressed;
    bool _invOpen;

    int _held;
    std::weak_ptr<Building> _boarded;
    bool _piloting;
    std::weak_ptr<Building> _hilightedBuilding; /* building containing block currently hilighted */
    irr::core::vector3di _hilightedBlock; /* block currently hilighted */

    std::shared_ptr<Building> _me;
    std::shared_ptr<Building> _sta;

    std::unordered_set<std::shared_ptr<Building> > _meshNeedsUpdate;

    DebugCuboid *_dc_me, *_dc_sta;

};

#endif