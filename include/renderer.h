#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

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
        void DrawTriangles();

        const std::vector<glm::vec4> & GetFrameBuffer() { return FrameBuffer; }

    private:
        Viewport RendererViewport;

        std::vector<glm::vec4> FrameBuffer;
        std::vector<float> DepthBuffer;
};

#endif