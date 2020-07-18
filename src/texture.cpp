#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string & File) {
    Data = stbi_load(File.c_str(), &Width, &Height, &NumChannels, 0);
}

Texture::~Texture() {
    stbi_image_free(Data);
    Data = nullptr;
}

glm::vec3 Texture::Sample(const glm::vec2 & uv) const {
    uint32_t idxS = (uint32_t)glm::floor(uv.s * Width);
    uint32_t idxT = (uint32_t)glm::floor(uv.t * Height);
    uint32_t idx = (idxT * Width + idxS) * NumChannels;

    float r = (float)(Data[idx++]);
    float g = (float)(Data[idx++]);
    float b = (float)(Data[idx++]);

    return glm::vec3(r, g, b);
}