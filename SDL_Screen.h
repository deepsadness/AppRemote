//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_SDL_SCREEN_H
#define ASREMOTE_SDL_SCREEN_H

#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <SDL2/SDL.h>
#define EVENT_NEW_FRAME (SDL_USEREVENT + 1)

class SDL_Screen {
public:
    char *name;
    SDL_Window *sdl_window;
    SDL_Texture *sdl_texture;

    int screen_w;
    int screen_h;
    SDL_Renderer *sdl_renderer;

    SDL_Screen(char *name, int screen_w, int screen_h);

    ~SDL_Screen();

    /**
     * 进行初始化
     * @return
     */
    SDL_bool init();

    void destroy();

    void uploadTexture(const Uint8 *Yplane, int Ypitch,
                        const Uint8 *Uplane, int Upitch,
                        const Uint8 *Vplane, int Vpitch);

    void push_frame_event();


};




#endif //ASREMOTE_SDL_SCREEN_H
