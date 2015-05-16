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
#include "kstr.h"
#include "heap.h"
#include "assert.h"
#include "uv_multiplex.h"
#include "batch_monitor.h"

#include "pear.h"

#include "usage.c"

#define min(a, b) ((a) < (b) ? (a) : (b))

#define mdb_fatal(e) { \
        assert(0 != e); \
        fprintf(stderr, "%s:%d - err:%d: %s\n", \
                __FILE__, __LINE__, e, mdb_strerror((e))); \
        exit(1); }

#define uv_fatal(e) { \
        assert(0 != e); \
        fprintf(stderr, "%s:%d - err:%s: %s\n", \
                __FILE__, __LINE__, uv_err_name((e)), uv_strerror((e))); \
        exit(1); }

typedef struct
{
    MDB_val key;
    MDB_val val;
} batch_item_t;

int batch_item_cmp(batch_item_t* a, batch_item_t* b, void* udata)
{
    return strncmp(a->key.mv_data, b->key.mv_data,
                   min(a->key.mv_size, b->key.mv_size));
}

server_t server;
server_t *sv = &server;

static int __http_error(h2o_req_t *req, int status_code, const char* reason)
{
    static h2o_generator_t generator = { NULL, NULL };
    static h2o_iovec_t body = { .base = "", .len = 0 };
    req->res.status = status_code;
    req->res.reason = reason;
    h2o_add_header(&req->pool,
                   &req->res.headers,
                   H2O_TOKEN_CONTENT_LENGTH,
                   H2O_STRLIT("0"));
    h2o_start_response(req, &generator);
    /* force keep-alive */
    req->http1_is_persistent = 1;
    h2o_send(req, &body, 1, 1);
    return 0;
}

static int __http_success(h2o_req_t *req, int status_code)
{
    static h2o_generator_t generator = { NULL, NULL };
    static h2o_iovec_t body = { .base = "", .len = 0 };
    req->res.status = status_code;
    req->res.reason = "OK";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
}

#define BATCHER_ERROR_LEN 128
char batcher_error[BATCHER_ERROR_LEN];

static int __batcher_commit(batch_monitor_t* m, batch_queue_t* bq)
{
    if (0 == heap_count(bq->queue))
        return 0;

    MDB_txn *txn;
    int e;

    e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

    while (0 < heap_count(bq->queue))
    {
        batch_item_t* item = heap_poll(bq->queue);
        e = mdb_put(txn, sv->docs, &item->key, &item->val, 0);
        if (MDB_MAP_FULL == e)
        {
            mdb_txn_abort(txn);
            while ((item = heap_poll(bq->queue)))
                ;
            snprintf(batcher_error, BATCHER_ERROR_LEN, "NOT ENOUGH SPACE");
            return -1;
        }
        else if (0 != e)
            mdb_fatal(e);
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    return 0;
}

static int __put(h2o_req_t *req, kstr_t* key)
{
    int e;
    batch_item_t item;
    item.key.mv_data = key->s;
    item.key.mv_size = key->len;
    item.val.mv_data = req->entity.base;
    item.val.mv_size = req->entity.len;
    e = bmon_offer(&sv->batch, &item);
    if (-1 == e)
        return __http_error(req, 400, batcher_error);
    return __http_success(req, 200);
}

static int __get(h2o_req_t *req, kstr_t* key)
{
    static h2o_generator_t generator = { NULL, NULL };
    h2o_iovec_t body;
    MDB_txn *txn;
    int e;

    e = mdb_txn_begin(sv->db_env, NULL, MDB_RDONLY, &txn);
    if (0 != e)
        mdb_fatal(e);

    MDB_val k = { .mv_size = key->len, .mv_data = key->s };
    MDB_val v;

    e = mdb_get(txn, sv->docs, &k, &v);
    switch (e)
    {
    case 0:
        break;
    case MDB_NOTFOUND:
        e = mdb_txn_commit(txn);
        if (0 != e)
            mdb_fatal(e);
        return __http_error(req, 404, "NOT FOUND");
    default:
        goto fail;
    }

    if (0 != e)
        mdb_fatal(e);

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    body.base = v.mv_data;
    body.len = v.mv_size;
    req->res.status = 200;
    req->res.reason = "OK";
    h2o_add_header(&req->pool,
                   &req->res.headers,
                   H2O_TOKEN_CONTENT_TYPE,
                   H2O_STRLIT("text/plain; charset=utf-8"));
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
fail:
    return __http_error(req, 400, "BAD");
}

static int __delete(h2o_req_t *req, kstr_t* key)
{
    int e;
    MDB_txn *txn;

    e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

    MDB_val k = { .mv_size = key->len, .mv_data = key->s };

    e = mdb_del(txn, sv->docs, &k, NULL);
    switch (e)
    {
    case 0:
        break;
    case MDB_NOTFOUND:
        e = mdb_txn_commit(txn);
        if (0 != e)
            mdb_fatal(e);
        return __http_error(req, 404, "NOT FOUND");
    default:
        goto fail;
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    return __http_success(req, 200);
fail:
    return __http_error(req, 400, "BAD");
}

static int __dispatch(h2o_handler_t * self, h2o_req_t * req)
{
    /* get key */
    char* end;
    kstr_t key;
    key.s = req->path.base + 1;
    end = strchr(key.s, '/');
    if (!end)
        goto fail;
    key.len = end - key.s;

    if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("PUT")))
        return __put(req, &key);
    else if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("GET")))
        return __get(req, &key);
    else if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("DELETE")))
        return __delete(req, &key);

fail:
    return __http_error(req, 400, "BAD");
}

static void __db_env_create(MDB_dbi *dbi, MDB_env **env, const char* path,
                            int size_mb)
{
    int e;

    mkdir(path, 0777);

    e = mdb_env_create(env);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_env_set_mapsize(*env, size_mb * 1024 * 1024);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_env_set_maxdbs(*env, 1024);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_env_open(*env, path,  MDB_WRITEMAP, 0664);
    if (22 == e)
    {
        fprintf(stderr,
                "ERROR:\tThe current database file path (%s) is not mmap-able\n"
                "\tPlease consider using a different database path\n", path);
        exit(1);
    }
    else if (0 != e)
        mdb_fatal(e);
}

static void __db_create(MDB_dbi *dbi, MDB_env *env, const char* db_name)
{
    int e;
    MDB_txn *txn;

    e = mdb_txn_begin(env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_dbi_open(txn, db_name, MDB_CREATE, dbi);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);
}

static void __on_accept(uv_stream_t * listener, int status)
{
    pear_thread_t* thread = listener->data;
    int e;

    if (0 != status)
        uv_fatal(status);

    uv_tcp_t *conn = malloc(sizeof(*conn));

    e = uv_tcp_init(listener->loop, conn);
    if (0 != status)
        uv_fatal(e);

    e = uv_accept(listener, (uv_stream_t*)conn);
    if (0 != e)
        uv_fatal(e);

    h2o_socket_t *sock =
        h2o_uv_socket_create((uv_stream_t*)conn, NULL, 0, (uv_close_cb)free);
    h2o_http1_accept(&thread->ctx, sv->cfg.hosts, sock);
}

static void __worker_start(void* uv_tcp)
{
    assert(uv_tcp);

    int e;
    uv_tcp_t* listener = uv_tcp;
    pear_thread_t* thread = listener->data;

    h2o_context_init(&thread->ctx, listener->loop, &sv->cfg);

    e = uv_listen((uv_stream_t*)listener, MAX_CONNECTIONS, __on_accept);
    if (e != 0)
        uv_fatal(e);

    while (1)
        uv_run(listener->loop, UV_RUN_DEFAULT);
}

static void __print_db_stats(MDB_dbi dbi, MDB_env *env)
{
    int e;
    MDB_stat stat;
    MDB_txn *txn;

    e = mdb_txn_begin(env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_stat(txn, dbi, &stat);
    if (0 != e)
        mdb_fatal(e);

    printf("ms_psize: %d\n", stat.ms_psize);
    printf("ms_depth: %d\n", stat.ms_depth);
    printf("ms_branch_pages: %ld\n", stat.ms_branch_pages);
    printf("ms_leaf_pages: %ld\n", stat.ms_leaf_pages);
    printf("ms_overflow_pages: %ld\n", stat.ms_overflow_pages);
    printf("ms_entries: %ld\n", stat.ms_entries);

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);
}

int main(int argc, char **argv)
{
    int e, i;
    options_t opts;
    uv_multiplex_t m;

    e = parse_options(argc, argv, &opts);
    if (-1 == e)
        exit(-1);
    else if (opts.help)
    {
        show_usage();
        exit(0);
    }

    sv->nworkers = atoi(opts.workers);
    __db_env_create(&sv->docs, &sv->db_env, opts.path, atoi(opts.db_size));
    __db_create(&sv->docs, sv->db_env, "docs");

    if (opts.stat)
    {
        __print_db_stats(sv->docs, sv->db_env);
        exit(0);
    }

    bmon_init(&sv->batch, atoi(opts.batch_period),
              (void*)batch_item_cmp, __batcher_commit);

    if (opts.daemonize)
    {
        int ret = daemon(1, 0);
        if (-1 == ret)
            abort();

        if (opts.pid_file)
        {
            FILE *fp = fopen(opts.pid_file, "wt");
            if (fp == NULL)
            {
                fprintf(stderr, "failed to open pid file:%s:%s\n",
                        opts.pid_file, strerror(errno));
                abort();
            }
            fprintf(fp, "%d\n", (int)getpid());
            fclose(fp);
        }
    }
    else
        signal(SIGPIPE, SIG_IGN);

    h2o_config_init(&sv->cfg);
    h2o_hostconf_t *hostconf = h2o_config_register_host(&sv->cfg, "default");
    h2o_pathconf_t *pathconf = h2o_config_register_path(hostconf, "/");
    h2o_handler_t *handler = h2o_create_handler(pathconf, sizeof(*handler));
    handler->on_req = __dispatch;

    /* Bind HTTP socket */
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_t listen;
    struct sockaddr_in addr;
    uv_loop_init(loop);
    uv_tcp_init(loop, &listen);
    uv_ip4_addr(opts.host, atoi(opts.port), &addr);
    e = uv_tcp_bind(&listen, (struct sockaddr *)&addr, 0);
    if (e != 0)
        uv_fatal(e);

    sv->threads = calloc(sv->nworkers + 1, sizeof(pear_thread_t));

    uv_multiplex_init(&m, &listen, IPC_PIPE_NAME, sv->nworkers, __worker_start);

    /* Start workers */
    for (i = 0; i < sv->nworkers; i++)
    {
        pear_thread_t* thread = &sv->threads[i + 1];
        uv_multiplex_worker_create(&m, i, thread);
    }

    bmon_dispatch(&sv->batch);

    uv_multiplex_dispatch(&m);

    while (1)
        pause();

fail:
    return 1;
}
