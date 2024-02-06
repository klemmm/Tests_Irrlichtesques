#include <iostream>
#include <irrlicht/irrlicht.h>
#include <irrlicht/quaternion.h>

#include <unistd.h>

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

#define ROTATION_SPEED 1.0
#define SPEED 200.0

static void rotate(vector3df &vec, const quaternion &rot) {
    quaternion rot_inv = rot;
    rot_inv.makeInverse();
    quaternion rotated = rot * quaternion(vec.X, vec.Y, vec.Z, 0.0) * rot_inv;
    vec = vector3df(rotated.X, rotated.Y, rotated.Z);
}

class MyEventReceiver : public IEventReceiver
{
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
    
    MyEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


int main(void) {

    MyEventReceiver receiver;
    IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(1600, 900), 16, false, false, true, &receiver);

    if (!device)
        return 1;
    
    device->setWindowCaption(L"Test");

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();



    device->getFileSystem()->addFileArchive("../assets/map-20kdm2.pk3");
    scene::IAnimatedMesh* mapmesh = smgr->getMesh("20kdm2.bsp");
    if (!mapmesh) {
        device->drop();
        return 1;
    }
    auto node = smgr->addOctreeSceneNode(mapmesh->getMesh(0), 0, -1, 1024);
    node->setPosition(core::vector3df(-1300,-144,-1249));
    


    auto autocam = smgr->addCameraSceneNode();
    ICursorControl *cc = device->getCursorControl();
    cc->setVisible(false);




    
    quaternion camera_orientation;
    vector3df camera_position;

    u32 then = device->getTimer()->getTime();

    while(device->run()) {
        const u32 now = device->getTimer()->getTime();
        const f32 delta = (f32)(now - then) / 1000.f;
        then = now; 

        vector2df rel_pos = cc->getRelativePosition();
        cc->setPosition(0.5f, 0.5f);
       
        vector3df movement;
        quaternion rotation;

        if (receiver.isKeyDown(irr::KEY_KEY_Z)) {
            movement.Z += delta * SPEED;
        }
         if (receiver.isKeyDown(irr::KEY_KEY_S)) {
            movement.Z -= delta * SPEED;
        }      
         if (receiver.isKeyDown(irr::KEY_KEY_Q)) {
            movement.X -= delta * SPEED;
        } 
         if (receiver.isKeyDown(irr::KEY_KEY_D)) {
            movement.X += delta * SPEED;
        }    
         if (receiver.isKeyDown(irr::KEY_SPACE)) {
            movement.Y += delta * SPEED;
        }  
         if (receiver.isKeyDown(irr::KEY_LSHIFT)) {
            movement.Y -= delta * SPEED;
        }

        if (rel_pos.X < 0.5f) {
            rotation.fromAngleAxis(ROTATION_SPEED*(0.5f - rel_pos.X), vector3df(0.0, 1.0, 0.0));
        }  
        if (rel_pos.X > 0.5f) {
            rotation.fromAngleAxis(ROTATION_SPEED*(0.5f - rel_pos.X), vector3df(0.0, 1.0, 0.0));
        } 
        camera_orientation = camera_orientation * rotation;

        if (rel_pos.Y < 0.5f) {
            rotation.fromAngleAxis(ROTATION_SPEED*(0.5f - rel_pos.Y), vector3df(1.0, 0.0, 0.0));
        }  
        if (rel_pos.Y > 0.5f) {
            rotation.fromAngleAxis(ROTATION_SPEED*(0.5f - rel_pos.Y), vector3df(1.0, 0.0, 0.0));
        }     
        camera_orientation = camera_orientation * rotation;    


        if (receiver.isKeyDown(irr::KEY_KEY_A)) {
            rotation.fromAngleAxis(-ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
        }  

        if (receiver.isKeyDown(irr::KEY_KEY_E)) {
            rotation.fromAngleAxis(ROTATION_SPEED*delta, vector3df(0.0, 0.0, 1.0));
        }  
        camera_orientation = camera_orientation * rotation;    
       
        auto forward = vector3df(0, 0, 1);
        auto up = vector3df(0, 1, 0);

        
        rotate(forward, camera_orientation);
        rotate(up, camera_orientation);      
        rotate(movement, camera_orientation);  
        camera_position = camera_position + movement;
  
        autocam->setPosition(camera_position);
        autocam->setTarget(camera_position + forward);
        autocam->setUpVector(up);

        driver->beginScene();

        smgr->drawAll();
        guienv->drawAll();

        driver->endScene();

    }
    device->drop();
}

