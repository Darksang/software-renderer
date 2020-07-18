#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml
struct Viewport {
    uint32_t X;
    uint32_t Y;

    uint32_t Width;
    uint32_t Height;
};

class Renderer {
    public:
        Renderer(const Viewport & V);
        ~Renderer();

        // glClearColor | glClear
        void Clear(const glm::vec4 & ClearColor);

        // Primitive drawing
        void DrawPoint(const glm::vec2 & p, const glm::vec4 & Color);
        void DrawLine(glm::vec2 p0, glm::vec2 p1, const glm::vec4 & Color);
        void DrawMesh(Mesh * Mesh, const glm::mat4 & Matrix);

        const std::vector<glm::vec4> & GetFrameBuffer() { return FrameBuffer; }

        void TestLines();

    private:
        Viewport RendererViewport;

        std::vector<glm::vec4> FrameBuffer;
        std::vector<float> DepthBuffer;

        inline glm::vec4 VertexShader(const glm::vec3 & Vertex, const glm::mat4 & T);

        inline bool IsInsideViewport(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2, Viewport & BoundingBox);
        inline float EdgeFunction(const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c);
};

#endif