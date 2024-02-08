#ifndef KBD_STATUS_H
#define KBD_STATUS_H 1
#include <irrlicht/IEventReceiver.h>

class  IKkbdStatus {
public:
    virtual bool isKeyDown(irr::EKEY_CODE keyCode) const = 0;
};
#endif