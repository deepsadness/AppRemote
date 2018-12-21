//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_EVENTCACHE_H
#define ASREMOTE_EVENTCACHE_H

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_mutex.h"

#define CONTROL_EVENT_QUEUE_SIZE 64


class EventQueue {
    SDL_Event queue[CONTROL_EVENT_QUEUE_SIZE];
    int head;
    int tail;
    long size=0;

public:

    SDL_bool init();

    void destroy();

    SDL_bool push_event(SDL_Event event);

    int is_empty();

    int is_full();

    SDL_bool take_event(SDL_Event *event);

};


#endif //ASREMOTE_EVENTCACHE_H
