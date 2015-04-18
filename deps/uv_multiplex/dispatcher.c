/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "uv.h"
#include "container_of.h"
#include "uv_multiplex.h"

typedef struct
{
    uv_pipe_t peer_handle;
    uv_write_t write_req;
} ipc_peer_t;

static void __ipc_close_cb(uv_handle_t* handle)
{
    ipc_peer_t* ctx = container_of(handle, ipc_peer_t, peer_handle);
    free(ctx);
}

/**
 * We are sending worker the listening handle
 */
static void __on_ipc_write(uv_write_t* req, int status)
{
    ipc_peer_t* ctx = container_of(req, ipc_peer_t, write_req);
    uv_close((uv_handle_t*)&ctx->peer_handle, __ipc_close_cb);
}

/**
 * worker has contacted us and wants to start listening
 */
static void __on_pipe_connection(uv_stream_t* pipe, int status)
{
    uv_multiplex_t* m = container_of((void*)pipe, uv_multiplex_t, pipe);
    int e = -1;

    uv_buf_t buf = uv_buf_init("PING", 4);

    ipc_peer_t* pc = calloc(1, sizeof(*pc));

    if (pipe->type == UV_TCP)
        e = uv_tcp_init(pipe->loop, (uv_tcp_t*)&pc->peer_handle);
    else if (pipe->type == UV_NAMED_PIPE)
        e = uv_pipe_init(pipe->loop, (uv_pipe_t*)&pc->peer_handle, 1);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        abort();
    }

    e = uv_accept(pipe, (uv_stream_t*)&pc->peer_handle);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        abort();
    }

    /* send the listen socket */
    e = uv_write2(&pc->write_req,
                  (uv_stream_t*)&pc->peer_handle,
                  &buf, 1,
                  (uv_stream_t*)m->listener,
                  __on_ipc_write);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        abort();
    }

    //uv_close((uv_handle_t*) pipe, NULL);
}

int uv_multiplex_dispatch(uv_multiplex_t* m)
{
    int e;
    uv_loop_t* loop = m->listener->loop;

    assert(loop);

    /* create pipe for handing off listen socket */
    e = uv_pipe_init(loop, &m->pipe, 1);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_pipe_bind(&m->pipe, m->pipe_name);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_listen((uv_stream_t*)&m->pipe, 128, __on_pipe_connection);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    int i;

    for (i = 0; i < m->nworkers; i++)
        uv_sem_post(&m->workers[i].sem);

    assert(0 == uv_run(loop, UV_RUN_DEFAULT));
    uv_close((uv_handle_t*)&m->listener, NULL);
    assert(0 == uv_run(loop, UV_RUN_DEFAULT));

    return 0;
}

int uv_multiplex_init(uv_multiplex_t * m,
                      uv_tcp_t* listener,
                      const char* pipe_name,
                      unsigned int nworkers,
                      void (*worker_start)(
                          void* uv_tcp))
{
    int i;

    // TODO make sure pipe is not inuse

    m->listener = listener;
    m->pipe_name = pipe_name;
    m->nworkers = nworkers;
    m->workers = calloc(m->nworkers, sizeof(uv_multiplex_worker_t));
    m->worker_start = worker_start;

    /* remove named pipe */
    unlink(pipe_name);

    for (i = 0; i < nworkers; i++)
    {
        uv_multiplex_worker_t* worker = &m->workers[i];
        worker->m = m;
        uv_sem_init(&worker->sem, 0);
    }

    return 0;
}
