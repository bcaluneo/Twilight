// Copyright (C) Brendan Caluneo
// Recreation of the "twilight" wallpaper program from SGI's Irix.
// Original source written by Howard Look.

#include "SDL.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <tuple>

bool quit = 0;
SDL_Window *window;
SDL_Renderer *render;

static const float TRANSITION = 0.2;
static const size_t NUM_SMALL_STARS = 2500;
static const size_t NUM_BIG_STARS = 200;

static const SDL_Color orange {255, 72, 0, 255};
static const SDL_Color blue   {0, 110, 189, 255};
static const SDL_Color black  {0, 0, 0, 255};

auto lerp(float a, float b, float t) {
  return (1 - t) * a + t * b;
}

// Computes the color of a star given its y position on the screen.
// Minor changes were made to this function from the original source code.
auto getStarColor(float y, float height) {
  int r, g, b;
  float a;
  float ratio;
  float cutoff = height * TRANSITION;

  if (y > height / 2) return std::make_tuple(255, 255, 255);

  if (y < cutoff) {
    ratio = y / cutoff;
    r = orange.r * (1.0 - ratio) + blue.r * ratio;
    g = orange.g * (1.0 - ratio) + blue.g * ratio;
    b = orange.b * (1.0 - ratio) + blue.b * ratio;
  } else {
    ratio = (y - cutoff) / (height - cutoff);
    r = blue.r * (1.0 - ratio) + black.r * ratio;
    g = blue.g * (1.0 - ratio) + black.g * ratio;
    b = blue.b * (1.0 - ratio) + black.b * ratio;
  }

	a = (y / (height / 2.0));

	r = r * (1.0 - a) + 255 * a;
	g = g * (1.0 - a) + 255 * a;
	b = b * (1.0 - a) + 255 * a;

  return std::make_tuple(r, g, b);
}

// Draws the wallpaper.
// Small stars are drawn as 1-pixel sized dots and large stars are drawn as
// diamonds if they are at right scale (randomly determined).
void draw(int w, int h, bool &redraw) {
  auto gradient = [](SDL_Color bottom, SDL_Color top, auto val) {
    auto r = static_cast<int>(lerp(bottom.r, top.r, val));
    auto g = static_cast<int>(lerp(bottom.g, top.g, val));
    auto b = static_cast<int>(lerp(bottom.b, top.b, val));
    SDL_SetRenderDrawColor(render, r, g, b, 255);
  };

  auto star = [&h](auto y) {
    const auto [r, g, b] = getStarColor(y, h);
    SDL_SetRenderDrawColor(render, r, g, b, 255);
  };

  if (redraw) {
    auto transHeight = h*TRANSITION;

    for (auto y = 0; y < transHeight; y++) {
      gradient(orange, blue, y/transHeight);
      SDL_RenderDrawLine(render, 0, h-y, w, h-y);
    }

    transHeight = h*(1-TRANSITION);

    for (auto y = 0; y < transHeight; y++) {
      gradient(blue, black, y/transHeight);
      SDL_RenderDrawLine(render, 0, transHeight-y-1, w, transHeight-y-1);
    }

    for (size_t i = 0; i < NUM_SMALL_STARS; i++) {
      auto pX = std::rand() % w;
      auto pY = std::rand() % h;
      star(pY);
      SDL_RenderDrawPoint(render, pX, h - pY);
    }

    for (size_t i = 0; i < NUM_BIG_STARS; i++) {
      auto pX = std::rand() % w;
      auto pY = std::rand() % h;
      auto scale = (std::rand() % 4) + 1;
      SDL_Rect vert {pX + 1, h - pY, 2, scale};
      SDL_Rect horz {pX, h - pY + 1, scale, 2};
      star(pY);
      SDL_RenderFillRect(render, &vert);
      SDL_RenderFillRect(render, &horz);
    }

    redraw = 0;
    SDL_RenderPresent(render);
  }
}

int main(int argc, char **args) {
  SDL_Init(SDL_INIT_VIDEO);
  std::srand(std::time(0));

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  auto w = dm.w;
  auto h = dm.h;

	window = SDL_CreateWindow("Twilight",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            w,
                            h,
                            SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED);
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(render, blue.r, blue.g, blue.b, 255);
  SDL_RenderClear(render);

  SDL_Event event;
  bool redraw = 1;
  while (!quit) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = 1;
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            quit = 1;
          }
          break;
        case SDL_KEYUP:
          if (event.key.keysym.sym == SDLK_r) {
           SDL_SetRenderDrawColor(render, blue.r, blue.g, blue.b, 255);
           SDL_RenderClear(render);
           redraw = 1;
          }

          break;
      }
    }

    draw(w, h, redraw);

    SDL_RenderPresent(render);
    SDL_Delay(1000 / 60);
  }

	SDL_Quit();

  return 0;
}
