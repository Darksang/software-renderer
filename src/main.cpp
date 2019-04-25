#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <chrono>

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

const glm::vec4 RED = glm::vec4(255, 0, 0, 255);
const glm::vec4 GREEN = glm::vec4(0, 255, 0, 255);
const glm::vec4 BLUE = glm::vec4(0, 0, 255, 255);

struct Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
}; 

static Pixel * FrameBuffer;

void DrawPoint(glm::vec2 p, glm::vec4 Color) {
    Color = glm::clamp(Color, 0.0f, 255.0f);

    int x = static_cast<int>(p.x);
    int y = static_cast<int>(p.y);

    FrameBuffer[y * SCREEN_WIDTH + x].a = static_cast<uint8_t>(Color.w);
    FrameBuffer[y * SCREEN_WIDTH + x].r = static_cast<uint8_t>(Color.x);
    FrameBuffer[y * SCREEN_WIDTH + x].g = static_cast<uint8_t>(Color.y);
    FrameBuffer[y * SCREEN_WIDTH + x].b = static_cast<uint8_t>(Color.z);
}

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

    int dx = static_cast<int>(p1.x - p0.x);
    int dy = static_cast<int>(p1.y - p0.y);

    // Use bit shifting instead of multiply
    int depsilon = std::abs(dy << 1);
    int epsilon = 0;

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

        epsilon += depsilon;

        if (epsilon > dx) {
            y += (p1.y > p0.y ? 1 : -1);
            epsilon -= (dx << 1);
        }
    }
}

void TestPoints() {
    DrawPoint(glm::vec2(56.0f, 240.0f), RED);
    DrawPoint(glm::vec2(480.0f, 189.0f), GREEN);
    DrawPoint(glm::vec2(86.0f, 75.0f), RED);
    DrawPoint(glm::vec2(150.0f, 89.0f), BLUE);
    DrawPoint(glm::vec2(35.0f, 110.0f), GREEN);
}

void TestLines() {
    /* Draw 8 regions (octants)
    DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(639.0f, 479.0f), GREEN);
    DrawLine(glm::vec2(0.0f, 479.0f), glm::vec2(639.0f, 0.0f), GREEN);
    DrawLine(glm::vec2(0.0f, 240.0f), glm::vec2(639.0f, 240.0f), GREEN);
    DrawLine(glm::vec2(320.0f, 0.0f), glm::vec2(320.0f, 479.0f), GREEN); */

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
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * Window = SDL_CreateWindow("Software Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture * Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event Event;

    // Initialize required buffers
    FrameBuffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
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

        //TestPoints();
        TestLines();

        // Display frame
        SDL_UpdateTexture(Texture, nullptr, FrameBuffer, SCREEN_WIDTH * sizeof(Pixel));

        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);

        SDL_RenderPresent(Renderer);

        const auto FrameEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Frame Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(FrameEnd - FrameBegin).count() << "ms" << std::endl;
    }

    delete [] FrameBuffer;

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}