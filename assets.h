#ifndef ASSETS_H
#define ASSETS_H 1
#include "common.h"

enum TextureId {
    DEFAULT = 0,
    SKYBOX_UP = 1,
    SKYBOX_DN = 2,
    SKYBOX_LT = 3,
    SKYBOX_RT = 4,
    SKYBOX_FT = 5,
    SKYBOX_BK = 6,
    TEXTURE_COUNT = 7,
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

class FontLoader {
public:
    static void load(irr::gui::IGUIEnvironment *guienv);
};

#endif

