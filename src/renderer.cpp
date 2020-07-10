#include "renderer.h"

Renderer::Renderer(const Viewport & V) : RendererViewport(V) {
    FrameBuffer.resize(V.Width * V.Height);
    DepthBuffer.resize(V.Width * V.Height);
}

Renderer::~Renderer() {
    FrameBuffer.clear();
    DepthBuffer.clear();
}

void Renderer::Clear(const glm::vec4 & ClearColor) {
    std::fill(FrameBuffer.begin(), FrameBuffer.end(), ClearColor);
    std::fill(DepthBuffer.begin(), DepthBuffer.end(), FLT_MAX);
}

void Renderer::DrawPoint(const glm::vec2 & p, const glm::vec4 & Color) {
    int x = static_cast<int>(p.x);
    int y = static_cast<int>(p.y);

    FrameBuffer[y * RendererViewport.Width + x] = glm::clamp(Color, 0.0f, 255.0f);
}

void Renderer::DrawLine(glm::vec2 p0, glm::vec2 p1, const glm::vec4 & Color) {
    // Check if we need to reverse input coordinates
    bool Steep = false;

    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        Steep = true;
    }

    // Make it left-to-right
    if (p0.x > p1.x)
        std::swap(p0, p1);

    int dx = static_cast<int>(p1.x - p0.x);
    int dy = static_cast<int>(p1.y - p0.y);

    // Use bit shifting instead of multiply
    int depsilon = std::abs(dy << 1);
    int epsilon = 0;

    int y = p0.y;

    for (int x = p0.x; x <= p1.x; x++) {
        if (Steep)
            FrameBuffer[x * RendererViewport.Width + y] = glm::clamp(Color, 0.0f, 255.0f);
        else
            FrameBuffer[y * RendererViewport.Width + x] = glm::clamp(Color, 0.0f, 255.0f);

        epsilon += depsilon;

        if (epsilon > dx) {
            y += (p1.y > p0.y ? 1 : -1);
            epsilon -= (dx << 1);
        }
    }
}