/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>

/* for pow */
#include <math.h>

/* for daemon */
#include <unistd.h>

/* for mkdir */
#include <sys/stat.h>

#include <assert.h>

#include "h2o.h"
#include "h2o/http1.h"
#include "lmdb.h"
#include "lmdb_helpers.h"
#include "kstr.h"
#include "heap.h"
#include "uv_helpers.h"
#include "uv_multiplex.h"
#include "batch_monitor.h"
#include "b64.h"
#include "ck_ht.h"
#include "pidfile.h"

#include "local.h"

#include "usage.c"
#include "path_parser.c"

#define min(a, b) ((a) < (b) ? (a) : (b))

#define ANYPORT 65535
#define UUID4_LEN 24
#define ETAG_PREFIX_LEN 8
#define ETAG_ID_LEN 20
#define ETAG_LEN ETAG_PREFIX_LEN + ETAG_ID_LEN
#define WOULD_OVERWRITE MDB_MULTIPLE << 1

typedef struct
{
    MDB_val key;
    MDB_val val;
    unsigned int flags;
} batch_item_t;

static int __batch_item_cmp(const batch_item_t* a, const batch_item_t* b,
                            const void* udata)
{
    return strncmp(b->key.mv_data, a->key.mv_data,
                   min(a->key.mv_size, b->key.mv_size));
}

server_t server;
server_t *sv = &server;

static int __http_error(h2o_req_t *req, const int status_code,
                        const char* reason)
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

static int __http_success(h2o_req_t *req, const int status_code)
{
    static h2o_generator_t generator = { NULL, NULL };
    static h2o_iovec_t body = { .base = "", .len = 0 };
    req->res.status = status_code;
    req->res.reason = "OK";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
}

static h2o_iovec_t __get_if_match_header_value(const h2o_req_t* req,
                                               const kstr_t* key)
{
    ssize_t header =
        h2o_find_header(&req->headers, H2O_TOKEN_IF_MATCH, SIZE_MAX);

    if (-1 == header)
        return (h2o_iovec_t){.base = NULL, .len = 0 };

    return req->headers.entries[header].value;
}

/**
 * @return 1 if the client wants an ETag; otherwise 0
 */
static int __prefers_etag(const h2o_req_t* req)
{
    ssize_t header = h2o_find_header_by_str(
        &req->headers, "prefers", strlen("prefers"), SIZE_MAX);

    if (-1 == header)
        return 0;

    if (!strncmp(req->headers.entries[header].value.base, "ETag",
                 min(strlen("ETag"), req->headers.entries[header].value.len)))
        return 1;

    return 0;
}

static int __get_or_create_etag(const kstr_t* key, const MDB_val *val,
                                h2o_iovec_t *etagvec)
{
    int e;
    ck_ht_hash_t hash;
    ck_ht_entry_t entry;
    char* etag = NULL;

    ck_ht_hash(&hash, &sv->etags, key->s, key->len);
    ck_ht_entry_key_set(&entry, key->s, key->len);
    do
    {
        e = ck_ht_get_spmc(&sv->etags, hash, &entry);
        if (0 == e)
        {
            uv_mutex_lock(&sv->etag_lock);
            int num = sv->etag_num;
            sv->etag_num++;
            uv_mutex_unlock(&sv->etag_lock);

            /* TODO: get this memory from a pool */
            etag = malloc(ETAG_LEN);
            if (!etag)
                goto fail;

            snprintf(etag, ETAG_LEN + 1, "%8.8d%20.20d", sv->etag_prefix, num);

            char* my_key = strndup(key->s, key->len);
            if (!my_key)
                goto fail;

            ck_ht_entry_set(&entry, hash, my_key, key->len, etag);
            e = ck_ht_put_spmc(&sv->etags, hash, &entry);
        }
    }
    while (e == 0);

    etagvec->base = ck_ht_entry_value(&entry);
    etagvec->len = ETAG_LEN;
    return 0;

fail:
    if (etag)
        free(etag);
    return -1;
}

static char* __remove_stored_etag(const kstr_t* key)
{
    ck_ht_hash_t hash;
    ck_ht_entry_t entry;

    ck_ht_entry_key_set(&entry, key->s, key->len);
    ck_ht_hash(&hash, &sv->etags, key->s, key->len);
    int e = ck_ht_remove_spmc(&sv->etags, hash, &entry);
    if (1 == e)
        return ck_ht_entry_value(&entry);
    return NULL;
}

static int __should_etag_conditional_put_succeed(const h2o_req_t *req,
                                                 const kstr_t* key,
                                                 const char* server_etag,
                                                 const h2o_iovec_t* client_etag)
{
    return client_etag->len == ETAG_LEN &&
           server_etag &&
           0 == strncmp(server_etag, client_etag->base, ETAG_LEN);
}

#define BATCHER_ERROR_LEN 128
char batcher_error[BATCHER_ERROR_LEN];

static int __batcher_commit(batch_monitor_t* m, batch_queue_t* bq)
{
    if (0 == heap_count(bq->queue))
        return 0;

    MDB_txn *txn;

    int e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

    while (0 < heap_count(bq->queue))
    {
        batch_item_t* item = heap_poll(bq->queue);
        e = mdb_put(txn, sv->docs, &item->key, &item->val, item->flags);
        switch (e)
        {
        case 0:
            break;
        case MDB_MAP_FULL:
        {
            mdb_txn_abort(txn);
            while ((item = heap_poll(bq->queue)))
                ;
            snprintf(batcher_error, BATCHER_ERROR_LEN, "NOT ENOUGH SPACE");
            return -1;
        }
        case MDB_KEYEXIST:
            item->flags = WOULD_OVERWRITE;
            break;
        default:
            mdb_fatal(e);
        }
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    return 0;
}

static int __put(h2o_req_t *req, kstr_t* key)
{
    char* sv_etag = __remove_stored_etag(key);
    h2o_iovec_t cli_etag = __get_if_match_header_value(req, key);
    if (0 < cli_etag.len &&
        !__should_etag_conditional_put_succeed(req, key, sv_etag, &cli_etag))
    {
        if (sv_etag)
            free(sv_etag);
        return __http_error(req, 412, "BAD ETAG");
    }

    if (sv_etag)
        free(sv_etag);

    batch_item_t item = {
        .flags       = 0,
        .key.mv_data = key->s,
        .key.mv_size = key->len,
        .val.mv_data = req->entity.base,
        .val.mv_size = req->entity.len,
    };

    int e = bmon_offer(&sv->batch, &item);
    if (0 != e)
        return __http_error(req, 400, batcher_error);
    return __http_success(req, 200);
}

typedef struct
{
    h2o_generator_t super;
    h2o_req_t *req;
    MDB_txn *txn;
    MDB_cursor* curs;
    kstr_t* key;
} get_keys_generator_t;

static void __get_keys_close(h2o_generator_t *_self, h2o_req_t *req)
{
    get_keys_generator_t *gen = (void*)_self;

    mdb_cursor_close(gen->curs);

    int e = mdb_txn_commit(gen->txn);
    if (0 != e)
        mdb_fatal(e);
}

static void __get_keys_send(get_keys_generator_t *self, int e, MDB_val* k,
                            h2o_req_t *req)
{
    if (0 != e || 0 < strncmp(k->mv_data, self->key->s,
                              min(self->key->len, k->mv_size)))
    {
        h2o_send(req, NULL, 0, 1);
        __get_keys_close((h2o_generator_t*)self, req);
    }
    else
    {
        #define SEND_BUFS 2
        h2o_iovec_t body[SEND_BUFS];
        body[0].base = k->mv_data;
        body[0].len = k->mv_size;
        body[1].base = "\n";
        body[1].len = 1;
        h2o_send(req, body, SEND_BUFS, 0);
    }
}

static void __get_keys_proceed(h2o_generator_t *_self, h2o_req_t *req)
{
    get_keys_generator_t *self = (void*)_self;
    MDB_val k, v;
    int e = mdb_cursor_get(self->curs, &k, &v, MDB_NEXT);
    __get_keys_send(self, e, &k, req);
}

static int __get_keys(h2o_req_t *req, kstr_t* key)
{
    get_keys_generator_t *gen = h2o_mem_alloc_pool(&req->pool, sizeof(*gen));
    gen->super.proceed = __get_keys_proceed;
    gen->super.stop = __get_keys_close;
    gen->req = req;
    gen->key = key;

    req->res.status = 200;
    req->res.reason = "OK";
    h2o_start_response(req, &gen->super);

    int e;

    e = mdb_txn_begin(sv->db_env, NULL, MDB_RDONLY, &gen->txn);
    if (0 != e)
        mdb_fatal(e);

    e = mdb_cursor_open(gen->txn, sv->docs, &gen->curs);
    if (0 != e)
        mdb_fatal(e);

    MDB_val k = { .mv_size = key->len, .mv_data = key->s }, v;
    /* don't care about output, only setting cursor */
    e = mdb_cursor_get(gen->curs, &k, &v, MDB_SET);
    if (0 != e)
        __get_keys_send(gen, mdb_cursor_get(gen->curs, &k, &v,
                                            MDB_FIRST), &k, req);
    else
        __get_keys_send(gen, e, &k, req);

    return 0;
fail:
    return __http_error(req, 400, "BAD");
}

static int __get(h2o_req_t *req, kstr_t* key, const int return_body)
{
    static h2o_generator_t generator = { NULL, NULL };
    MDB_txn *txn;

    int e = mdb_txn_begin(sv->db_env, NULL, MDB_RDONLY, &txn);
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
        mdb_fatal(e);
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    if (__prefers_etag(req))
    {
        h2o_iovec_t etag;

        e = __get_or_create_etag(key, &v, &etag);
        if (-1 == e)
            goto fail;

        h2o_add_header(&req->pool, &req->res.headers,
                       H2O_TOKEN_ETAG,
                       etag.base, etag.len);
    }

    h2o_iovec_t body;
    if (return_body)
    {
        body.base = v.mv_data;
        body.len = v.mv_size;
    }
    else
    {
        body.base = "";
        body.len = 0;
    }

    req->res.status = 200;
    req->res.reason = "OK";
    h2o_start_response(req, &generator);
    h2o_send(req, &body, 1, 1);
    return 0;
fail:
    return __http_error(req, 400, "BAD");
}

static int __delete(h2o_req_t *req, kstr_t * key)
{
    int e;
    MDB_txn *txn;
    MDB_val k = { .mv_size = key->len, .mv_data = key->s };

    e = mdb_txn_begin(sv->db_env, NULL, 0, &txn);
    if (0 != e)
        mdb_fatal(e);

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
        mdb_fatal(e);
    }

    e = mdb_txn_commit(txn);
    if (0 != e)
        mdb_fatal(e);

    return __http_success(req, 200);
fail:
    return __http_error(req, 400, "BAD");
}

static void __generate_uuid4(char* uuid4_str)
{
    unsigned long int buf[2];
    int i;

    /* FIXME: use a better random generator */
    for (i = 0; i < 4; i++)
        ((unsigned int*)buf)[i] = rand();

    b64_encodes((unsigned char*)&buf, sizeof(buf), uuid4_str + 1, UUID4_LEN);
}

static int __post(h2o_req_t *req)
{
    char uuid4_str[1 + UUID4_LEN];

    batch_item_t item = {
        .flags       = MDB_NOOVERWRITE,
        .key.mv_data = uuid4_str + 1,
        .key.mv_size = UUID4_LEN,
        .val.mv_data = req->entity.base,
        .val.mv_size = req->entity.len,
    };

    do
    {
        __generate_uuid4(uuid4_str);
        int e = bmon_offer(&sv->batch, &item);
        if (-1 == e)
            return __http_error(req, 400, batcher_error);
    }
    while (item.flags == WOULD_OVERWRITE);

    uuid4_str[0] = '/';
    h2o_add_header(&req->pool,
                   &req->res.headers,
                   H2O_TOKEN_LOCATION,
                   uuid4_str,
                   1 + UUID4_LEN);
    return __http_success(req, 200);
}

static int __dispatch(h2o_handler_t * self, h2o_req_t *req)
{
    if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("POST")))
    {
        if (1 == req->path.len)
            return __post(req);
        goto fail;
    }

    parse_result_t r;
    int e = parse_path(req->path.base, req->path.len, &r);
    if (-1 == e || 0 == r.key.len)
        return __http_error(req, 400, "BAD PATH");

    if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("PUT")))
        return __put(req, &r.key);
    else if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("GET")))
    {
        if (r.get_keys)
            return __get_keys(req, &r.key);
        return __get(req, &r.key, 1);
    }
    else if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("HEAD")))
        return __get(req, &r.key, 0);
    else if (h2o_memis(req->method.base, req->method.len, H2O_STRLIT("DELETE")))
        return __delete(req, &r.key);
fail:
    return __http_error(req, 400, "BAD");
}

static void __on_accept(uv_stream_t *listener, const int status)
{
    _thread_t* thread = listener->data;
    int e;

    if (0 != status)
        uv_fatal(status);

    uv_tcp_t *conn = calloc(1, sizeof(*conn));

    e = uv_tcp_init(listener->loop, conn);
    if (0 != status)
        uv_fatal(e);

    e = uv_accept(listener, (uv_stream_t*)conn);
    if (0 != e)
        uv_fatal(e);

    h2o_socket_t *sock =
        h2o_uv_socket_create((uv_stream_t*)conn, (uv_close_cb)free);
    h2o_http1_accept(&thread->ctx, sv->cfg.hosts, sock);
}

static void __worker_start(void* uv_tcp)
{
    assert(uv_tcp);

    uv_tcp_t* listener = uv_tcp;
    _thread_t* thread = listener->data;

    h2o_context_init(&thread->ctx, listener->loop, &sv->cfg);

    int e = uv_listen((uv_stream_t*)listener, MAX_CONNECTIONS, __on_accept);
    if (e != 0)
        uv_fatal(e);

    while (1)
        uv_run(listener->loop, UV_RUN_DEFAULT);
}

struct ck_malloc __hs_allocators = {
    .malloc  = (void*)malloc,
    .realloc = (void*)realloc,
    .free    = (void*)free,
};

static void __start_multiplex_workers(uv_multiplex_t* m)
{
    int i;

    for (i = 0; i < sv->nworkers; i++)
    {
        _thread_t* thread = &sv->threads[i + 1];
        uv_multiplex_worker_create(m, i, thread);
    }
}

int main(int argc, char **argv)
{
    int e;
    options_t opts;

    e = parse_options(argc, argv, &opts);
    if (-1 == e)
        exit(-1);
    else if (opts.help)
    {
        show_usage();
        exit(0);
    }

    sv->nworkers = atoi(opts.workers);
    mdb_db_env_create(&sv->docs, &sv->db_env, 0, opts.path, atoi(opts.db_size));
    mdb_db_create(&sv->docs, sv->db_env, "docs");

    if (opts.stat)
    {
        mdb_print_db_stats(sv->docs, sv->db_env);
        exit(0);
    }

    srand(time(NULL));

    sv->etag_num = 0;
    /* ETags are stored in-memory. When we boot we generate a random ETag prefix.
     * This is done to avoid ETags issued before the reboot being mistaken for
     * ETags after the reboot */
    sv->etag_prefix = rand() % (int)(pow(10, ETAG_PREFIX_LEN));
    uv_mutex_init(&sv->etag_lock);
    e = ck_ht_init(&sv->etags, CK_HT_MODE_BYTESTRING, NULL, &__hs_allocators,
                   128, 0);
    if (!e)
        abort();

    bmon_init(&sv->batch,
              atoi(opts.batch_period),
              (void*)__batch_item_cmp,
              __batcher_commit);

    if (opts.daemonize)
    {
        int e = daemon(1, 0);
        if (-1 == e)
            abort();

        if (opts.pid_file)
            pidfile_write(opts.pid_file);
    }
    else
        signal(SIGPIPE, SIG_IGN);

    h2o_pathconf_t *pathconf;
    h2o_handler_t *handler;
    h2o_hostconf_t *hostconf;

    h2o_config_init(&sv->cfg);

    hostconf = h2o_config_register_host(&sv->cfg,
                                        h2o_iovec_init(H2O_STRLIT("default")),
                                        ANYPORT);

    pathconf = h2o_config_register_path(hostconf, "/");
    h2o_chunked_register(pathconf);

    handler = h2o_create_handler(pathconf, sizeof(*handler));
    handler->on_req = __dispatch;

    uv_tcp_t listen;

    uv_bind_listen_socket(&listen, opts.host, atoi(opts.port));

    sv->threads = calloc(sv->nworkers + 1, sizeof(_thread_t));
    if (!sv->threads)
        exit(-1);

    uv_multiplex_t m;
    uv_multiplex_init(&m, &listen, IPC_PIPE_NAME, sv->nworkers, __worker_start);
    __start_multiplex_workers(&m);
    uv_multiplex_dispatch(&m);

    bmon_dispatch(&sv->batch);

    while (1)
        pause();

fail:
    return 1;
}
