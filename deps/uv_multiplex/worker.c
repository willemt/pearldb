
/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "uv.h"
#include "container_of.h"
#include "uv_multiplex.h"

static void __on_ipc_close(uv_handle_t* handle)
{
    //uv_multiplex_worker_t* worker = container_of(handle, uv_multiplex_worker_t, pipe);
    //free(ctx);
}

static void __on_ipc_alloc(uv_handle_t* handle,
                           size_t suggested_size,
                           uv_buf_t* buf)
{
//    uv_multiplex_worker_t* worker = container_of(handle, uv_multiplex_worker_t, pipe);
    buf->base = calloc(1, suggested_size);
    buf->len = suggested_size;
}

/**
 * worker has received the listening socket
 */
static void __on_ipc_read(uv_stream_t* handle,
                          ssize_t nread,
                          const uv_buf_t* buf)
{
    int e;
    uv_multiplex_worker_t* worker;

    worker = container_of(handle, uv_multiplex_worker_t, pipe);

    assert(1 == uv_pipe_pending_count((uv_pipe_t*)handle));

    uv_handle_type type = uv_pipe_pending_type((uv_pipe_t*)handle);

    assert(type == UV_TCP);

    e = uv_tcp_init(worker->pipe.loop, &worker->listener);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_accept(handle, (uv_stream_t*)&worker->listener);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    uv_close((uv_handle_t*)handle, NULL);
}

/**
 * Worker has connected to dispatcher
 * Worker will start reading the listen socket */
static void __on_ipc_connect(uv_connect_t* req, int status)
{
    int e;
    uv_multiplex_worker_t* worker;

    worker = container_of(req, uv_multiplex_worker_t, connect_req);

    if (0 != status)
    {
        fprintf(stderr, "%s\n", uv_strerror(status));
        abort();
    }

    e = uv_read_start((uv_stream_t*)&worker->pipe, __on_ipc_alloc,
                      __on_ipc_read);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }
}

/**
 * Worker will get listen handle from dispatcher
 */
static void __get_listen_handle(uv_loop_t* loop,
                                uv_multiplex_worker_t* worker)
{
    int e;

    e = uv_pipe_init(loop, &worker->pipe, 1);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    uv_pipe_connect(&worker->connect_req, &worker->pipe, worker->m->pipe_name,
                    __on_ipc_connect);

    uv_run(loop, UV_RUN_DEFAULT);
}

static void __worker(void* _worker)
{
    uv_multiplex_worker_t* worker = _worker;

    /* Wait until the main thread is ready. */
    uv_sem_wait(&worker->sem);
    __get_listen_handle(&worker->loop, worker);
    uv_sem_post(&worker->sem);
    worker->m->worker_start(&worker->listener);
}

int uv_multiplex_worker_create(uv_multiplex_t* m,
                               unsigned int worker_id,
                               void* udata)
{
    uv_multiplex_worker_t* worker = &m->workers[worker_id];

    uv_loop_init(&worker->loop);
    worker->listener.data = udata;
    uv_thread_create(&worker->thread, __worker, (void*)worker);
    return 0;
}
