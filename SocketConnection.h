//
// Created by Cry on 2018-12-20.
//

#ifndef ASREMOTE_SOCKETCONNECTION_H
#define ASREMOTE_SOCKETCONNECTION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <cstdio>

/**
 * 这里的方法均阻塞
 */
class SocketConnection {
public:
    int client_conn;

    /**
     * 连接Socket
     */
    SDL_bool connect_server();

    /**
     * 关闭Socket
     */
    void close_client();

    /**
     * Socket 发送
     */
    int send_to_(uint8_t *buf, int len);

    /**
     * Socket 接受
     */
    int recv_from_(uint8_t *buf, int len);
};


#endif //ASREMOTE_SOCKETCONNECTION_H
