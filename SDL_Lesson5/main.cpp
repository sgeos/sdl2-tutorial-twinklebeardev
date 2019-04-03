#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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

void renderTexture(SDL_Texture *pTexture, SDL_Renderer *pRenderer, SDL_Rect pDestination, SDL_Rect *pClip = nullptr) {
  SDL_RenderCopy(pRenderer, pTexture, pClip, &pDestination);
}

void renderTexture(
  SDL_Texture *pTexture,
  SDL_Renderer *pRenderer,
  int pPositionX,
  int pPositionY,
  int pWidth,
  int pHeight,
  SDL_Rect *pClip = nullptr
) {
  SDL_Rect destination;
  destination.x = pPositionX;
  destination.y = pPositionY;
  destination.w = pWidth;
  destination.h = pHeight;
  renderTexture(pTexture, pRenderer, destination, pClip);
}

void renderTexture(SDL_Texture *pTexture, SDL_Renderer *pRenderer, int pPositionX, int pPositionY, SDL_Rect *pClip = nullptr) {
  SDL_Rect destination;
  destination.x = pPositionX;
  destination.y = pPositionY;
  if (nullptr == pClip) {
    SDL_QueryTexture(pTexture, nullptr, nullptr, &destination.w, &destination.h);
  } else {
    destination.w = pClip->w;
    destination.h = pClip->h;
  }
  renderTexture(pTexture, pRenderer, destination, pClip);
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
  SDL_Texture *image = loadTexture(resourcePath + "image.png", renderer);
  if (nullptr == image) {
    Utility::cleanup(image, renderer, window);
    IMG_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  bool done = false;
  int frame = 0;
  SDL_Rect clips[4];
  for (int i = 0; i < 4; i++) {
    clips[i].x = i / 2 * Constants::ClipSize();
    clips[i].y = i % 2 * Constants::ClipSize();
    clips[i].w = Constants::ClipSize();
    clips[i].h = Constants::ClipSize();
  }
  bool clipOverride = false;
  int clipIndex = 0;
  do {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
        case SDL_MOUSEBUTTONDOWN:
          done = true;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_1:
              clipOverride = true;
              clipIndex = 0;
              break;
            case SDLK_2:
              clipOverride = true;
              clipIndex = 1;
              break;
            case SDLK_3:
              clipOverride = true;
              clipIndex = 2;
              break;
            case SDLK_4:
              clipOverride = true;
              clipIndex = 3;
              break;
            case SDLK_ESCAPE:
              done = true;
              break;
            default:
              clipOverride = false;
              break;
          }
        default:
          break;
      }
    }
    clipIndex = clipOverride ? clipIndex : (frame / Constants::FramesPerSecond()) % 4;
    SDL_RenderClear(renderer);
    int tileWidth = Constants::TileSize();
    int tileHeight = Constants::TileSize();
    int offsetX = (frame / -3) % tileWidth - tileWidth;
    int offsetY = sin((float)frame / (Constants::FramesPerSecond() * 3)) * tileHeight - tileHeight;
    for (int y = offsetY; y < Constants::WindowHeight(); y += tileHeight) {
      for (int x = offsetX; x < Constants::WindowWidth(); x += tileWidth) {
        renderTexture(image, renderer, x, y, tileWidth, tileHeight);
      }
    }
    int imageWidth = Constants::ClipSize();
    int imageHeight = Constants::ClipSize();
    SDL_QueryTexture(image, nullptr, nullptr, &imageWidth, &imageHeight);
    imageWidth *= 1.0 + 0.9 * cos((float)frame / (Constants::FramesPerSecond() / 2));
    imageHeight *= 1.0 + 0.9 * sin((float)frame / (Constants::FramesPerSecond() / 2));
    int centerX = (Constants::WindowWidth() - imageWidth) / 2;
    int centerY = (Constants::WindowHeight() - imageHeight) / 2;
    int x = centerX * (1.0 + 0.5 * cos((float)frame / (2 * Constants::FramesPerSecond())));
    int y = centerY * (1.0 + 0.5 * sin((float)frame / Constants::FramesPerSecond()));
    renderTexture(image, renderer, x, y, imageWidth, imageHeight, &clips[clipIndex]);
    SDL_RenderPresent(renderer);
    if (0 == frame % Constants::FramesPerSecond()) {
      std::cout << "Frame: " << frame << std::endl;
    }
    frame++;
    SDL_Delay(Constants::FrameWait());
  } while (!done);
  Utility::cleanup(image, renderer, window);
  IMG_Quit();
  SDL_Quit();
  return EXIT_SUCCESS;
}

