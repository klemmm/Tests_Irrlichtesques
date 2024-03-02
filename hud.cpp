#include "hud.h"
#include "constants.h"
#include "assets.h"
using namespace irr;

Hud::Hud(irr::gui::IGUIEnvironment *guienv) : _guienv(guienv), _selectedInBar(0), _invOpen(false) {
    _status = guienv->addStaticText(L"", irr::core::rect<irr::s32>(100,15,1800,65), false, false);
    
}

void Hud::draw(irr::video::IVideoDriver *driver) {
    irr::video::ITexture* texture = TextureLoader::get(TextureId::DEFAULT);
    irr::core::vector2di stepInPixel;
    irr::core::vector2df step  = TextureLoader::getStep(TextureId::DEFAULT);
    irr::core::dimension2du size = texture->getSize();
    stepInPixel.X = size.Width * step.X;
    stepInPixel.Y = size.Height * step.Y;

    // 604, 900
    for (int i = 0; i < 9; i++) {
        int x = 604 + (64 + 8)*i;
        int y = 900;
        int c = (i == _selectedInBar) ? 255 : 64;
        driver->draw2DRectangle(video::SColor(255,c,c,c), core::rect<s32>(x - 3, y - 3, x + 64 + 6, y + 64 + 6));
        driver->draw2DRectangle(video::SColor(255,0, 0, 0), core::rect<s32>(x, y, x + 64, y + 64));
        int ty = i % 3;
        int tx = i / 3;
        driver->draw2DImage(texture, core::rect<s32>(x, y, x + 64, y + 64), core::rect<s32>(tx*stepInPixel.X, ty*stepInPixel.Y, tx*stepInPixel.X + stepInPixel.X, ty*stepInPixel.Y + stepInPixel.Y) );
    }
    driver->draw2DRectangle(video::SColor(255,255,255,255), core::rect<s32>(Constants::WIDTH/2 - 5, Constants::HEIGHT/2 - 1, Constants::WIDTH/2 + 5, Constants::HEIGHT/2 + 1));
    driver->draw2DRectangle(video::SColor(255,255,255,255), core::rect<s32>(Constants::WIDTH/2 - 1, Constants::HEIGHT/2 - 5, Constants::WIDTH/2 + 1, Constants::HEIGHT/2 + 5));


    if (_invOpen) {
        driver->draw2DRectangle(video::SColor(255,192,192,192), core::rect<s32>(312, 296, 1608, 728));

        for (int x = 0; x < 18; x++) {
            for (int y = 0; y < 6; y++) {
                driver->draw2DRectangle(video::SColor(255,64, 64, 64), core::rect<s32>(316 + x * (64 + 8), 300 + y * (64 + 8), 316 + 64 + x * (64 + 8), 300 + 64 + y * (64 + 8)));

            }
        }
    }

}

void Hud::process(float delta) {

}

void Hud::barSelect(int n) {
    _selectedInBar = n;

}

void Hud::setStatus(const char *txt) {
    
    size_t ret = mbstowcs(_buf, txt, STATUS_MAX - 1);
    if (ret == -1) {
        _buf[0] = L'\0';
    } else {
        _buf[ret] = L'\0';
    }
    
    
    _status->setText(_buf);
}