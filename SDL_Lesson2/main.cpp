#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

#include "Constants.h"
#include "Utility.h"

void logSdlError(std::ostream &pOutputStream, const std::string pMessage) {
  pOutputStream << pMessage << " Error: " << SDL_GetError() << std::endl;
}

SDL_Texture *loadTexture(const std::string &pFileName, SDL_Renderer *pRenderer) {
  SDL_Texture *texture = nullptr;
  SDL_Surface *loadedImage = SDL_LoadBMP(pFileName.c_str());
  if (nullptr == loadedImage) {
    logSdlError(std::cout, "LoadBMP");
  } else {
    texture = SDL_CreateTextureFromSurface(pRenderer, loadedImage);
    SDL_FreeSurface(loadedImage);
    if (nullptr == texture) {
      logSdlError(std::cout, "CreateTextureFromSurface");
    }
  }
  return texture;
}

void renderTexture(SDL_Texture *pTexture, SDL_Renderer *pRenderer, int pPositionX, int pPositionY) {
  SDL_Rect destination;
  destination.x = pPositionX;
  destination.y = pPositionY;
  SDL_QueryTexture(pTexture, nullptr, nullptr, &destination.w, &destination.h);
  SDL_RenderCopy(pRenderer, pTexture, nullptr, &destination);
}

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
    SDL_Quit();
    return EXIT_FAILURE;
  }
  const std::string resourcePath = Constants::ResourcePath(Constants::ApplicationName());
  SDL_Texture *background = loadTexture(resourcePath + "background.bmp", renderer);
  SDL_Texture *image = loadTexture(resourcePath + "image.bmp", renderer);
  if (nullptr == background || nullptr == image) {
    Utility::cleanup(background, image, renderer, window);
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
    int backgroundWidth, backgroundHeight;
    SDL_QueryTexture(background, nullptr, nullptr, &backgroundWidth, &backgroundHeight);
    for (int y = 0; y < Constants::WindowHeight(); y += backgroundHeight) {
      for (int x = 0; x < Constants::WindowWidth(); x += backgroundWidth) {
        renderTexture(background, renderer, x, y);
      }
    }
    int imageWidth, imageHeight;
    SDL_QueryTexture(image, nullptr, nullptr, &imageWidth, &imageHeight);
    int centerX = (Constants::WindowWidth() - imageWidth) / 2;
    int centerY = (Constants::WindowHeight() - imageHeight) / 2;
    int x = centerX * (1.0 + 0.5 * cos((float)frame / (2 * Constants::FramesPerSecond())));
    int y = centerY * (1.0 + 0.5 * sin((float)frame / Constants::FramesPerSecond()));
    renderTexture(image, renderer, x, y);
    SDL_RenderPresent(renderer);
    if (0 == frame % Constants::FramesPerSecond()) {
      std::cout << "Frame: " << frame << std::endl;
    }
    frame++;
    SDL_Delay(Constants::FrameWait());
  } while (!done);
  Utility::cleanup(background, image, renderer, window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

