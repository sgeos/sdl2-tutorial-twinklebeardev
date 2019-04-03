#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Constants.h"
#include "Utility.h"

void logSdlError(std::ostream &pOutputStream, const std::string pMessage) {
  pOutputStream << pMessage << " Error: " << SDL_GetError() << std::endl;
}

SDL_Texture *loadTexture(const std::string &pFileName, SDL_Renderer *pRenderer) {
  SDL_Texture *texture = IMG_LoadTexture(pRenderer, pFileName.c_str());
  if (nullptr == texture) {
    logSdlError(std::cout, "LoadTexture");
  }
  return texture;
}

void renderTexture(SDL_Texture *pTexture, SDL_Renderer *pRenderer, int pPositionX, int pPositionY, int pWidth, int pHeight) {
  SDL_Rect destination;
  destination.x = pPositionX;
  destination.y = pPositionY;
  destination.w = pWidth;
  destination.h = pHeight;
  SDL_RenderCopy(pRenderer, pTexture, nullptr, &destination);
}

void renderTexture(SDL_Texture *pTexture, SDL_Renderer *pRenderer, int pPositionX, int pPositionY) {
  int width, height;
  SDL_QueryTexture(pTexture, nullptr, nullptr, &width, &height);
  renderTexture(pTexture, pRenderer, pPositionX, pPositionY, width, height);
}

int main(int argc, char** argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "Error: SDL_Init " << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if (IMG_INIT_PNG != (IMG_INIT_PNG & IMG_Init(IMG_INIT_PNG))) {
    logSdlError(std::cout, "IMG_Init");
    SDL_Quit();
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
    logSdlError(std::cout, "SDL_CreateWindow");
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(
    window,
    Constants::DefaultRendererWindow(),
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );
  if (nullptr == renderer) {
    logSdlError(std::cout, "SDL_CreateRenderer");
    Utility::cleanup(window);
    IMG_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }
  const std::string resourcePath = Constants::ResourcePath(Constants::ApplicationName());
  SDL_Texture *background = loadTexture(resourcePath + "background.png", renderer);
  SDL_Texture *image = loadTexture(resourcePath + "image.png", renderer);
  if (nullptr == background || nullptr == image) {
    Utility::cleanup(background, image, renderer, window);
    IMG_Quit();
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
    int tileWidth = Constants::TileSize();
    int tileHeight = Constants::TileSize();
    int offsetX = (frame / 2) % tileWidth - tileWidth;
    int offsetY = (-frame / 3) % tileHeight - tileHeight;
    for (int y = offsetY; y < Constants::WindowHeight(); y += tileHeight) {
      for (int x = offsetX; x < Constants::WindowWidth(); x += tileWidth) {
        renderTexture(background, renderer, x, y, tileWidth, tileHeight);
      }
    }
    int imageWidth, imageHeight;
    SDL_QueryTexture(image, nullptr, nullptr, &imageWidth, &imageHeight);
    imageWidth *= 1.0 + 0.5 * cos((float)frame / (Constants::FramesPerSecond() / 2));
    imageHeight *= 1.0 + 0.5 * sin((float)frame / (Constants::FramesPerSecond() / 2));
    int centerX = (Constants::WindowWidth() - imageWidth) / 2;
    int centerY = (Constants::WindowHeight() - imageHeight) / 2;
    int x = centerX * (1.0 + 0.5 * cos((float)frame / (2 * Constants::FramesPerSecond())));
    int y = centerY * (1.0 + 0.5 * sin((float)frame / Constants::FramesPerSecond()));
    renderTexture(image, renderer, x, y, imageWidth, imageHeight);
    SDL_RenderPresent(renderer);
    if (0 == frame % Constants::FramesPerSecond()) {
      std::cout << "Frame: " << frame << std::endl;
    }
    frame++;
    SDL_Delay(Constants::FrameWait());
  } while (!done);
  Utility::cleanup(background, image, renderer, window);
  IMG_Quit();
  SDL_Quit();
  return EXIT_SUCCESS;
}

