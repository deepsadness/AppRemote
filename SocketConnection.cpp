//
// Created by Cry on 2018-12-20.
//

#include <SDL2/SDL_stdinc.h>
#include "SocketConnection.h"

SDL_bool SocketConnection::connect_server() {
    //创建Socket
    client_conn = socket(PF_INET, SOCK_STREAM, 0);
    if (!client_conn) {
        perror("can not create socket!!");
        return SDL_FALSE;
    }

    struct sockaddr_in in_addr;
    memset(&in_addr, 0, sizeof(sockaddr_in));

    in_addr.sin_port = htons(9000);
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(client_conn, (struct sockaddr *) &in_addr, sizeof(struct sockaddr));
    if (ret < 0) {
        perror("socket connect error!!\\n");
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

void SocketConnection::close_client() {
    if (client_conn >= 0) {
        shutdown(client_conn, SHUT_RDWR);
        close(client_conn);
        client_conn = 0;
    }
}

int SocketConnection::send_to_(uint8_t *buf, int len) {
    if (!client_conn) {
        return 0;
    }
    return send(client_conn, buf, len, 0);
}

int SocketConnection::recv_from_(uint8_t *buf, int len) {
    if (!client_conn) {
        return 0;
    }
    //rev 和 read 的区别 https://blog.csdn.net/superbfly/article/details/72782264
    return recv(client_conn, buf, len, 0);
}
