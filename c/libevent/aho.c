#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <err.h>
#include <event.h>
#include <evhttp.h>

void now_handler(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf;
    buf = evbuffer_new();

    if (buf == NULL)
        err(1, "failed to create response buffer");

    evbuffer_add_printf(buf, "{\"now\":%d}", (int)time(NULL));
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

int main(int argc, char **argv)
{
    struct evhttp *httpd;

    event_init();
    httpd = evhttp_start("0.0.0.0", 8888 );
    if ( httpd == NULL ){
        fprintf(stderr, "Start server error: %m\n");
        exit(1);
    }

    /* Set a callback for requests to "/specific". */
    /* evhttp_set_cb(httpd, "/specific", another_handler, NULL); */

    /* Set a callback for all other requests. */
    evhttp_set_gencb(httpd, now_handler, NULL);

    event_dispatch();

    /* Not reached in this code as it is now. */
    evhttp_free(httpd);

    return 0;
}
