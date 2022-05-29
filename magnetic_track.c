#include "magnetic_track.h"

void m_track_queue_init(struct track_quest_queue** tq_hd, struct pending_queue** pq_hd, int track_loc) {
    (*tq_hd) = (struct track_quest_queue*)malloc(sizeof(struct track_quest_queue));
    if (*tq_hd == NULL) {
        printf("Nullpointer detected when creating the track quest queue!\n");
        exit(0);
        return;
    }
    (*tq_hd)->total_quest = 0;
    (*tq_hd)->tq_st = (struct track_quest*)malloc(sizeof(struct track_quest));
    (*tq_hd)->tq_cr = (*tq_hd)->tq_fs = (*tq_hd)->tq_st;
    (*tq_hd)->tq_st->pr_qt = NULL;
    (*tq_hd)->tq_cr->nt_qt = NULL;

    (*pq_hd) = (struct pending_queue*)malloc(sizeof(struct pending_queue));
    if (*pq_hd == NULL) {
        printf("Nullpointer detected when creating the track quest queue!\n");
        exit(0);
        return;
    }
    (*pq_hd)->vruntime = 0;
    (*pq_hd)->track_loc = track_loc;
    (*pq_hd)->momentum = MOVE_DIRECTION;
    (*pq_hd)->tq_processing = (struct track_quest*)malloc(sizeof(struct track_quest));
    (*pq_hd)->assist = (*pq_hd)->pq_cr = (*pq_hd)->tq_processing;
    (*pq_hd)->tq_processing->pr_qt = NULL;
    (*pq_hd)->pq_cr->nt_qt = NULL;
    return;
}

void m_track_queue_reset(struct track_quest_queue* tq_hd, struct pending_queue* pq_hd) {
    tq_hd->tq_fs = tq_hd->tq_st;
    pq_hd->track_loc = TRACK_START;
    pq_hd->momentum = MOVE_DIRECTION;
    pq_hd->vruntime = 0;
}
// Update the track quest queue with a new track request

void m_track_queue_update(struct track_quest_queue* tq_hd, int track_id, int reqdata, int vreqtime) {
    tq_hd->tq_cr->reqdata = reqdata;
    tq_hd->tq_cr->vreqtime = vreqtime;
    tq_hd->tq_cr->track_id = track_id;

    struct track_quest* p = tq_hd->tq_cr;
    p->nt_qt = (struct track_quest*)malloc(sizeof(struct track_quest));
    p->nt_qt->pr_qt = p;
    tq_hd->tq_cr = p->nt_qt;
    tq_hd->tq_cr->nt_qt = NULL;
}

int m_track_quest_time(struct track_quest* tq, int current_head_loc) {
    float res = 1000.0 / (2 * R) + (float)(tq->reqdata) / (R * BYTES_PER_TRACK) + M * abs(current_head_loc - tq->track_id) + 2;
    return (int)res;
}

// After finishing a schedule in pending queue

/*
struct track_quest* m_track_queue_select(struct track_quest_queue* tq_hd, struct pending_queue* pq_hd) {
    struct track_quest* res_tq;

    if (pq_hd->tq_processing == pq_hd->pq_cr) {
        if (tq_hd->tq_st == tq_hd->tq_cr) {
            return NULL;
        }
        res_tq = tq_hd->tq_st;
        tq_hd->tq_st = tq_hd->tq_st->nt_qt;
        tq_hd->tq_st->pr_qt = NULL;

        if (pq_hd->vruntime <= res_tq->vreqtime)
            pq_hd->vruntime = res_tq->vreqtime;

        return res_tq;
    }
    else {
        if (tq_hd->tq_st == tq_hd->tq_cr)
            return NULL;
        if (tq_hd->tq_st->vreqtime <= pq_hd->vruntime) {
            res_tq = tq_hd->tq_st;
            tq_hd->tq_st = tq_hd->tq_st->nt_qt;

            return res_tq;
        }
        else
            return NULL;
    }
}*/

struct track_quest* m_track_queue_select(struct track_quest_queue* tq_hd, struct pending_queue* pq_hd) {
    struct track_quest* res_tq;

    if (pq_hd->tq_processing == pq_hd->pq_cr) {
        if (tq_hd->tq_fs == tq_hd->tq_cr) {
            return NULL;
        }
        res_tq = tq_hd->tq_fs;
        tq_hd->tq_fs = tq_hd->tq_fs->nt_qt;
        // tq_hd->tq_st->pr_qt = NULL;

        if (pq_hd->vruntime <= res_tq->vreqtime)
            pq_hd->vruntime = res_tq->vreqtime;

        return res_tq;
    }
    else {
        if (tq_hd->tq_fs == tq_hd->tq_cr)
            return NULL;
        if (tq_hd->tq_fs->vreqtime <= pq_hd->vruntime) {
            res_tq = tq_hd->tq_fs;
            tq_hd->tq_fs = tq_hd->tq_fs->nt_qt;

            return res_tq;
        }
        else
            return NULL;
    }
}