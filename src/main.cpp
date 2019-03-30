#include <iostream>
#include <sdl2/SDL.h>

#include <glm/glm.hpp>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char **argv) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * Window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture * Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event Event;

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

        // Clear
        SDL_SetRenderDrawColor(Renderer, 43, 53, 56, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(Renderer);

        SDL_RenderPresent(Renderer);
    }

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}