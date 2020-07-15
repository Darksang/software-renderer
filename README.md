# Software Renderer

# Compiling

Create a *build* folder and run CMake from there. Build after the process finishes. If compiling on Mac OS X, make sure to have SDL 2.0 installed, CMake should find it automatically.

# Executing

**Windows**. After the executable is generated, copy *SDL2.dll* from *lib/sdl2/* (32 or 64 bits) to where the executable is.

# Libraries Used

- [SDL2](https://www.libsdl.org/)
- [OpenGL Mathematics 0.9.9.3](https://glm.g-truc.net/0.9.9/index.html)
- [stbi Image Loader](https://github.com/nothings/stb)
- [tinyobjloader](https://github.com/syoyo/tinyobjloader)