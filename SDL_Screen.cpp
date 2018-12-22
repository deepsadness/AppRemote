//
// Created by Cry on 2018-12-20.
//

#include <sys/param.h>
#include "SDL_Screen.h"

#define DISPLAY_MARGINS 96

SDL_Screen::SDL_Screen(char *name, int screen_w, int screen_h) : name(name), screen_w(screen_w), screen_h(screen_h) {
    printf("s screen_w = %d,screen_h = %d\n", screen_w, screen_h);
}

SDL_Screen::~SDL_Screen() {
}

struct size {
    int width;
    int height;
};


// get the preferred display bounds (i.e. the screen bounds with some margins)
static SDL_bool get_preferred_display_bounds(struct size *bounds) {
    SDL_Rect rect;
#if SDL_VERSION_ATLEAST(2, 0, 5)
# define GET_DISPLAY_BOUNDS(i, r) SDL_GetDisplayUsableBounds((i), (r))
#else
# define GET_DISPLAY_BOUNDS(i, r) SDL_GetDisplayBounds((i), (r))
#endif
    if (GET_DISPLAY_BOUNDS(0, &rect)) {
//        LOGW("Could not get display usable bounds: %s", SDL_GetError());
        printf("Could not get display usable bounds: %s\n", SDL_GetError());
        return SDL_FALSE;
    }

    bounds->width = MAX(0, rect.w - DISPLAY_MARGINS);
    bounds->height = MAX(0, rect.h - DISPLAY_MARGINS);
    return SDL_TRUE;
}

// return the optimal size of the window, with the following constraints:
//  - it attempts to keep at least one dimension of the current_size (i.e. it crops the black borders)
//  - it keeps the aspect ratio
//  - it scales down to make it fit in the display_size
static struct size get_optimal_size(struct size current_size, struct size frame_size) {
    if (frame_size.width == 0 || frame_size.height == 0) {
        // avoid division by 0
        return current_size;
    }

    struct size display_size;
    // 32 bits because we need to multiply two 16 bits values
    int w;
    int h;

    if (!get_preferred_display_bounds(&display_size)) {
        // cannot get display bounds, do not constraint the size
        w = current_size.width;
        h = current_size.height;
    } else {
        w = MIN(current_size.width, display_size.width);
        h = MIN(current_size.height, display_size.height);
    }

    SDL_bool keep_width = static_cast<SDL_bool>(frame_size.width * h > frame_size.height * w);
    if (keep_width) {
        // remove black borders on top and bottom
        h = frame_size.height * w / frame_size.width;
    } else {
        // remove black borders on left and right (or none at all if it already fits)
        w = frame_size.width * h / frame_size.height;
    }

    // w and h must fit into 16 bits
    SDL_assert_release(w < 0x10000 && h < 0x10000);
    return (struct size) {w, h};
}

SDL_bool SDL_Screen::init() {
    printf("SDL_Screen init\n");

    //对SDL进行初始化。需要初始化 sdl_window render sdlrect
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return SDL_FALSE;
    }
    struct size frame_size = {
            .height=screen_h,
            .width=screen_w
    };
    struct size window_size = get_optimal_size(frame_size, frame_size);

    printf("SDL:window_size ,w = %d ,h =%d \n", window_size.width, window_size.height);

    //创建window
    sdl_window = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            window_size.width, window_size.height,
            SDL_WINDOW_RESIZABLE);

    if (!sdl_window) {
        printf("SDL:could not create sdl_window ,error :%s\n", SDL_GetError());
        return SDL_FALSE;
    }

    //创建Render
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);

    if (SDL_RenderSetLogicalSize(sdl_renderer, frame_size.width, frame_size.height)) {
        printf("Could not set renderer logical size: %s\n", SDL_GetError());
        return SDL_FALSE;
    }

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
