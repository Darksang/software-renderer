#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>
#include <chrono>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void DrawLine(glm::vec2 p0, glm::vec2 p1, std::vector<glm::vec4> & FrameBuffer, const glm::vec4 Color) {
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
        if (Steep)
            FrameBuffer[x * SCREEN_WIDTH + y] = Color;
        else
            FrameBuffer[y * SCREEN_WIDTH + x] = Color;

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

    std::vector<unsigned char> ColorBuffer;
    std::vector<glm::vec4> FrameBuffer;

    ColorBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    FrameBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT);

    std::vector<glm::vec4> AuxFrameBuffer;
    AuxFrameBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT);
    glm::vec4 ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    std::fill(AuxFrameBuffer.begin(), AuxFrameBuffer.end(), ClearColor);

    int FrameBufferDimensions = SCREEN_WIDTH * SCREEN_HEIGHT * 4;

    bool Running = true;

    while (Running) {
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
        SDL_SetRenderDrawColor(Renderer, (uint8_t)ClearColor.x, (uint8_t)ClearColor.y, (uint8_t)ClearColor.z, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(Renderer);

        // Invalidate buffer
        const auto InvalidateBegin = std::chrono::high_resolution_clock::now();
        FrameBuffer = AuxFrameBuffer;
        const auto InvalidateEnd = std::chrono::high_resolution_clock::now();

        std::cout << "Invalidate Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(InvalidateEnd - InvalidateBegin).count() << "ms" << std::endl;

        // Test line
        const auto LineBegin = std::chrono::high_resolution_clock::now();
        DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(320.0f, 240.0f), FrameBuffer, glm::vec4(0.239f, 0.952f, 0.070f, 1.0f));
        const auto LineEnd = std::chrono::high_resolution_clock::now();

        std::cout << "Line Draw Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(LineEnd - LineBegin).count() << "ms" << std::endl;

        // Write to ColorBuffer
        const auto WriteColorBegin = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for
        for (int i = 0; i < FrameBufferDimensions; i += 4) {
            glm::vec4 rgba = FrameBuffer[i / 4];
            rgba = glm::clamp(rgba, 0.0f, 1.0f);

            ColorBuffer[i] = (unsigned char)(255 * rgba.b);
            ColorBuffer[i + 1] = (unsigned char)(255 * rgba.g);
            ColorBuffer[i + 2] = (unsigned char)(255 * rgba.r);
            ColorBuffer[i + 3] = (unsigned char)(255 * rgba.a);
        }
        const auto WriteColorEnd = std::chrono::high_resolution_clock::now();

        std::cout << "Write Color Buffer Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(WriteColorEnd - WriteColorBegin).count() << "ms" << std::endl;

        // Display frame
        const auto PresentBegin = std::chrono::high_resolution_clock::now();
        SDL_UpdateTexture(Texture, nullptr, ColorBuffer.data(), SCREEN_WIDTH * 4);
        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);

        SDL_RenderPresent(Renderer);
        const auto PresentEnd = std::chrono::high_resolution_clock::now();

        std::cout << "Present Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(PresentEnd - PresentBegin).count() << "ms" << std::endl << std::endl;
    }

    ColorBuffer.clear();
    FrameBuffer.clear();
    AuxFrameBuffer.clear();

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}