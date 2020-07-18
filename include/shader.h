#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <algorithm>

glm::vec3 LightDirection(1, 1, 1);

struct IShader {
    virtual glm::vec4 Vertex(const glm::vec3 & Vertex, const glm::vec3 & Normal, int idx) = 0;
    virtual glm::vec4 Fragment() = 0;
};

struct GouraudShader : public IShader {
    glm::mat4 MVP, MV;

    glm::vec3 VaryingIntensity;

    glm::vec3 RGB{255, 255, 255};

    glm::vec4 Vertex(const glm::vec3 & Vertex, const glm::vec3 & Normal, int idx) override {
        VaryingIntensity[idx] = std::max(0.0f, glm::dot(Normal, LightDirection));
        return MVP * glm::vec4(Vertex, 1);
    }

    glm::vec4 Fragment() override {
        return glm::vec4(VaryingIntensity * RGB, 255);
    }
};

#endif