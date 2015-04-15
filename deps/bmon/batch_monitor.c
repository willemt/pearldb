/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>

/* for nanosleep */
#include <time.h>

#include <uv.h>

#include "assert.h"
#include "heap.h"
#include "batch_monitor.h"

static void __batcher_loop(void *n)
{
    batch_monitor_t* m = n;

    uv_mutex_lock(&m->lock);
    batch_queue_t *bq = &m->queues[m->curr_idx];
    batch_queue_t *prev_bq =
        &m->queues[(m->curr_idx + 1) % MAX_BATCH_QUEUES];
    if (0 < prev_bq->puts_waiting || 0 == heap_count(bq->queue))
    {
        struct timespec tim, tim2;
        tim.tv_sec = 0;
        tim.tv_nsec = m->nanos;

        if (nanosleep(&tim, &tim2) < 0)
            printf("Nano sleep system call failed \n");

        uv_mutex_unlock(&m->lock);
        return;
    }
    m->commit(m, bq);
    m->curr_idx = (m->curr_idx + 1) % MAX_BATCH_QUEUES;
    uv_mutex_unlock(&m->lock);
    uv_cond_signal(&m->done);
}

static void __batcher(void *n)
{
    while (1)
        __batcher_loop(n);
}

int bmon_dispatch(batch_monitor_t* m)
{
    uv_thread_create(&m->thread, __batcher, m);
    return 0;
}

int bmon_offer(batch_monitor_t* m, void* item)
{
    /* offer data to idle batch queue */
    uv_mutex_lock(&m->lock);
    int bq_idx = m->curr_idx;
    batch_queue_t *bq = &m->queues[bq_idx];
    bq->puts_waiting++;
    heap_offer(&bq->queue, item);

    /* wait until our batch queue is emptied by the batch thread */
    while (1)
    {
        if (m->curr_idx != bq_idx)
            break;
        /* signal wasn't meant for us */
        uv_cond_signal(&m->done);
        uv_cond_wait(&m->done, &m->lock);
    }

    /* batch thread wrote our data */
    bq->puts_waiting--;
    uv_mutex_unlock(&m->lock);
    /* let one of our fellow batch threads exit */
    uv_cond_signal(&m->done);

    return 0;
}

int bmon_init(batch_monitor_t* batch,
              int batch_period,
              int (*item_cmp)(
                  void* a, void* b, void* udata),
              int (*commit)(batch_monitor_t* m, batch_queue_t* bq))
{
    uv_mutex_init(&batch->lock);
    uv_cond_init(&batch->done);
    batch->queues[0].queue = heap_new((void*)item_cmp, NULL);
    batch->queues[1].queue = heap_new((void*)item_cmp, NULL);
    batch->nanos = batch_period;
    batch->commit = commit;
    return 0;
}
