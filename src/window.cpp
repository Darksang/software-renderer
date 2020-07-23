#include "window.h"

#include <algorithm>

Window::Window(uint32_t Width, uint32_t Height, const std::string & Name) : Width(Width), Height(Height) {
    SDLWindow = nullptr;
    Renderer = nullptr;
    Texture = nullptr;

    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    SDLWindow = SDL_CreateWindow(Name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, 0);
    Renderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
    Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, Width, Height);

    // Allocate framebuffer (ARGB8888)
    FrameBuffer.resize(Width * Height * 4);
}

Window::~Window() {
    SDL_DestroyWindow(SDLWindow);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(Texture);

    SDLWindow = nullptr;
    Renderer = nullptr;
    Texture = nullptr;

    FrameBuffer.clear();

    SDL_Quit();
}

void Window::Clear() {
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(Renderer);
    //std::fill(FrameBuffer.begin(), FrameBuffer.end(), 0);
}

void Window::SwapBuffers() {
    SDL_UpdateTexture(Texture, nullptr, FrameBuffer.data(), Width * 4);

    SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
    SDL_RenderPresent(Renderer);
}

void Window::UpdateFramebuffer(const std::vector<glm::vec4> & Buffer) {
    int32_t BufferSize = Width * Height * 4;

#pragma omp parallel for
    for (int32_t i = 0; i < BufferSize; i += 4) {
        glm::vec4 Pixel = Buffer[i / 4];

        FrameBuffer[i] = (uint8_t)(Pixel.b);
        FrameBuffer[i + 1] = (uint8_t)(Pixel.g);
        FrameBuffer[i + 2] = (uint8_t)(Pixel.r);
        FrameBuffer[i + 3] = (uint8_t)(Pixel.a);
    }
}