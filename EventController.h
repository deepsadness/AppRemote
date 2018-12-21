//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_EVENTCONTROLLER_H
#define ASREMOTE_EVENTCONTROLLER_H


#include "EventCache.h"
#include "SDL2/SDL_thread.h"
#include "SDL_Screen.h"
#include "SocketConnection.h"

class EventController {
public:
    EventQueue *queue;
    SDL_Thread *event_tid;
    SDL_Screen *screen;
    SocketConnection *connection;
    SDL_bool stop;

    SDL_mutex *mutex;
    SDL_cond *cond;

    EventController(SDL_Screen *screen, SocketConnection *connection);

    void init();

    void async_start();

    void destroy();

    void event_handle();

    void stop_loop();

    void push_event(SDL_Event event);

    void handleButtonEvent(SDL_Screen *screen, SDL_MouseButtonEvent *event);

    void handleSDLKeyEvent(SDL_Screen *sc, SDL_KeyboardEvent *event);

    void handleScrollEvent(SDL_Screen *sc, SDL_MouseWheelEvent *event);

};


#endif //ASREMOTE_EVENTCONTROLLER_H
