#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <vector>

#include <sdl2/SDL.h>

#include <glm/glm.hpp>

class Window {
    public:
        Window(uint32_t Width, uint32_t Height, const std::string & Name);
        ~Window();

        void Clear();
        void SwapBuffers();
        void UpdateFramebuffer(const std::vector<glm::vec4> & Buffer);

    private:
        SDL_Window * SDLWindow;
        SDL_Renderer * Renderer;
        SDL_Texture * Texture;

        std::vector<uint8_t> FrameBuffer;

        uint32_t Width;
        uint32_t Height;
};

#endif