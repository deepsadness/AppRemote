#include <iostream>
#include <SDL2/SDL_stdinc.h>
#include "SocketConnection.h"
#include "SDL_Screen.h"
#include "FrameCache.h"
#include "FFmpegDecoder.h"
#include "EventCache.h"
#include "EventController.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    //先进行Socket连接
    SocketConnection *socketConnection = new SocketConnection();
    if (socketConnection->connect_server() == SDL_FALSE) {
        return -1;
    }
    //连接成功。
    printf("连接成功\n");
    //从客户端接受屏幕数据
    uint8_t size[4];
    socketConnection->recv_from_(reinterpret_cast<uint8_t *>(size), 4);

    //这里先写死，后面从客户端内接受
    int width = (size[0] << 8) | (size[1]);
    int height = (size[2] << 8) | (size[3]);

    printf("width = %d , height = %d \n", width, height);


    int scale =2;
    char *name = "as_remote";
    SDL_Screen *screen = new SDL_Screen(name, static_cast<int>(width / scale), static_cast<int>(height / scale));
    screen->init();
    //开启编码器
    FrameCache *cache = new FrameCache();
    cache->init();

    FFmpegDecoder *decoder = new FFmpegDecoder(socketConnection, cache, screen);
//    decoder->init();
    decoder->async_start();

    EventController *controller = new EventController(screen, socketConnection);
    controller->init();
    controller->async_start();

    //开启事件循环
    printf("开启事件循环\n");

    SDL_Event event;
    //开启Event Loop
    for (;;) {
        SDL_WaitEvent(&event);
//        printf("接到事件\n");

        if (event.type == EVENT_NEW_FRAME) { //渲染
            AVFrame *render = cache->render_frame;
            screen->uploadTexture(
                    render->data[0], render->linesize[0],
                    render->data[1], render->linesize[1],
                    render->data[2], render->linesize[2]
            );
            cache->consume_render = SDL_TRUE;
        } else if (event.type == SDL_QUIT) {
            printf("rev event type=SDL_QUIT\n");
            break;
        } else {
            controller->push_event(event);
        }
    }

    controller->destroy();
    decoder->stop();
    decoder->destroy();
    screen->destroy();
    delete screen;
    delete controller;
    delete cache;
    delete decoder;
    close_conn:
    socketConnection->close_client();
    delete socketConnection;
    return 0;
}

