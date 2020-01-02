#include "window.h"

Window::Window(const uint32_t Width, const uint32_t Height, const std::string & Name) : m_width(Width), m_height(Height) {
    m_window = nullptr;
    m_renderer = nullptr;
    m_texture = nullptr;

    // Initialize SDL Video module
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow(Name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_ALLOW_HIGHDPI);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);
}

Window::~Window() {
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyTexture(m_texture);

    m_window = nullptr;
    m_renderer = nullptr;
    m_texture = nullptr;

    SDL_Quit();
}