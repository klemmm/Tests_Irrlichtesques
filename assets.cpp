#include "common.h"
#include "assets.h"

irr::video::ITexture *TextureLoader::textures[TextureId::TEXTURE_COUNT];

const char *TextureLoader::filenames[TextureId::TEXTURE_COUNT] = {
    "../assets/texturemap1.jpg",
    "../assets/skybox_up.jpg",
    "../assets/skybox_dn.jpg",
    "../assets/skybox_lt.jpg",
    "../assets/skybox_rt.jpg",
    "../assets/skybox_ft.jpg",
    "../assets/skybox_bk.jpg"
};

const irr::core::vector2df TextureLoader::steps[TextureId::TEXTURE_COUNT] = {
    irr::core::vector2df(1.0/3.0, 1.0/3.0),
    irr::core::vector2df(1, 1),
    irr::core::vector2df(1, 1),
    irr::core::vector2df(1, 1),
    irr::core::vector2df(1, 1),
    irr::core::vector2df(1, 1),
    irr::core::vector2df(1, 1),
};

irr::video::ITexture *TextureLoader::get(TextureId texture_id) {
    return textures[texture_id];
}

 void TextureLoader::load(irr::video::IVideoDriver *driver) {
    for (int i = 0; i < TextureId::TEXTURE_COUNT; i++) {
        textures[i] = driver->getTexture(filenames[i]);
    }
 }

 void FontLoader::load(irr::gui::IGUIEnvironment *guienv) {
    irr::gui::IGUIFont* fnt = guienv->getFont("../assets/fontdefault.png");
    
    guienv->getSkin()->setFont(fnt);
    guienv->getSkin()->setColor(irr::gui::EGDC_BUTTON_TEXT, irr::video::SColor(255, 255, 255, 255));
 }