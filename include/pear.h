#ifndef PEAR_H
#define PEAR_H

/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define IPC_PIPE_NAME "pear_server_ipc_33"
#define MAX_CONNECTIONS 512
#define WORKER_THREADS 4
#define THREADS WORKER_THREADS + 1

typedef struct
{
    h2o_context_t ctx;

    uv_thread_t thread;

    uv_tcp_t listener;

    /* for giving listen socket to workers */
    uv_pipe_t pipe;

    /* connect worker to main thread via pipe */
    uv_connect_t connect_req;

    uv_sem_t sem;
} pear_thread_t;

typedef struct
{
    MDB_dbi docs;
    MDB_env *db_env;
    h2o_globalconf_t cfg;

    pear_thread_t *threads;

    uv_barrier_t listeners_created_barrier;
} server_t;

extern server_t server;
extern server_t *sv;

H2O_NORETURN void pear_worker_loop(void *_thread_index);
H2O_NORETURN void pear_listen_loop(void *_thread_index);

#endif /* PEAR_H */
