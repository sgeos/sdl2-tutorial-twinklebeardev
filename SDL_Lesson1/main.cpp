#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>

#include "Constants.h"
#include "Utility.h"

int main(int argc, char** argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "Error: SDL_Init " << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  SDL_Window *window = SDL_CreateWindow(
    Constants::WindowTitle(),
    Constants::WindowPositionX(),
    Constants::WindowPositionY(),
    Constants::WindowWidth(),
    Constants::WindowHeight(),
    SDL_WINDOW_SHOWN
  );
  if (nullptr == window) {
    std::cout << "Error: SDL_CreateWindow " << SDL_GetError() << std::endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(
    window,
    Constants::DefaultRendererWindow(),
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );
  if (nullptr == renderer) {
    std::cout << "Error: SDL_CreateRenderer " << SDL_GetError() << std::endl;
    Utility::cleanup(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }
  const std::string imagePath = Constants::ResourcePath(Constants::ApplicationName()) + "hello.bmp";
  SDL_Surface *bitmap = SDL_LoadBMP(imagePath.c_str());
  if (nullptr == bitmap) {
    std::cout << "Error: SDL_LoadBMP " << SDL_GetError() << std::endl;
    Utility::cleanup(renderer, window);
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bitmap);
  Utility::cleanup(bitmap);
  if (nullptr == texture) {
    std::cout << "Error: SDL_CreateTextureFromSurface " << SDL_GetError() << std::endl;
    Utility::cleanup(renderer, window);
    SDL_Quit();
    return EXIT_FAILURE;
  }
  bool done = false;
  int frame = 0;
  do {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
      case SDL_QUIT:
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
        done = true;
        break;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    if (0 == frame % Constants::FramesPerSecond()) {
      std::cout << "Frame: " << frame << std::endl;
    }
    frame++;
    SDL_Delay(Constants::FrameWait());
  } while (!done);
  Utility::cleanup(texture, renderer, window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

