//
// Created by Cry on 2018-12-20.
//

#include "EventController.h"

void EventController::init() {
    queue = new EventQueue();
    queue->init();

    mutex = SDL_CreateMutex();
    if (!mutex) {
        printf("EventQueue SDL_CreateMutex error\n");
//        return SDL_FALSE;
    }

    cond = SDL_CreateCond();
    if (!cond) {
        printf("EventQueue SDL_CreateCond error\n");
//        return SDL_FALSE;
    }
}

int event_run(void *data) {
    EventController *controller = static_cast<EventController *>(data);
    controller->stop = SDL_FALSE;
    controller->event_handle();
    return 0;
}

void EventController::async_start() {
    event_tid = SDL_CreateThread(event_run, "event_controller", this);
}

void EventController::destroy() {
    stop_loop();
    queue->destroy();

//    SDL_CondSignal(cond);
    SDL_DestroyCond(cond);
    SDL_DestroyMutex(mutex);

}


void EventController::handleButtonEvent(SDL_Screen *screen, SDL_MouseButtonEvent *event) {
    int width = screen->screen_w;
    int height = screen->screen_h;
    int x = event->x;
    int y = event->y;
    //是否超过来边界
    bool outside_device_screen = x < 0 || x >= width ||
                                 y < 0 || y >= height;

    printf("outside_device_screen =%d\n", outside_device_screen);
    if (outside_device_screen) {
        // ignore
        return;
    }
    char buf[6];
    memset(buf, 0, sizeof(buf));
    printf("event x =%d\n", event->x);
    printf("event y =%d\n", event->y);
    printf("event char size =%zu\n", sizeof(char));
    buf[0] = 0;
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        //发送down 事件
        buf[1] = 1;
    } else {
        // 发送UP事件
        buf[1] = 0;
    }
    //高8位
    buf[2] = event->x >> 8;
    //低8位
    buf[3] = event->x & 0xff;
    //高8位
    buf[4] = event->y >> 8;
    //低8位
    buf[5] = event->y & 0xff;

    int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 6);;
    printf("send result = %d\n", result);
}

// Convert window coordinates (as provided by SDL_GetMouseState() to renderer coordinates (as provided in SDL mouse events)
//
// See my question:
// <https://stackoverflow.com/questions/49111054/how-to-get-mouse-position-on-mouse-wheel-event>
void EventController::handleScrollEvent(SDL_Screen *sc, SDL_MouseWheelEvent *event) {
    //处理滑动事件
    int x_c;
    int y_c;
    int *x = &x_c;
    int *y = &y_c;
    SDL_GetMouseState(x, y);
    SDL_Rect viewport;
    float scale_x, scale_y;
    SDL_RenderGetViewport(sc->sdl_renderer, &viewport);
    SDL_RenderGetScale(sc->sdl_renderer, &scale_x, &scale_y);
    *x = (int) (*x / scale_x) - viewport.x;
    *y = (int) (*y / scale_y) - viewport.y;


    int width = sc->screen_w;
    int height = sc->screen_h;

    //是否超过来边界
    bool outside_device_screen = x_c < 0 || x_c >= width ||
                                 y_c < 0 || y_c >= height;

    printf("outside_device_screen =%d\n", outside_device_screen);
    if (outside_device_screen) {
        // ignore
        return;
    }

    SDL_assert_release(x_c >= 0 && x_c < 0x10000 && y_c >= 0 && y_c < 0x10000);

    //使用这个来记录滑动的方向
    // SDL behavior seems inconsistent between horizontal and vertical scrolling
    // so reverse the horizontal
    // <https://wiki.libsdl.org/SDL_MouseWheelEvent#Remarks>
    // SDL 的滑动情况，两个方向不一致
    int mul = event->direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
    int hs = -mul * event->x;
    int vs = mul * event->y;

    char buf[14];
    memset(buf, 0, sizeof(buf));
    printf(" x_c =%d\n", x_c);
    printf(" y_c =%d\n", y_c);
    printf(" hs =%d\n", hs);
    printf(" vs =%d\n", vs);
    buf[0] = 0;
    //滚动事件
    buf[1] = 2;
    //高8位
    buf[2] = x_c >> 8;
    //低8位
    buf[3] = x_c & 0xff;
    //高8位
    buf[4] = y_c >> 8;
    //低8位
    buf[5] = y_c & 0xff;

    //继续滚动距离
    buf[6] = hs >> 24;
    //低8位
    buf[7] = hs >> 16;
    buf[8] = hs >> 8;
    buf[9] = hs;


    //高8位
    buf[10] = vs >> 24;
    //低8位
    buf[11] = vs >> 16;
    buf[12] = vs >> 8;
    buf[13] = vs;

    int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 14);

    printf("send result = %d\n", result);

}

void EventController::handleSDLKeyEvent(SDL_Screen *sc, SDL_KeyboardEvent *event) {
    //分别对应 mac 上的 control option command
    int ctrl = event->keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
    int alt = event->keysym.mod & (KMOD_LALT | KMOD_RALT);
    int meta = event->keysym.mod & (KMOD_LGUI | KMOD_RGUI);
    printf("ctrl = %d,", ctrl);
    printf("meta = %d,", meta);
    printf("alt = %d,\n", alt);

    //期望control+ H = home键 control+b = back键
    //再去取keycode
    SDL_Keycode keycode = event->keysym.sym;
    printf("keycode = %d, action type = %d\n", keycode, event->type);
    printf("b = %d, action type = %d\n", SDLK_b, event->type);
    if (event->type == SDL_KEYDOWN && ctrl != 0) {
        //这个时候发送的是按下的状态
        if (keycode == SDLK_h) {
            char buf[4];
            memset(buf, 0, sizeof(buf));
            buf[0] = 0;
            //自定义的案件事件
            buf[1] = 3;
            //1 是 down
            buf[2] = 1;
            //key code home 键对应的是 3
            buf[3] = 3;
            int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 4);

            printf("send result = %d\n", result);
        } else if (keycode == SDLK_b) {
            char buf[4];
            memset(buf, 0, sizeof(buf));
            buf[0] = 0;
            //自定义的案件事件
            buf[1] = 3;
            //1 是 down
            buf[2] = 1;
            //key code back 键对应的是 4
            buf[3] = 4;
            int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 4);
            printf("send result = %d\n", result);
        }
    }
    if (event->type == SDL_KEYUP && keycode != 0) {
        if (keycode == SDLK_h) {
            char buf[4];
            memset(buf, 0, sizeof(buf));
            buf[0] = 0;
            //自定义的案件事件
            buf[1] = 3;
            //1 是 up
            buf[2] = 0;
            //key code home 键对应的是 3
            buf[3] = 3;
            int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 4);
            printf("send result = %d\n", result);
        } else if (keycode == SDLK_b) {
            char buf[4];
            memset(buf, 0, sizeof(buf));
            buf[0] = 0;
            //自定义的案件事件
            buf[1] = 3;
            //1 是 up
            buf[2] = 0;
            //key code back 键对应的是 4
            buf[3] = 4;
            int result = connection->send_to_(reinterpret_cast<uint8_t *>(buf), 4);
            printf("send result = %d\n", result);
        }
    }

}

void EventController::event_handle() {
    printf("event_handle");
    for (;;) {
        if (stop) {
            break;
        }
        SDL_Event event;
        SDL_LockMutex(mutex);

        if (queue->is_empty()) {
//            printf("is empty\n");
//        return SDL_FALSE;
            SDL_CondWait(cond, mutex);
        }
        SDL_bool sdl_bool = queue->take_event(&event);
        SDL_UnlockMutex(mutex);

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            handleButtonEvent(screen, &event.button);
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            handleButtonEvent(screen, &event.button);
        } else if (event.type == SDL_KEYDOWN) {
            handleSDLKeyEvent(screen, &event.key);
        } else if (event.type == SDL_KEYUP) {
            handleSDLKeyEvent(screen, &event.key);
        } else if (event.type == SDL_MOUSEWHEEL) {
            //处理滑动事件
            handleScrollEvent(screen, &event.wheel);
        }
    }

}

void EventController::stop_loop() {
    stop = SDL_TRUE;
    SDL_DetachThread(event_tid);
}

EventController::EventController(SDL_Screen *screen, SocketConnection *connection) : screen(screen),
                                                                                     connection(connection) {}

void EventController::push_event(SDL_Event event) {
    SDL_LockMutex(mutex);
    //先判断当前是否为空。因为如果为空的话，会锁住。
    int empty = queue->is_empty();
    queue->push_event(event);
    //如果是空的话，就需要通知他继续取
    if (empty) {
//        printf("signal");
        SDL_CondSignal(cond);
    }
    SDL_UnlockMutex(mutex);
}

