
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WBY_IMPLEMENTATION

#include "web.h"

/* request and websocket handling callback */
static int dispatch(struct wby_con *connection, void *pArg) {
    fprintf(stderr,"dispatch\n");
    return 0;
}
static int websocket_connect(struct wby_con *conn, void *pArg) {
    fprintf(stderr,"connect\n");
    return WBY_OK;
}
static void websocket_connected(struct wby_con *con, void *pArg) {
    fprintf(stderr,"connected\n");
}
static int websocket_frame(struct wby_con *conn, const struct wby_frame *frame, void *pArg) {
    fprintf(stderr,"frame\n");
    return WBY_OK;
}
static void websocket_closed(struct wby_con *connection, void *pArg) {
    fprintf(stderr,"closed\n");
}

int main(int argc, const char * argv[])
{
    /* setup config */
    struct wby_config config;
    memset(&config, 0, sizeof(config));
    config.address = "127.0.0.1";
    config.port = 8888;
    config.connection_max = 8;
    config.request_buffer_size = 2048;
    config.io_buffer_size = 8192;
    config.dispatch = dispatch;
    config.ws_connect = websocket_connect;
    config.ws_connected = websocket_connected;
    config.ws_frame = websocket_frame;
    config.ws_closed = websocket_closed;

    /* compute and allocate needed memory and start server */
    struct wby_server server;
    wby_size needed_memory;
    wby_init(&server, &config, &needed_memory);
    void *memory = calloc(needed_memory, 1);
    wby_start(&server, memory);
    while (1) {
        wby_update(&server);

    }
    wby_stop(&server);
    free(memory);
}
