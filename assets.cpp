#include <irrlicht/irrlicht.h>
#include "assets.h"

irr::video::ITexture *TextureLoader::textures[TextureId::TEXTURE_COUNT];

const char *TextureLoader::filenames[TextureId::TEXTURE_COUNT] = {
    "../assets/texturemap1.jpg",
};

const irr::core::vector2df TextureLoader::steps[TextureId::TEXTURE_COUNT] = {
    irr::core::vector2df(1.0/2.0, 1.0),
};

irr::video::ITexture *TextureLoader::get(TextureId texture_id) {
    return textures[texture_id];
}

 void TextureLoader::load(irr::video::IVideoDriver *driver) {
    for (int i = 0; i < TextureId::TEXTURE_COUNT; i++) {
        textures[i] = driver->getTexture(filenames[i]);
    }
 }