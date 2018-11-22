#include <iostream>
#include "SDL.h"

int main(int argc, char ** argv) {
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * Window = SDL_CreateWindow("Testing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(Renderer);
    SDL_RenderPresent(Renderer);

    SDL_Event Event;
    
    bool Running = true;

    // Main loop
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

        // TODO: Render frame
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);

    SDL_Quit();

    return 0;
}