#ifndef UV_BATCH_MONITOR_H
#define UV_BATCH_MONITOR_H

#define MAX_BATCH_QUEUES 2

typedef struct
{
    /* threads that are waiting to save PUT request */
    int puts_waiting;

    /* priority queue for batches
     * we want to write out data using key order */
    heap_t* queue;
} batch_queue_t;

typedef struct batch_monitor_s batch_monitor_t;

struct batch_monitor_s
{
    /* batch monitor is occupied */
    uv_mutex_t lock;

    /* batch has been written
     * threads can now send HTTP response */
    uv_cond_t done;

    /* an idle queue, and a busy queue */
    batch_queue_t queues[MAX_BATCH_QUEUES];

    /* the current batch queue */
    int curr_idx;

    /* number of nanoseconds between batch commits */
    int nanos;

    uv_thread_t thread;

    int (*commit)(batch_monitor_t *m, batch_queue_t* bq);
};


int bmon_init(batch_monitor_t* batch,
              int batch_period,
              int (*item_cmp)(
                  void* a, void* b, void* udata),
              int (*commit)(batch_queue_t* bq));


int bmon_offer(batch_monitor_t* m, void* item);


int bmon_dispatch(batch_monitor_t* m);

#endif /* UV_BATCH_MONITOR_H */
