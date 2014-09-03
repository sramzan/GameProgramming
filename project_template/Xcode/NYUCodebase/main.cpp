
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL_image.h>

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);

    bool done = false;
    SDL_Event event;
    
    while(!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
    }
    
    SDL_Quit();
    return 0;
}