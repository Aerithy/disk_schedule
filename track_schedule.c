#include "track_schedule.h"

void track_show(struct pending_queue* pq_hd)
{
    struct track_quest* p = pq_hd->tq_processing;
    printf("Current managed pending queue is: \n");
    while (p != pq_hd->pq_cr) {
        if (p == pq_hd->assist)
            printf("turn direction ");
        printf("%d ", p->track_id);
        p = p->nt_qt;
    }
    printf("Direction: %d\n", pq_hd->momentum);
}

int track_sche_process(struct pending_queue* pq_hd) {
    static int total_process_time;
    struct track_quest* p = pq_hd->tq_processing;
    if (p->nt_qt == NULL) {
        printf("Null pending queue\n");
        return 0;
    }
    printf("Next request: %d track, \t%d data, \tquest at %d, \tstart at %d\n",
        p->track_id, p->reqdata, p->vreqtime, pq_hd->vruntime);

    // printf("Processing...\n");

    int time_used = m_track_quest_time(p, pq_hd->track_loc);
    pq_hd->vruntime += time_used;
    total_process_time += time_used;
    pq_hd->track_loc = p->track_id;
    pq_hd->tq_processing = p->nt_qt;
    pq_hd->tq_processing->pr_qt = NULL;

    // printf("End processing, time used %d, \tcurrent disk head %d, \truntime %d\n\n", time_used, pq_hd->track_loc, pq_hd->vruntime);
    return total_process_time;
}

int track_schedule_FCFS(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd) {
    struct track_quest* p = pq_hd->pq_cr;
    struct track_quest* tq;
    struct track_quest* mq;

    // update the pending queue by FCFS
    while ((mq = m_track_queue_select(tq_hd, pq_hd)) != NULL) {
        tq = (struct track_quest*)malloc(sizeof(struct track_quest));
        tq->reqdata = mq->reqdata;
        tq->track_id = mq->track_id;
        tq->vreqtime = mq->vreqtime;
        tq->pr_qt = p->pr_qt;
        tq->nt_qt = p;

        if (p->pr_qt == NULL)
            pq_hd->tq_processing = tq;
        else
            p->pr_qt->nt_qt = tq;
        p->pr_qt = tq;
    }

    return track_sche_process(pq_hd);
}

int track_schedule_SSTF(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd) {
   
    struct track_quest* p = pq_hd->pq_cr;
    struct track_quest* tq = NULL;
    struct track_quest* mq;

    // update the pending queue
    int nearst, prev_loc;
    while ((mq = m_track_queue_select(tq_hd, pq_hd)) != NULL) {
        tq = (struct track_quest*)malloc(sizeof(struct track_quest));
        tq->reqdata = mq->reqdata;
        tq->track_id = mq->track_id;
        tq->vreqtime = mq->vreqtime;
        tq->pr_qt = p->pr_qt;
        tq->nt_qt = p;

        if (p->pr_qt == NULL)
            pq_hd->tq_processing = tq;
        else
            p->pr_qt->nt_qt = tq;
        p->pr_qt = tq;
    }

    nearst = MAX_TRACK;
    prev_loc = pq_hd->track_loc;
    p = pq_hd->tq_processing;
    // if tq's stack is nearer than the p->nt_qt, 
    while (p != pq_hd->pq_cr) {
        if (nearst > abs(p->track_id - prev_loc))
            tq = p;
        p = p->nt_qt;
    }

    if (tq == NULL)
        return track_sche_process(pq_hd);

    if (tq == pq_hd->tq_processing)
        return track_sche_process(pq_hd);

    tq->pr_qt->nt_qt = tq->nt_qt;
    tq->nt_qt->pr_qt = tq->pr_qt;

    p = pq_hd->tq_processing;
    p->pr_qt = tq;
    pq_hd->tq_processing = tq;

    tq->nt_qt = p;
    tq->pr_qt = NULL;

    return track_sche_process(pq_hd);
}

// scan_move_style determines whether the disk head move will follow the SCAN(-1) or the CSCAN(1) way
int track_schedule_SCAN(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd, int scan_move_style)
{
    struct track_quest* p;
    struct track_quest* q;
    struct track_quest* tq;
    struct track_quest* mq;

    // update the pending queue by SCAN
    while ((mq = m_track_queue_select(tq_hd, pq_hd)) != NULL) {
        tq = (struct track_quest*)malloc(sizeof(struct track_quest));
        tq->reqdata = mq->reqdata;
        tq->track_id = mq->track_id;
        tq->vreqtime = mq->vreqtime;

        if ((tq->track_id - pq_hd->track_loc) * pq_hd->momentum > 0) { // This quest is located at the same direction of the disk head 
            p = pq_hd->tq_processing;
            while (p != pq_hd->assist) {
                if ((tq->track_id - p->track_id) * pq_hd->momentum <= 0)
                    break;
                p = p->nt_qt;
            }
            tq->pr_qt = p->pr_qt;
            tq->nt_qt = p;

            if (p->pr_qt == NULL) 
                pq_hd->tq_processing = tq;
            else 
                p->pr_qt->nt_qt = tq;
            p->pr_qt = tq;
        }
        else {
            q = pq_hd->assist;
            while (q != pq_hd->pq_cr) {
                if ((tq->track_id - q->track_id) * pq_hd->momentum * scan_move_style <= 0)
                    break;
                q = q->nt_qt;
            }
            tq->pr_qt = q->pr_qt;
            tq->nt_qt = q;

            if (q->pr_qt == NULL)
                pq_hd->tq_processing = pq_hd->assist = tq;
            else if (q == pq_hd->assist)
            {
                q->pr_qt->nt_qt = tq;
                pq_hd->assist = tq;
            }
            else
                q->pr_qt->nt_qt = tq;
            q->pr_qt = tq;
        }
    }

    if (pq_hd->tq_processing == pq_hd->assist)
    {
        pq_hd->momentum = scan_move_style * pq_hd->momentum;
        pq_hd->assist = pq_hd->pq_cr;
    }

    // track_show(pq_hd);

    return track_sche_process(pq_hd);
}

int track_schedule_FSCAN(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd, int scan_move_style)
{
    struct track_quest* p;
    struct track_quest* q;
    struct track_quest* tq;
    struct track_quest* mq;

    // update the pending queue by SCAN
    if (pq_hd->pq_cr == pq_hd->tq_processing) {

        while ((mq = m_track_queue_select(tq_hd, pq_hd)) != NULL) {
            tq = (struct track_quest*)malloc(sizeof(struct track_quest));
            tq->reqdata = mq->reqdata;
            tq->track_id = mq->track_id;
            tq->vreqtime = mq->vreqtime;
            if ((tq->track_id - pq_hd->track_loc) * pq_hd->momentum > 0) { // This quest is located at the same direction of the disk head 
                p = pq_hd->tq_processing;
                while (p != pq_hd->assist) {
                    if ((tq->track_id - p->track_id) * pq_hd->momentum <= 0)
                        break;
                    p = p->nt_qt;
                }
                tq->pr_qt = p->pr_qt;
                tq->nt_qt = p;

                if (p->pr_qt == NULL)
                    pq_hd->tq_processing = tq;
                else
                    p->pr_qt->nt_qt = tq;
                p->pr_qt = tq;
            }
            else {
                q = pq_hd->assist;
                while (q != pq_hd->pq_cr) {
                    if ((tq->track_id - q->track_id) * pq_hd->momentum * scan_move_style <= 0)
                        break;
                    q = q->nt_qt;
                }
                tq->pr_qt = q->pr_qt;
                tq->nt_qt = q;

                if (q->pr_qt == NULL)
                    pq_hd->tq_processing = pq_hd->assist = tq;
                else if (q == pq_hd->assist)
                {
                    q->pr_qt->nt_qt = tq;
                    pq_hd->assist = tq;
                }
                else
                    q->pr_qt->nt_qt = tq;
                q->pr_qt = tq;
            }
        }
    }
    if (pq_hd->tq_processing == pq_hd->assist)
    {
        pq_hd->momentum = scan_move_style * pq_hd->momentum;
        pq_hd->assist = pq_hd->pq_cr;
    }

    // track_show(pq_hd);

    return track_sche_process(pq_hd);
}
