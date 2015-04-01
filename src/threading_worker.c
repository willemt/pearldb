/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* for daemon */
#include <unistd.h>

/* for mkdir */
#include <sys/stat.h>

#include "h2o.h"
#include "h2o/http1.h"
#include "lmdb.h"
#include "container_of.h"

#include "pear.h"

#include "assert.h"

/**
 * worker just accepted connection with client
 */
static void __on_accept(uv_stream_t * listener, int status)
{
    pear_thread_t* thread = container_of(listener, pear_thread_t, listener);
    int e;

    if (0 != status)
    {
        fprintf(stderr, "%s\n", uv_strerror(status));
        abort();
    }

    //uv_tcp_t *conn = h2o_mem_alloc(sizeof(*conn));
    uv_tcp_t *conn = malloc(sizeof(*conn));

    e = uv_tcp_init(listener->loop, conn);
    if (0 != status)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_accept(listener, (uv_stream_t*)conn);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        uv_close((uv_handle_t*)conn, (uv_close_cb)free);
        abort();
    }

    //h2o_context_init(&thread->ctx, &loop, &sv->cfg);

    h2o_socket_t *sock =
        h2o_uv_socket_create((uv_stream_t*)conn, NULL, 0, (uv_close_cb)free);
    h2o_http1_accept(&thread->ctx, sv->cfg.hosts, sock);
}

static void __on_ipc_close(uv_handle_t* handle)
{
    //pear_thread_t* thread = container_of(handle, pear_thread_t, pipe);
    //free(ctx);
}

static void __on_ipc_alloc(uv_handle_t* handle, size_t suggested_size,
                           uv_buf_t* buf)
{
//    pear_thread_t* thread = container_of(handle, pear_thread_t, pipe);
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
    pear_thread_t* thread = container_of(handle, pear_thread_t, pipe);
    int e;

    assert(1 == uv_pipe_pending_count((uv_pipe_t*)handle));

    uv_handle_type type = uv_pipe_pending_type((uv_pipe_t*)handle);

    assert(type == UV_TCP);

    e = uv_tcp_init(thread->pipe.loop, &thread->listener);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    e = uv_accept(handle, (uv_stream_t*)&thread->listener);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    uv_close((uv_handle_t*)handle, NULL);
}

/**
 * worker has connected to main thread.
 * it will start reading the listen socket */
static void __on_ipc_connect(uv_connect_t* req, int status)
{
    int e;
    pear_thread_t* thread = container_of(req, pear_thread_t, connect_req);

    if (0 != status)
    {
        fprintf(stderr, "%s\n", uv_strerror(status));
        abort();
    }

    e = uv_read_start((uv_stream_t*)&thread->pipe, __on_ipc_alloc,
                      __on_ipc_read);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }
}

/**
 * worker will get listen handle from main thread
 */
static void __get_listen_handle(uv_loop_t* loop, pear_thread_t* thread)
{
    int e;

    e = uv_pipe_init(loop, &thread->pipe, 1);
    if (0 != e)
    {
        fprintf(stderr, "%s\n", uv_strerror(e));
        abort();
    }

    uv_pipe_connect(&thread->connect_req, &thread->pipe, IPC_PIPE_NAME,
                    __on_ipc_connect);

    uv_run(loop, UV_RUN_DEFAULT);
}

H2O_NORETURN void pear_worker_loop(void *_thread_index)
{
    int e;
    size_t thread_index = (size_t)_thread_index;
    pear_thread_t* thread = &sv->threads[thread_index];

    uv_loop_t loop;
    uv_loop_init(&loop);

    uv_barrier_wait(&sv->listeners_created_barrier);

    /* Wait until the main thread is ready. */
    uv_sem_wait(&thread->sem);
    __get_listen_handle(&loop, thread);
    uv_sem_post(&thread->sem);

    h2o_context_init(&thread->ctx, thread->listener.loop, &sv->cfg);

    e = uv_listen((uv_stream_t*)&thread->listener, MAX_CONNECTIONS, __on_accept);
    if (e != 0)
        fprintf(stderr, "worker uv_listen:%s\n", uv_strerror(e));

    while (1)
        uv_run(&loop, UV_RUN_DEFAULT);
}
