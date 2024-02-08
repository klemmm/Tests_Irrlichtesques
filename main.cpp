#include <iostream>
#include <irrlicht/irrlicht.h>
#include <irrlicht/quaternion.h>

#include <unistd.h>

#include "assets.h"
#include "world.h"
#include "constants.h"
#include "kbd_status.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

class KbdStatusReceiver : public IEventReceiver, public IKkbdStatus {
public:
    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        }
        return false;
    }

    virtual bool isKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }
    
    KbdStatusReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

int main(void) {
    KbdStatusReceiver kbdstatus;
    IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(Constants::WIDTH, Constants::HEIGHT), 32, false, true, true, &kbdstatus);

    device->setWindowCaption(L"Tests Irrlichtesques");

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();

    TextureLoader::load(driver);

    ICursorControl *cc = device->getCursorControl();
    cc->setVisible(false);

    World world(smgr);

    u32 then = device->getTimer()->getTime();
    cc->setPosition(0.5f, 0.5f);
    while(device->run()) {
        const u32 now = device->getTimer()->getTime();
        const f32 delta = (f32)(now - then) / 1000.f;
        then = now;        

        if (kbdstatus.isKeyDown(irr::KEY_ESCAPE))
            break;

        vector2df mouse_movement = cc->getRelativePosition();
        cc->setPosition(0.5f, 0.5f);            
        
        world.process(delta, kbdstatus, mouse_movement);

        driver->beginScene();

        smgr->drawAll();
        guienv->drawAll();
        driver->endScene();
    }
    device->drop();
    return 0;
}
