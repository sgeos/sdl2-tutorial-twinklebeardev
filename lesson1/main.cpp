#include <iostream>
#include <SDL2/SDL.h>

#include "Constants.h"

/*
 * Lesson 0: Test to make sure SDL is setup properly
 */
int main(int argc, char** argv) {
  std::string imagePath = Constants::ResourcePath("Lesson1") + "hello.bmp";
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Surface *bitmap = nullptr;
  SDL_Texture *texture = nullptr;
  bool error = false;
  bool done = false;
  int frame = 0;

  error = SDL_Init(SDL_INIT_VIDEO);
  if (error) {
    std::cout << "SDL_Init ";
    goto error_init;
  }
  window = SDL_CreateWindow(
    Constants::WindowTitle(),
    Constants::WindowX(),
    Constants::WindowY(),
    Constants::WindowW(),
    Constants::WindowH(),
    SDL_WINDOW_SHOWN
  );
  error = nullptr == window;
  if (error) {
    std::cout << "SDL_CreateWindow ";
    goto error_window;
  }
  renderer = SDL_CreateRenderer(
    window,
    Constants::DefaultRendererWindow(),
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );
  error = nullptr == renderer;
  if (error) {
    std::cout << "SDL_CreateRenderer ";
    goto error_renderer;
  }
  bitmap = SDL_LoadBMP(imagePath.c_str());
  error = nullptr == bitmap;
  if (error) {
    std::cout << "SDL_LoadBMP ";
    goto error_texture;
  }
  texture = SDL_CreateTextureFromSurface(renderer, bitmap);
  SDL_FreeSurface(bitmap);
  error = nullptr == texture;
  if (error) {
    std::cout << "SDL_CreateTextureFromSurface ";
    goto error_texture;
  }
  do {
    SDL_Event event;
    SDL_PollEvent(&event);
    done = done || SDL_QUIT == event.type;
    done = done || SDL_KEYDOWN == event.type;
    done = done || SDL_MOUSEBUTTONDOWN == event.type;
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    if (0 == frame % Constants::FramesPerSecond()) {
      std::cout << "Frame: " << frame << std::endl;
    }
    frame++;
    SDL_Delay(Constants::FrameWait());
  } while (!done);
  SDL_DestroyTexture(texture);
  error_texture:
    SDL_DestroyRenderer(renderer);
  error_renderer:
    SDL_DestroyWindow(window);
  error_window:
    SDL_Quit();
  error_init:
    if (error) {
      std::cout << "Error: " << SDL_GetError() << std::endl;
    }
  return error;
}

