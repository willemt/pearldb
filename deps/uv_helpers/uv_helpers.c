#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "uv.h"
#include "uv_helpers.h"

void uv_bind_listen_socket(uv_tcp_t* listen, const char* host, const int port)
{
    int e;

    uv_loop_t *loop = uv_default_loop();

    e = uv_loop_init(loop);
    if (e != 0)
        uv_fatal(e);

    e = uv_tcp_init(loop, listen);
    if (e != 0)
        uv_fatal(e);

    struct sockaddr_in addr;
    e = uv_ip4_addr(host, port, &addr);
    if (e != 0)
        uv_fatal(e);

    e = uv_tcp_bind(listen, (struct sockaddr *)&addr, 0);
    if (e != 0)
        uv_fatal(e);
}
