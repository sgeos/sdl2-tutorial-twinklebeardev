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

TTF_Font *openFont(const std::string &pFontFileName, int pFontSize) {
  TTF_Font *font = TTF_OpenFont(pFontFileName.c_str(), pFontSize);
  if (nullptr == font) {
    logSdlError(std::cout, "TTF_OpenFont");
    return nullptr;
  }
  return font;
}

SDL_Texture *renderText(
  const std::string &pMessage,
  TTF_Font *pFont,
  SDL_Color pColor,
  SDL_Renderer *pRenderer
) {
  SDL_Surface *surface = TTF_RenderText_Blended(pFont, pMessage.c_str(), pColor);
  if (nullptr == surface) {
    logSdlError(std::cout, "TTF_RenderText");
    return nullptr;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(pRenderer, surface);
  if (nullptr == texture) {
    logSdlError(std::cout, "CreateTexture");
  }
  SDL_FreeSurface(surface);
  return texture;
}

int main(int argc, char** argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "Error: SDL_Init " << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  if (0 != TTF_Init()) {
    logSdlError(std::cout, "TTF_Init");
    SDL_Quit();
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
  TTF_Font *font = openFont(resourcePath + "twinklebear_ascii.ttf", 64);
  SDL_Color image_color = {0xFF, 0xFF, 0xFF, 0xFF};
  SDL_Texture *image = renderText("True type font test!", font, image_color, renderer);
  SDL_Color background_color = {0x00, 0x00, 0x66, 0xFF};
  SDL_Texture *background = renderText("Background  ...  ", font, background_color, renderer);
  TTF_CloseFont(font);
  if (nullptr == image || nullptr == background) {
    Utility::cleanup(image, background, renderer, window);
    IMG_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }
  bool done = false;
  int frame = 0;
  do {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_KEYDOWN:
          done = true;
          break;
        default:
          break;
      }
    }
    SDL_RenderClear(renderer);
    int tileWidth, tileHeight;
    SDL_QueryTexture(image, nullptr, nullptr, &tileWidth, &tileHeight);
    tileWidth /= 2;
    tileHeight /= 2;
    int offsetX = cos((float)frame / (Constants::FramesPerSecond() * 3)) * tileHeight - tileHeight;
    int offsetY = (frame / 2) % tileWidth - tileWidth;
    for (int y = offsetY; y < Constants::WindowHeight(); y += tileHeight) {
      for (int x = offsetX; x < Constants::WindowWidth(); x += tileWidth) {
        renderTexture(background, renderer, x, y, tileWidth, tileHeight);
      }
    }
    int imageWidth, imageHeight;
    SDL_QueryTexture(image, nullptr, nullptr, &imageWidth, &imageHeight);
    imageWidth *= 1.0 + 0.7 * cos((float)frame / (Constants::FramesPerSecond() / 2));
    imageHeight *= 1.0 + 0.7 * sin((float)frame / (Constants::FramesPerSecond() / 2));
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
  Utility::cleanup(image, renderer, window);
  IMG_Quit();
  SDL_Quit();
  return EXIT_SUCCESS;
}

