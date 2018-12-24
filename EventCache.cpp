//
// Created by Cry on 2018-12-20.
//

#include "EventCache.h"

SDL_bool EventQueue::init() {
    head = 0;
    tail = 0;
    size = 0;
    return SDL_TRUE;
}

SDL_bool EventQueue::push_event(SDL_Event event) {
    SDL_bool res;
    if (size >= CONTROL_EVENT_QUEUE_SIZE) {
        return SDL_FALSE;
    }

    if (is_full()) {
        printf("is is_full\n");
        res = SDL_FALSE;
    } else {
        queue[head] = event;
        head = (head + 1) % CONTROL_EVENT_QUEUE_SIZE;
        size++;
    }

//    printf("queue push_event event ,head = %d , size = %ld \n", head, size);

    return res;
}

SDL_bool EventQueue::take_event(SDL_Event *event) {

    if (size == 0) {
        return SDL_FALSE;
    }
    *event = queue[tail];
    tail = (tail + 1) % CONTROL_EVENT_QUEUE_SIZE;
    size--;
//    printf("queue take_event event ,tail = %d ,size = %ld \n", tail, size);

    if (size < 0 || size > CONTROL_EVENT_QUEUE_SIZE) {
        printf("when size is larger than max?? size = %ld ,max = %d \n", size, CONTROL_EVENT_QUEUE_SIZE);

        abort();
    }

    return SDL_TRUE;
}

void EventQueue::destroy() {

}

int EventQueue::is_empty() {
    return head == tail;
}

int EventQueue::is_full() {
    return (head + 1) % CONTROL_EVENT_QUEUE_SIZE == tail;
}

