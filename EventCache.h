//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_EVENTCACHE_H
#define ASREMOTE_EVENTCACHE_H

#include "SDL2/SDL_events.h"

class EventCache {
    SDL_Event event;
    EventCache *next;
    int size;

public:
    void push_event(SDL_Event event);
    SDL_Event pop_event();
};


#endif //ASREMOTE_EVENTCACHE_H
