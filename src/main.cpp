#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "window.h"
#include "renderer.h"
#include "mesh.h"
#include "shader.h"

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;

static const float FOV = 45.0f;

int main(int argc, char **argv) {
    // Initialize SDL window
    Window MainWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Software Rasterizer");

    // Initialize software rasterizer
    Viewport Viewport = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Renderer * Rasterizer = new Renderer(Viewport);

    // Load OBJ and texture
    Mesh * Object = new Mesh();
    Object->LoadOBJ("obj/viking_room.obj");
    Object->LoadTexture("textures/viking_room.png");

    // Camera and projection
    glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    GouraudShader SimpleShader;

    SDL_Event Event;

    bool Running = true;

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

                case SDL_SCANCODE_W:
                    CameraPosition += 0.05f * CameraFront;
                    //std::cout << "Camera: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
                    break;

                case SDL_SCANCODE_S:
                    CameraPosition -= 0.05f * CameraFront;
                    //std::cout << "Camera: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
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

        glm::mat4 ViewMatrix = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);

        // Model
        //ViewMatrix = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);
        //glm::mat4 ModelMatrix = glm::rotate(ModelMatrix, 0.025f, glm::vec3(0, 1, 0));
        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        //ModelMatrix = glm::rotate(ModelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
        // Model * View
        glm::mat4 MV = ViewMatrix * ModelMatrix;
        // Model * View * Projection
        glm::mat4 MVP = ProjectionMatrix * MV;

        Rasterizer->DrawMesh(Object, MVP);

        // Update the window buffer with the new frame
        //const auto CopyBufferBegin = std::chrono::high_resolution_clock::now();
        MainWindow.UpdateFramebuffer(Rasterizer->GetFrameBuffer());
        //const auto CopyBufferEnd = std::chrono::high_resolution_clock::now();
        //std::cout << "Copy Buffer Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(CopyBufferEnd - CopyBufferBegin).count() << "ms" << std::endl;
        MainWindow.SwapBuffers();

        const auto FrameEnd = std::chrono::high_resolution_clock::now();
        //std::cout << "Frame Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(FrameEnd - FrameBegin).count() << "ms" << std::endl;
    }

    delete Rasterizer;
    delete Object;

    SDL_Quit();

    return 0;
}