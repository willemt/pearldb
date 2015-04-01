
/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "h2o.h"
#include "h2o/http1.h"
#include "lmdb.h"
#include "container_of.h"

#include "pear.h"

#include "assert.h"

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
    pear_thread_t* thread = container_of((void*)pipe, pear_thread_t, pipe);
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
                  (uv_stream_t*)&thread->listener,
                  __on_ipc_write);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        abort();
    }

    //uv_close((uv_handle_t*) pipe, NULL);
}

/**
 * listen thread passes listen socket to workers via named pipe
 */
H2O_NORETURN void pear_listen_loop(void *_thread_index)
{
    int e;
    struct sockaddr_in addr;
    size_t thread_index = (size_t)_thread_index;
    pear_thread_t* thread = &sv->threads[thread_index];

    uv_loop_t loop;
    uv_loop_init(&loop);

    /* bind HTTP socket */
    thread->listener.data = thread;
    uv_tcp_init(&loop, &thread->listener);
    uv_ip4_addr("127.0.0.1", 8888, &addr);
    e = uv_tcp_bind(&thread->listener, (struct sockaddr *)&addr, 0);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        abort();
    }

    /* create pipe for handing off listen socket */
    e = uv_pipe_init(&loop, &thread->pipe, 1);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_pipe_bind(&thread->pipe, IPC_PIPE_NAME);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_listen((uv_stream_t*)&thread->pipe, 128, __on_pipe_connection);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    int i;

    for (i = 0; i < WORKER_THREADS; i++)
        uv_sem_post(&sv->threads[i + 1].sem);

    uv_barrier_wait(&sv->listeners_created_barrier);

    assert(0 == uv_run(&loop, UV_RUN_DEFAULT));
    uv_close((uv_handle_t*)&thread->listener, NULL);
    assert(0 == uv_run(&loop, UV_RUN_DEFAULT));

    while (1)
        uv_run(&loop, UV_RUN_DEFAULT);
}
