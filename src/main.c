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
#include "docopt.c"

#include "pear.h"

#include "assert.h"

server_t server;
server_t *sv = &server;

static void __register_handler(h2o_hostconf_t *hostconf, const char *path, int (*on_req)(
                                   h2o_handler_t *,
                                   h2o_req_t *))
{
    h2o_pathconf_t *pathconf = h2o_config_register_path(hostconf, path);
    h2o_handler_t *handler = h2o_create_handler(pathconf, sizeof(*handler));
    handler->on_req = on_req;
}

static int __put(h2o_req_t *req, kstr_t* key)
{
    static h2o_generator_t generator = { NULL, NULL };
    MDB_txn *txn;
    int e;

    e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
    {
        perror("can't create transaction");
        abort();
    }

    MDB_val k = { .mv_size = key->len,
                  .mv_data = key->s };
    MDB_val v = { .mv_size = req->entity.len,
                  .mv_data = (void*)req->entity.base };

    e = mdb_put(txn, sv->docs, &k, &v, 0);
    if (0 != e)
    {
        perror("mdm put failed");
        abort();
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
    {
        perror("can't commit transaction");
        abort();
    }

    h2o_iovec_t body;
    body.len = 0;
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
    req->res.status = 400;
    req->res.reason = "BAD";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 1;
}

static int __get(h2o_req_t *req, kstr_t* key)
{
    static h2o_generator_t generator = { NULL, NULL };
    h2o_iovec_t body;
    MDB_txn *txn;
    int e;

    e = mdb_txn_begin(sv->db_env, NULL, MDB_RDONLY, &txn);
    if (0 != e)
    {
        perror("can't create transaction");
        abort();
    }

    MDB_val k = { .mv_size = key->len,
                  .mv_data = key->s };
    MDB_val v;

    e = mdb_get(txn, sv->docs, &k, &v);
    switch (e)
    {
    case 0:
        break;
    case MDB_NOTFOUND:
        e = mdb_txn_commit(txn);
        if (0 != e)
        {
            perror("can't commit transaction");
            abort();
        }
        req->res.status = 404;
        req->res.reason = "NOT FOUND";
        body.len = 0;
        h2o_start_response(req, &generator);
        h2o_send(req, &body, 1, 1);
        return 0;
    default:
        goto fail;
    }

    if (0 != e)
    {
        perror("mdm get failed");
        abort();
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
    {
        perror("can't commit transaction");
        abort();
    }

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
    body.len = 0;
    req->res.status = 400;
    req->res.reason = "BAD";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return -1;
}

static int __delete(h2o_req_t *req, kstr_t* key)
{
    static h2o_generator_t generator = { NULL, NULL };
    h2o_iovec_t body;
    int e;

    body.len = 0;

    MDB_txn *txn;

    e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
    {
        perror("can't create transaction");
        abort();
    }

    MDB_val k = { .mv_size = key->len,
                  .mv_data = key->s };

    e = mdb_del(txn, sv->docs, &k, NULL);
    switch (e)
    {
    case 0:
        break;
    case MDB_NOTFOUND:
        e = mdb_txn_commit(txn);
        if (0 != e)
        {
            perror("can't commit transaction");
            abort();
        }
        req->res.status = 404;
        req->res.reason = "NOT FOUND";
        body.len = 0;
        h2o_start_response(req, &generator);
        h2o_send(req, &body, 1, 1);
        return 0;
    default:
        goto fail;
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
    {
        perror("can't commit transaction");
        abort();
    }

    req->res.status = 200;
    req->res.reason = "OK";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
fail:
    req->res.status = 400;
    req->res.reason = "BAD";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return -1;
}

static int __pear(h2o_handler_t * self, h2o_req_t * req)
{
    h2o_iovec_t body;
    static h2o_generator_t generator = { NULL, NULL };

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
    body.len = 0;
    req->res.status = 400;
    req->res.reason = "BAD";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
}

static h2o_globalconf_t config;
static h2o_context_t ctx;

static void __on_accept(uv_stream_t * listener, int status)
{
    uv_tcp_t *conn;
    h2o_socket_t *sock;

    if (status != 0)
        return;

    conn = h2o_mem_alloc(sizeof(*conn));
    uv_tcp_init(listener->loop, conn);

    if (uv_accept(listener, (uv_stream_t*)conn) != 0)
    {
        uv_close((uv_handle_t*)conn, (uv_close_cb)free);
        return;
    }

    sock = h2o_uv_socket_create((uv_stream_t*)conn, NULL, 0,
                                (uv_close_cb)free);
    h2o_http1_accept(&ctx, ctx.globalconf->hosts, sock);
}

static int __create_listener(void)
{
    static uv_tcp_t listener;
    struct sockaddr_in addr;
    int e;

    uv_tcp_init(ctx.loop, &listener);
    uv_ip4_addr("127.0.0.1", 8888, &addr);
    e = uv_tcp_bind(&listener, (struct sockaddr *)&addr, 0);
    if (e != 0)
    {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(e));
        goto fail;
    }
    e = uv_listen((uv_stream_t*)&listener, 128, __on_accept);
    if (e != 0)
    {
        fprintf(stderr, "uv_listen:%s\n", uv_strerror(e));
        goto fail;
    }

    return 0;
fail:
    uv_close((uv_handle_t*)&listener, NULL);
    return e;
}

static void __db_env_create(MDB_dbi *dbi, MDB_env **env, const char* path)
{
    int e;

    e = mkdir(path, 0777);

    e = mdb_env_create(env);
    if (0 != e)
    {
        perror("can't create lmdb env");
        abort();
    }

    e = mdb_env_set_mapsize(*env, 1048576000);
    if (0 != e)
    {
        perror("can't set map size");
        abort();
    }

    e = mdb_env_set_maxdbs(*env, 1024);
    if (0 != e)
    {
        perror(mdb_strerror(e));
        abort();
    }

    e = mdb_env_open(*env, path,  MDB_WRITEMAP, 0664);
    if (0 != e)
    {
        perror(mdb_strerror(e));
        abort();
    }
}

static void __db_create(MDB_dbi *dbi, MDB_env *env, const char* db_name)
{
    int e;
    MDB_txn *txn;

    e = mdb_txn_begin(env, NULL, 0, &txn);
    if (0 != e)
    {
        perror("can't create transaction");
        abort();
    }

    e = mdb_dbi_open(txn, db_name, MDB_CREATE, dbi);
    if (0 != e)
    {
        perror("can't create lmdb db");
        abort();
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
    {
        perror("can't create transaction");
        abort();
    }
}

static void __new_server(server_t* sv, const char* db_path)
{
    __db_env_create(&sv->docs, &sv->db_env, db_path);
    __db_create(&sv->docs, sv->db_env, "docs");
}

int main(int argc, char **argv)
{
    DocoptArgs args = docopt(argc, argv, 1, "0.1");

    __new_server(sv, args.db_path ? args.db_path : "store");

    if (args.daemonize)
    {
        int ret = daemon(1, 0);
        if (-1 == ret)
            abort();
    }
    else
        signal(SIGPIPE, SIG_IGN);

    h2o_config_init(&config);
    h2o_hostconf_t *hostconf = h2o_config_register_host(&config, "default");
    __register_handler(hostconf, "/", __pear);

    uv_loop_t loop;
    uv_loop_init(&loop);
    h2o_context_init(&ctx, &loop, &config);

    if (__create_listener() != 0)
    {
        fprintf(stderr, "failed to listen to 127.0.0.1:8888:%s\n",
                strerror(errno));
        goto fail;
    }

    uv_run(ctx.loop, UV_RUN_DEFAULT);

fail:
    return 1;
}
