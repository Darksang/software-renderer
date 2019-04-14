#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <chrono>

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

struct Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
}; 

static Pixel * FrameBuffer;

void DrawLine(glm::vec2 p0, glm::vec2 p1, glm::vec4 Color) {
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
            FrameBuffer[x * SCREEN_WIDTH + y].a = static_cast<uint8_t>(Color.w);
            FrameBuffer[x * SCREEN_WIDTH + y].r = static_cast<uint8_t>(Color.x);
            FrameBuffer[x * SCREEN_WIDTH + y].g = static_cast<uint8_t>(Color.y);
            FrameBuffer[x * SCREEN_WIDTH + y].b = static_cast<uint8_t>(Color.z);
        } else {
            FrameBuffer[y * SCREEN_WIDTH + x].a = static_cast<uint8_t>(Color.w);
            FrameBuffer[y * SCREEN_WIDTH + x].r = static_cast<uint8_t>(Color.x);
            FrameBuffer[y * SCREEN_WIDTH + x].g = static_cast<uint8_t>(Color.y);
            FrameBuffer[y * SCREEN_WIDTH + x].b = static_cast<uint8_t>(Color.z);
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
    SDL_Texture * Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event Event;

    // Initialize required buffers
    FrameBuffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel));

    bool Running = true;
    bool Swap = true;

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
                    Swap = !Swap;

                    if (Swap)
                        std::cout << "Swap True, Draw Green Line" << std::endl;
                    else
                        std::cout << "Swap False, Draw Blue Line" << std::endl;

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

        // Invalidate buffers
        memset(FrameBuffer, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Pixel));

        // Test line
        if (Swap == true)
            DrawLine(glm::vec2(30.0f, 235.0f), glm::vec2(30.0f, 298.0f), glm::vec4(0, 255, 0, 255));
        else 
            DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(640.0f, 480.0f), glm::vec4(0, 0, 255, 255));

        // Display frame
        if (SDL_UpdateTexture(Texture, nullptr, FrameBuffer, SCREEN_WIDTH * sizeof(Pixel)) < 0)
            std::cout << "Update Texture Error" << std::endl << SDL_GetError() << std::endl;

        if (SDL_RenderCopy(Renderer, Texture, nullptr, nullptr) < 0)
            std::cout << "Render Copy Error" << std::endl << SDL_GetError() << std::endl;

        SDL_RenderPresent(Renderer);
    }

    delete [] FrameBuffer;

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}