//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_FFMPEGDECODER_H
#define ASREMOTE_FFMPEGDECODER_H


#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_thread.h>
#include "SocketConnection.h"
#include "FrameCache.h"
#include "SDL2/SDL_events.h"
#include "SDL_Screen.h"

#define BUF_SIZE 0x10000

class FFmpegDecoder {
public:
    SocketConnection *connection;
    FrameCache *cache;
    SDL_Thread *decoder_tid;
    SDL_Screen *screen;

    AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    AVIOContext *avio_ctx;

    AVPacket *packet;

    SDL_bool request_stop;

    FFmpegDecoder(SocketConnection *connection);

    FFmpegDecoder(SocketConnection *connection, FrameCache *cache, SDL_Screen *screen);

    SDL_bool init();

    SDL_bool async_start();

    void _decode_loop();

    void stop();

    void destroy();
};


#endif //ASREMOTE_FFMPEGDECODER_H
