#include "renderer.h"

#include <algorithm>

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

void Renderer::TestLines() {
    const glm::vec4 RED = glm::vec4(255, 0, 0, 255);
    const glm::vec4 GREEN = glm::vec4(0, 255, 0, 255);
    const glm::vec4 BLUE = glm::vec4(0, 0, 255, 255);
    const glm::vec4 WHITE = glm::vec4(255, 255, 255, 255);

    // Draw 8 regions (octants)
    DrawLine(glm::vec2(RendererViewport.Width - 1, RendererViewport.Height - 1), glm::vec2(0.0f, 0.0f), RED);
    DrawLine(glm::vec2(RendererViewport.Width - 1, 0.0f), glm::vec2(0.0f, RendererViewport.Height - 1), RED);
    DrawLine(glm::vec2(RendererViewport.Width - 1, RendererViewport.Height / 2), glm::vec2(0.0f, RendererViewport.Height / 2), RED);
    DrawLine(glm::vec2(RendererViewport.Width / 2, RendererViewport.Height - 1), glm::vec2(RendererViewport.Width / 2, 0.0f), RED);
}

void Renderer::DrawMesh(Mesh * Mesh, const glm::mat4 & Matrix) {
    int32_t TotalTriangles = Mesh->IndexBuffer.size() / 3;

    // Iterate all triangles
    for (int32_t i = 0; i < TotalTriangles; i++) {
        // Fetch triangle attributes
        glm::vec3 v0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].Position;
        glm::vec3 v1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].Position;
        glm::vec3 v2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].Position;

        glm::vec3 n0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].Normal;
        glm::vec3 n1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].Normal;
        glm::vec3 n2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].Normal;

        glm::vec2 uv0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].TexCoords;
        glm::vec2 uv1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].TexCoords;
        glm::vec2 uv2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].TexCoords;

        // Execute Vertex Shader for each vertex (to Clip Space)
        glm::vec4 v0Clip = VertexShader(v0, Matrix);
        glm::vec4 v1Clip = VertexShader(v1, Matrix);
        glm::vec4 v2Clip = VertexShader(v2, Matrix);

        // Perspective divide and convert to NDC
        float OneOverW0 = 1.0f / v0Clip.w;
        float OneOverW1 = 1.0f / v1Clip.w;
        float OneOverW2 = 1.0f / v2Clip.w;

        glm::vec3 v0NDC = v0Clip * OneOverW0;
        glm::vec3 v1NDC = v1Clip * OneOverW1;
        glm::vec3 v2NDC = v2Clip * OneOverW2;

        // Viewport transform (to Raster Space)
        glm::vec2 v0Raster = { (v0NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v0NDC.y) * RendererViewport.Height * 0.5f };
        glm::vec2 v1Raster = { (v1NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v1NDC.y) * RendererViewport.Height * 0.5f };
        glm::vec2 v2Raster = { (v2NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v2NDC.y) * RendererViewport.Height * 0.5f };

        // Perform back-face culling
        float Area = EdgeFunction(v0Raster, v1Raster, v2Raster);
        if (Area < 0)
            continue;

        Area = 1.0f / Area;

        // Viewport clipping
        Viewport BoundingBox;
        if (!IsInsideViewport(v0Raster, v1Raster, v2Raster, BoundingBox))
            continue;

        //DrawLine(v0Raster, v1Raster, glm::vec4(255, 255, 255, 255));
        //DrawLine(v1Raster, v2Raster, glm::vec4(255, 255, 255, 255));
        //DrawLine(v2Raster, v0Raster, glm::vec4(255, 255, 255, 255));

        // Triangle traversal
        for (uint32_t j = BoundingBox.Y; j <= BoundingBox.Height; j++) {
            for (uint32_t i = BoundingBox.X; i <= BoundingBox.Width; i++) {
                glm::vec2 P = { i + 0.5f, j + 0.5f };

                float e0 = EdgeFunction(v1Raster, v2Raster, P);
                float e1 = EdgeFunction(v2Raster, v0Raster, P);
                float e2 = EdgeFunction(v0Raster, v1Raster, P);

                // Check if pixel is inside triangle
                if (e0 >= 0 && e1 >= 0 && e2 >= 0) {
                    e0 *= Area;
                    e1 *= Area;
                    e2 *= Area;

                    // Perspective correct depth interpolation
                    // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
                    float Depth = (e0 * v0NDC.z) + (e1 * v1NDC.z) + (e2 * v2NDC.z);

                    // Depth testing
                    if (Depth < DepthBuffer[j * RendererViewport.Width + i]) {
                        DepthBuffer[j * RendererViewport.Width + i] = Depth;

                        // Perspective correct barycentric coordinates
                        float f0 = e0 * OneOverW0;
                        float f1 = e1 * OneOverW1;
                        float f2 = e2 * OneOverW2;

                        float u = f0 / (f0 + f1 + f2);
                        float v = f1 / (f0 + f1 + f2);

                        glm::vec2 uv = u * uv0 + v * uv1 + (1.0f - u - v) * uv2;
                        glm::vec3 Fragment = Mesh->MeshTexture->Sample(uv);

                        FrameBuffer[j * RendererViewport.Width + i] = glm::vec4(Fragment, 255);
                    }
                }
            }
        }
    }
}

void Renderer::DrawWireframe(Mesh * Mesh, const glm::mat4 & Matrix) {
    int32_t TotalTriangles = Mesh->IndexBuffer.size() / 3;

    // Iterate all triangles
    for (int32_t i = 0; i < TotalTriangles; i++) {
        // Fetch triangle attributes
        glm::vec3 v0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].Position;
        glm::vec3 v1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].Position;
        glm::vec3 v2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].Position;

        glm::vec3 n0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].Normal;
        glm::vec3 n1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].Normal;
        glm::vec3 n2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].Normal;

        glm::vec2 uv0 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3]].TexCoords;
        glm::vec2 uv1 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 1]].TexCoords;
        glm::vec2 uv2 = Mesh->VertexBuffer[Mesh->IndexBuffer[i * 3 + 2]].TexCoords;

        // Execute Vertex Shader for each vertex (to Clip Space)
        glm::vec4 v0Clip = VertexShader(v0, Matrix);
        glm::vec4 v1Clip = VertexShader(v1, Matrix);
        glm::vec4 v2Clip = VertexShader(v2, Matrix);

        // Perspective divide and convert to NDC
        float OneOverW0 = 1.0f / v0Clip.w;
        float OneOverW1 = 1.0f / v1Clip.w;
        float OneOverW2 = 1.0f / v2Clip.w;

        glm::vec3 v0NDC = v0Clip * OneOverW0;
        glm::vec3 v1NDC = v1Clip * OneOverW1;
        glm::vec3 v2NDC = v2Clip * OneOverW2;

        // Viewport transform (to Raster Space)
        glm::vec2 v0Raster = { (v0NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v0NDC.y) * RendererViewport.Height * 0.5f };
        glm::vec2 v1Raster = { (v1NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v1NDC.y) * RendererViewport.Height * 0.5f };
        glm::vec2 v2Raster = { (v2NDC.x + 1) * RendererViewport.Width * 0.5f, (1 - v2NDC.y) * RendererViewport.Height * 0.5f };

        // Perform back-face culling
        float Area = EdgeFunction(v0Raster, v1Raster, v2Raster);
        if (Area < 0)
            continue;

        // Viewport clipping
        Viewport BoundingBox;
        if (!IsInsideViewport(v0Raster, v1Raster, v2Raster, BoundingBox))
            continue;

        // TODO: Crashes if it's outside viewport
        DrawLine(v0Raster, v1Raster, glm::vec4(255, 255, 255, 255));
        DrawLine(v1Raster, v2Raster, glm::vec4(255, 255, 255, 255));
        DrawLine(v2Raster, v0Raster, glm::vec4(255, 255, 255, 255));
    }
}

inline glm::vec4 Renderer::VertexShader(const glm::vec3 & Vertex, const glm::mat4 & T) {
    return T * glm::vec4(Vertex, 1);
}

inline bool Renderer::IsInsideViewport(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2, Viewport & BoundingBox) {
    float xMax = std::max( { v0.x, v1.x, v2.x } );
    float xMin = std::min( { v0.x, v1.x, v2.x } );
    float yMax = std::max( { v0.y, v1.y, v2.y } );
    float yMin = std::min( { v0.y, v1.y, v2.y } );

    // Outside of viewport
    if (xMin + 1 > RendererViewport.Width || xMax < 0 || yMin + 1 > RendererViewport.Height || yMax < 0)
        return false;

    // Compute triangle bounding box
    BoundingBox.X = std::max<uint32_t>(0, (uint32_t)xMin);
    BoundingBox.Y = std::max<uint32_t>(0, (uint32_t)yMin);
    BoundingBox.Width = std::min<uint32_t>(RendererViewport.Width - 1, (uint32_t)xMax);
    BoundingBox.Height = std::min<uint32_t>(RendererViewport.Height - 1, (uint32_t)yMax);

    return true;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
inline float Renderer::EdgeFunction(const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}