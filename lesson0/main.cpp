#include <iostream>
#include <SDL2/SDL.h>

/*
 * Lesson 0: Test to make sure SDL is setup properly
 */
int main(int argc, char** argv) {
  if (0 == SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "SDL_Init OK" << std::endl;
  } else {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_Quit();
  return 0;
}

