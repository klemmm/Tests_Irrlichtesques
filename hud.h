#ifndef HUD_H
#define HUD_H 1

#include "common.h"

#define STATUS_MAX 64

class Hud {
public:
    Hud(irr::gui::IGUIEnvironment *guienv);

    void process(float delta);

    void setStatus(const char *txt);

    void draw(irr::video::IVideoDriver*);

    void barSelect(int);

    void setInventory(bool invOpen) { _invOpen = invOpen; }
private:
    irr::gui::IGUIEnvironment *_guienv;

    irr::gui::IGUIStaticText *_status;

    wchar_t _buf[STATUS_MAX];
    int _selectedInBar;
    bool _invOpen;
};

#endif

