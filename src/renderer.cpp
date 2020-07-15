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

void Renderer::DrawMesh(Mesh * Mesh, const glm::mat4 & Matrix) {
    int32_t TotalTriangles = Mesh->IndexBuffer.size() / 3;

    // Iterate all triangles
    for (int32_t i = 0; i < TotalTriangles; i++) {
        // Fetch triangle vertices
        glm::vec3 v0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].Position;
        glm::vec3 v1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].Position;
        glm::vec3 v2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].Position;

        // Execute Vertex Shader for each vertex (to Clip Space)
        glm::vec4 v0Clip = VertexShader(v0, Matrix);
        glm::vec4 v1Clip = VertexShader(v1, Matrix);
        glm::vec4 v2Clip = VertexShader(v2, Matrix);

        // Perspective divide and convert to NDC
        float w0 = 1.f / v0Clip.w;
        float w1 = 1.f / v1Clip.w;
        float w2 = 1.f / v2Clip.w;

        glm::vec3 v0NDC = v0Clip * w0;
        glm::vec3 v1NDC = v1Clip * w1;
        glm::vec3 v2NDC = v2Clip * w2;

        // Viewport transform (to Raster Space)
        glm::vec2 v0Raster = { (v0NDC.x + 1) / 2 * RendererViewport.Width, (1 - v0NDC.y) / 2 * RendererViewport.Height };
        glm::vec2 v1Raster = { (v1NDC.x + 1) / 2 * RendererViewport.Width, (1 - v1NDC.y) / 2 * RendererViewport.Height };
        glm::vec2 v2Raster = { (v2NDC.x + 1) / 2 * RendererViewport.Width, (1 - v2NDC.y) / 2 * RendererViewport.Height };

        // Perform back-face culling
        float Area = EdgeFunction(v0Raster, v1Raster, v2Raster);
        if (Area < 0)
            continue;

        // Viewport clipping
        if (!IsInsideViewport(v0Raster, v1Raster, v2Raster))
            continue;

        DrawLine(v0Raster, v1Raster, glm::vec4(255, 255, 255, 255));
        DrawLine(v1Raster, v2Raster, glm::vec4(255, 255, 255, 255));
        DrawLine(v2Raster, v0Raster, glm::vec4(255, 255, 255, 255));
        std::cout << "Drawn" << std::endl;
    }
}

inline glm::vec4 Renderer::VertexShader(const glm::vec3 & P, const glm::mat4 & T) {
    return T * glm::vec4(P, 1);
}

inline bool Renderer::IsInsideViewport(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2) {
    // Compute bounding box
    float MaxX = std::max(v0.x, std::max(v1.x, v2.x));
    float MinX = std::min(v0.x, std::min(v1.x, v2.x));
    float MaxY = std::max(v0.y, std::max(v1.y, v2.y));
    float MinY = std::min(v0.y, std::min(v1.y, v2.y));

    // Outside of viewport
    if (MinX + 1 > RendererViewport.Width || MaxX < 0 || MinY + 1 > RendererViewport.Height || MaxY > 0)
        return false;

    // TODO return bounding box

    return true;
}

inline float Renderer::EdgeFunction(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2) {
    return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
}