//
// Created by Cry on 2018-12-20.
//

#include "SDL_Screen.h"

SDL_Screen::SDL_Screen(char *name, int screen_w, int screen_h) : name(name), screen_w(screen_w), screen_h(screen_h) {}

SDL_Screen::~SDL_Screen() {
    if (name) {
        delete name;
    }
}

SDL_bool SDL_Screen::init() {
    printf("SDL_Screen init\n");

    //对SDL进行初始化。需要初始化 sdl_window render sdlrect
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return SDL_FALSE;
    }

    //创建window
    sdl_window = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            screen_w, screen_h,
            SDL_WINDOW_RESIZABLE);

    if (!sdl_window) {
        printf("SDL:could not create sdl_window ,error :%s\n", SDL_GetError());
        return SDL_FALSE;
    }

    //创建Render
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
    //创建texture
    sdl_texture = SDL_CreateTexture(
            sdl_renderer,
            SDL_PIXELFORMAT_IYUV,
            SDL_TEXTUREACCESS_STREAMING,
            screen_w, screen_h);

    return SDL_TRUE;
}

void SDL_Screen::destroy() {
    printf("SDL_Screen destroy\n");

    if (sdl_texture) {
        SDL_DestroyTexture(sdl_texture);
    }
    if (sdl_renderer) {
        SDL_DestroyRenderer(sdl_renderer);
    }
    if (sdl_window) {
        SDL_DestroyWindow(sdl_window);
    }

    SDL_Quit();
}

void SDL_Screen::uploadTexture(const Uint8 *Yplane, int Ypitch, const Uint8 *Uplane, int Upitch, const Uint8 *Vplane,
                               int Vpitch) {
    SDL_UpdateYUVTexture(sdl_texture, NULL,
                         Yplane, Ypitch,
                         Uplane, Upitch,
                         Vplane, Vpitch);

    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);

    SDL_Delay(40);
}

void SDL_Screen::push_frame_event() {
    static SDL_Event new_frame_event = {
            .type=EVENT_NEW_FRAME
    };
    SDL_PushEvent(&new_frame_event);

}
