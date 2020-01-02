#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include <sdl2/SDL.h>

class Window {
    public:
        Window(const uint32_t Width, const uint32_t Height, const std::string & Name);
        ~Window();

    private:
        SDL_Window * m_window;
        SDL_Renderer * m_renderer;
        SDL_Texture * m_texture;

        uint32_t m_width;
        uint32_t m_height;
};

#endif