#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

#include <glm/glm.hpp>

#include "stb_image.h"

enum TextureSampler {
    RGB = 3,
    RGBA = 4
};

class Texture {
    public:
        Texture(const std::string & File);
        ~Texture();

        int32_t Width;
        int32_t Height;
        int32_t NumChannels;

        glm::vec3 Sample(const glm::vec2 & uv) const;

    private:
        stbi_uc * Data;
};

#endif