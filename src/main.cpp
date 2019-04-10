#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>
#include <chrono>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Pixel {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

void DrawLine(glm::vec2 p0, glm::vec2 p1, Pixel Framebuffer[], glm::vec4 Color) {
    Color = glm::clamp(Color, 0.0f, 255.0f);
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

    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;

    float m = std::abs((dy / dx));
    float error = 0.0f;

    int y = p0.y;

    for (int x = p0.x; x <= p1.x; x++) {
        if (Steep) {
            Framebuffer[x * SCREEN_WIDTH + y].a = static_cast<unsigned char>(Color.w);
            Framebuffer[x * SCREEN_WIDTH + y].r = static_cast<unsigned char>(Color.x);
            Framebuffer[x * SCREEN_WIDTH + y].g = static_cast<unsigned char>(Color.y);
            Framebuffer[x * SCREEN_WIDTH + y].b = static_cast<unsigned char>(Color.z);
        } else {
            Framebuffer[y * SCREEN_WIDTH + x].a = static_cast<unsigned char>(Color.w);
            Framebuffer[y * SCREEN_WIDTH + x].r = static_cast<unsigned char>(Color.x);
            Framebuffer[y * SCREEN_WIDTH + x].g = static_cast<unsigned char>(Color.y);
            Framebuffer[y * SCREEN_WIDTH + x].b = static_cast<unsigned char>(Color.z);
        }

        error += m;

        if (error > 0.5f) {
            y += (p1.y > p0.y ? 1 : -1);
            error -= 1;
        }
    }
}

int main(int argc, char **argv) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * Window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture * Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event Event;

    glm::vec4 ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize required buffers
    Pixel * FrameBuffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel));

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

                case SDL_SCANCODE_SPACE:
                    std::cout << "Space Pressed" << std::endl;
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

        // Clear screen
        SDL_SetRenderDrawColor(Renderer, static_cast<uint8_t>(ClearColor.x), static_cast<uint8_t>(ClearColor.y), static_cast<uint8_t>(ClearColor.z), SDL_ALPHA_OPAQUE);
        SDL_RenderClear(Renderer);

        // Invalidate buffers
        memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel));

        // Test line
        DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(320.0f, 240.0f), FrameBuffer, glm::vec4(0, 0, 255, 0.1f));
        DrawLine(glm::vec2(30.0f, 235.0f), glm::vec2(30.0f, 298.0f), FrameBuffer, glm::vec4(0, 255, 0, 0.1f));

        // Display frame
        SDL_UpdateTexture(Texture, nullptr, FrameBuffer, SCREEN_WIDTH * 4);
        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);

        SDL_RenderPresent(Renderer);

        /* const auto FrameEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Frame Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(FrameEnd - FrameBegin).count() << "ms";
        auto FramesPerSecond = 1000.0f / std::chrono::duration_cast<std::chrono::milliseconds>(FrameEnd - FrameBegin).count();
        std::cout << " FPS: " << (int)FramesPerSecond << std::endl; */
    }

    delete [] FrameBuffer;

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}