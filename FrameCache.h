//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_FRAMECACHE_H
#define ASREMOTE_FRAMECACHE_H

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_mutex.h>

extern "C" {
#include "libavformat/avformat.h"
};

class FrameCache {
private:
    SDL_mutex *mutex;

public:
    AVFrame *decode_frame;
    AVFrame *render_frame;
    SDL_bool consume_render;

    SDL_bool init();

    SDL_bool product_frame();

    void frames_swap();
};


#endif //ASREMOTE_FRAMECACHE_H
