#include <iostream>
#include <sdl2/SDL.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "window.h"
#include "renderer.h"
#include "mesh.h"

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;

static const float FOV = 80.0f;

const glm::vec4 RED = glm::vec4(255, 0, 0, 255);
const glm::vec4 GREEN = glm::vec4(0, 255, 0, 255);
const glm::vec4 BLUE = glm::vec4(0, 0, 255, 255);
const glm::vec4 WHITE = glm::vec4(255, 255, 255, 255);

/*void TestLines() {
    // Draw 8 regions (octants)
    DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(639.0f, 479.0f), GREEN);
    DrawLine(glm::vec2(0.0f, 479.0f), glm::vec2(639.0f, 0.0f), GREEN);
    DrawLine(glm::vec2(0.0f, 240.0f), glm::vec2(639.0f, 240.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 0.0f), glm::vec2(320.0f, 479.0f), GREEN);

    DrawLine(glm::vec2(639.0f, 479.0f), glm::vec2(0.0f, 0.0f), RED);
    DrawLine(glm::vec2(639.0f, 0.0f), glm::vec2(0.0f, 479.0f), RED);
    DrawLine(glm::vec2(639.0f, 240.0f), glm::vec2(0.0f, 240.0f), RED);
    DrawLine(glm::vec2(320.0f, 479.0f), glm::vec2(320.0f, 0.0f), RED);

    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(450.0f, 190.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(380.0f, 30.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(260.0f, 60.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(50.0f, 175.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(90.0f, 290.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(240.0f, 370.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(370.0f, 400.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 240.0f), glm::vec2(500.0f, 320.0f), GREEN);
} */

int main(int argc, char **argv) {
    /*SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * SDLWindow = SDL_CreateWindow("Software Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer * Renderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_RendererFlip Flip = SDL_FLIP_VERTICAL;
    SDL_Texture * Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    */

    // Initialize SDL window
    Window MainWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Software Rasterizer");

    // Initialize software rasterizer
    Viewport V = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Renderer * Rasterizer = new Renderer(V);

    /* Initialize required buffers
    FrameBuffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel)); */

    /* Obj model loading
    tinyobj::attrib_t Attrib;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    std::string Warn, Err;

    tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warn, &Err, "obj/african_head.obj");

    if (!Warn.empty()) {
    std::cout << Warn << std::endl;
    }

    if (!Err.empty()) {
    std::cerr << Err << std::endl;
    }

    std::cout << "Number of Shapes: " << Shapes.size() << std::endl;
    std::cout << "Number of Faces: " << Shapes[0].mesh.num_face_vertices.size() << std::endl; */

    // Model loading end

    // https://vulkan-tutorial.com/Loading_models

    std::vector<Vertex> VertexBuffer;
    std::vector<uint32_t> IndexBuffer;
    std::unordered_map<Vertex, uint32_t> UniqueVertices;

    tinyobj::attrib_t Attributes;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    std::string Warning, Error;

    if (tinyobj::LoadObj(&Attributes, &Shapes, &Materials, &Warning, &Error, "obj/cube.obj")) {
        std::cout << "Number of Vertices = " << (int)(Attributes.vertices.size() / 3) << std::endl;
        std::cout << "Number of Normals = " << (int)(Attributes.normals.size() / 3) << std::endl;
        std::cout << "Number of TexCoords = " << (int)(Attributes.texcoords.size() / 2) << std::endl;
        std::cout << "Number of Shapes = " << Shapes.size() << std::endl;

        for (const auto & Shape : Shapes) {
            for (const auto & Index : Shape.mesh.indices) {
                Vertex V{};

                V.Position = {
                    Attributes.vertices[3 * Index.vertex_index],
                    Attributes.vertices[3 * Index.vertex_index + 1],
                    Attributes.vertices[3 * Index.vertex_index + 2]
                };

                V.Normal = {
                    Attributes.normals[3 * Index.normal_index],
                    Attributes.normals[3 * Index.normal_index + 1],
                    Attributes.normals[3 * Index.normal_index + 2],
                };

                V.TexCoords = {
                    Attributes.texcoords[2 * Index.texcoord_index],
                    Attributes.texcoords[2 * Index.texcoord_index + 1]
                };

                // If this vertex isn't duplicate, add it
                if (UniqueVertices.count(V) == 0) {
                    UniqueVertices[V] = static_cast<uint32_t>(VertexBuffer.size());
                    VertexBuffer.push_back(V); 
                }

                IndexBuffer.push_back(UniqueVertices[V]);
            }
        }

        std::cout << "Vertices = " << VertexBuffer.size() << std::endl;
        std::cout << "Indices = " << IndexBuffer.size() << std::endl;
        std::cout << "Triangles = " << IndexBuffer.size() / 3 << std::endl;
    } else {
        return -1;
    }

    Mesh * Object = new Mesh();
    Object->VertexBuffer = VertexBuffer;
    Object->IndexBuffer = IndexBuffer;

    //

    SDL_Event Event;

    bool Running = true;

    // Camera and projection
    glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 ViewMatrix = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);

    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f);

    while (Running) {
        const auto FrameBegin = std::chrono::high_resolution_clock::now();

        // Process events
        while (SDL_PollEvent(&Event)) {
            switch (Event.type) {
            case SDL_KEYDOWN:
                switch (Event.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    Running = false;
                    break;

                case SDL_SCANCODE_SPACE:
                    break;

                default:
                    break;
                }
                break;

            case SDL_KEYUP:
                break;

            case SDL_QUIT:
                Running = false;
                break;

            default:
                break;
            }
        }

        MainWindow.Clear();

        // glClear
        Rasterizer->Clear(glm::vec4(51.0f, 76.5f, 76.5f, 255));

        // Model
        //glm::mat4 ModelMatrix = glm::rotate(ModelMatrix, 0.025f, glm::vec3(0, 1, 0));
        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        // Model * View
        glm::mat4 MV = ViewMatrix * ModelMatrix;
        // Model * View * Projection
        glm::mat4 MVP = ProjectionMatrix * MV;

        Rasterizer->DrawMesh(Object, MVP);

        /* Rendering
        Rasterizer->DrawPoint(glm::vec2(480.0f, 189.0f), GREEN);
        Rasterizer->DrawPoint(glm::vec2(150.0f, 89.0f), BLUE);
        Rasterizer->DrawPoint(glm::vec2(35.0f, 110.0f), GREEN);
        Rasterizer->DrawPoint(glm::vec2(0.0f, 0.0f), GREEN);

        Rasterizer->DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), BLUE); */

        // Update the window buffer with the new frame
        MainWindow.UpdateFramebuffer(Rasterizer->GetFrameBuffer());
        MainWindow.SwapBuffers();

        /* Invalidate buffers
        memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel));

        // Draw Model Wireframe
        const auto ModelRenderStart = std::chrono::high_resolution_clock::now();
        size_t IndexOffset = 0;

        // Loop over faces
        for (size_t f = 0; f < Shapes[0].mesh.num_face_vertices.size(); f++) {
            int fv = Shapes[0].mesh.num_face_vertices[f];

            glm::vec2 Vertex[3];

            // Loop over vertices
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = Shapes[0].mesh.indices[IndexOffset + v];
                tinyobj::real_t vx = Attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = Attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = Attrib.vertices[3 * idx.vertex_index + 2];

                Vertex[v] = glm::vec2(vx, vy);
            }

            glm::vec2 p0 = glm::vec2( (Vertex[0].x + 1.) * (SCREEN_WIDTH / 2.), (Vertex[0].y + 1) * (SCREEN_HEIGHT / 2.));
            glm::vec2 p1 = glm::vec2( (Vertex[1].x + 1.) * (SCREEN_WIDTH / 2.), (Vertex[1].y + 1) * (SCREEN_HEIGHT / 2.));
            glm::vec2 p2 = glm::vec2( (Vertex[2].x + 1.) * (SCREEN_WIDTH / 2.), (Vertex[2].y + 1) * (SCREEN_HEIGHT / 2.));

            DrawLine(p0, p1, WHITE);
            DrawLine(p1, p2, WHITE);
            DrawLine(p2, p0, WHITE);

            IndexOffset += fv;
        }
        const auto ModelRenderEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Model Render Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(ModelRenderEnd - ModelRenderStart).count() << "ms" << std::endl;

        // Display frame
        SDL_UpdateTexture(Texture, nullptr, FrameBuffer, SCREEN_WIDTH * sizeof(Pixel));

        SDL_RenderCopyEx(Renderer, Texture, nullptr, nullptr, 0, nullptr, Flip);

        SDL_RenderPresent(Renderer);

        const auto FrameEnd = std::chrono::high_resolution_clock::now();
        //std::cout << "Frame Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(FrameEnd - FrameBegin).count() << "ms" << std::endl;
        */
    }

    delete Rasterizer;

    delete Object;

    SDL_Quit();

    return 0;
}