#include <iostream>
#include <SDL.h>

#include "Vector2i.h"
#include "Vector3f.h"

#include <vector>
#include <algorithm>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int fbDim = SCREEN_WIDTH * SCREEN_HEIGHT * 3;

Vector3f Red(1.0f, 0, 0);

Vector3f CalculateBarycentric(Vector2i p0, Vector2i p1, Vector2i p2, Vector2i p) {
    Vector3f pf0((float)p0.x, (float)p0.y, 0);
    Vector3f pf1((float)p1.x, (float)p1.y, 0);
    Vector3f pf2((float)p2.x, (float)p2.y, 0);
    Vector3f pf((float)p.x, (float)p.y, 0);

    // Calculate barycentric coordinates using Cramer's rule for solving a linear system
    Vector3f v0 = pf1 - pf0;
    Vector3f v1 = pf2 - pf0;
    Vector3f v2 = pf - pf0;

    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);

    float invDenom = 1.0f / (d00 * d11 - d01 * d01);

    float v = (d11 * d20 - d01 * d21) * invDenom;
    float w = (d00 * d21 - d01 * d20) * invDenom;
    float u = 1.0f - v - w;

    return (Vector3f(v, w, u));
}

void DrawTriangle(Vector2i v0, Vector2i v1, Vector2i v2, std::vector<Vector3f> &fb) {
    // Compute triangle bounding box
    int maxX = std::max(v0.x, std::max(v1.x, v2.x));
    int minX = std::min(v0.x, std::min(v1.x, v2.x));

    int maxY = std::max(v0.y, std::max(v1.y, v2.y));
    int minY = std::min(v0.y, std::min(v1.y, v2.y));

    Vector2i bbox_max(maxX, maxY);
    Vector2i bbox_min(minX, minY);

    Vector2i p;

    for (p.x = bbox_min.x; p.x <= bbox_max.x; p.x++) {
        for (p.y = bbox_min.y; p.y <= bbox_max.y; p.y++) {
            Vector3f bc = CalculateBarycentric(v0, v1, v2, p);

            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;

            fb[p.y * SCREEN_WIDTH + p.x] = Red;
        }
    }
}

int main(int argc, char **argv) {

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

    SDL_Window *Window = SDL_CreateWindow("Testing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);

    SDL_Texture *Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(Renderer);
    SDL_RenderPresent(Renderer);

    // Frame buffer
    std::vector<Vector3f> FrameBuffer;
    FrameBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT);

    std::vector<unsigned char> ColorBuffer;
    ColorBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT * 3);

    // Test
    Vector2i t0[3] = {Vector2i(0, 0), Vector2i(50, 160), Vector2i(70, 80)};

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

        // Render frame

        // Clear renderer
        SDL_SetRenderDrawColor(Renderer, 43, 53, 56, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(Renderer);

        // Reset framebuffer
        Vector3f clearColor(1, 1, 1);
        fill(FrameBuffer.begin(), FrameBuffer.end(), clearColor);

        // Rasterize
        DrawTriangle(t0[0], t0[1], t0[2], FrameBuffer);

        // Display frame
        for (int i = 0; i < fbDim; i += 3) {
            Vector3f rgb = FrameBuffer[i / 3];

            ColorBuffer[i] = (unsigned char)(255 * rgb.z);
            ColorBuffer[i + 1] = (unsigned char)(255 * rgb.y);
            ColorBuffer[i + 2] = (unsigned char)(255 * rgb.x);
        }

        SDL_UpdateTexture(Texture, nullptr, ColorBuffer.data(), SCREEN_WIDTH * 3);

        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
        SDL_RenderPresent(Renderer);
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);

    SDL_Quit();

    return 0;
}