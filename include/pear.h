#ifndef PEAR_H
#define PEAR_H

/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define IPC_PIPE_NAME "pear_server_ipc"
#define MAX_CONNECTIONS 512
#define WORKER_THREADS 4
#define THREADS WORKER_THREADS + 1

typedef struct
{
    h2o_context_t ctx;
} pear_thread_t;

typedef struct
{
    MDB_dbi docs;
    MDB_env *db_env;
    h2o_globalconf_t cfg;
    pear_thread_t *threads;
} server_t;

extern server_t server;
extern server_t *sv;

#endif /* PEAR_H */
