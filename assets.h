#ifndef ASSETS_H
#define ASSETS_H 1
#include <irrlicht/irrlicht.h>

enum TextureId {
    DEFAULT = 0,
    TEXTURE_COUNT = 1,
};

class TextureLoader {
private:
    static irr::video::ITexture *textures[TextureId::TEXTURE_COUNT];
    static const char *filenames[TextureId::TEXTURE_COUNT];
    static const irr::core::vector2df steps[TextureId::TEXTURE_COUNT];

public:    
    static irr::video::ITexture *get(TextureId texture_id);
    static void load(irr::video::IVideoDriver *driver);

    static irr::core::vector2df getStep(TextureId texture_id) {
        return steps[texture_id]; 
    }
};

#endif

