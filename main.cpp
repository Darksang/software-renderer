#include <iostream>
#include <SDL.h>

#include "Vector2i.h"
#include "Vector3f.h"

int main(int argc, char ** argv) {
    
    SDL_Init(SDL_INIT_VIDEO);

    Vector2i v0(2, 3);
    Vector3f v1(3, 5, 8);

    std::cout << "x: " << v0.x << " y: " << v0.y << std::endl;
    std::cout << "x: " << v1.x << " y: " << v1.y << " z: " << v1.z << std::endl;

    v1 *= 2;

    std::cout << "x: " << v1.x << " y: " << v1.y << " z: " << v1.z << std::endl;

    Vector3f v2(7, 2, 8);

    float dot = Dot(v1, v2);

    std::cout << "Dot Product v1/v2: " << dot << std::endl;

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

        // TODO: Render frame
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);

    SDL_Quit();

    return 0;
}