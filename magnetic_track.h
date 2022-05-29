#ifndef _MAGNETIC_TRACK_H_
#define _MAGNETIC_TRACK_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define R 500
#define M 0.2
#define TRACK_START 100
#define BYTES_PER_TRACK 18000
#define MAX_TRACK 200
#define MOVE_DIRECTION 1
#define TEST_SAMPLE 100

struct track_quest {
    int track_id;
    int reqdata;
    int vreqtime;
    struct track_quest* nt_qt;
    struct track_quest* pr_qt;
};

struct track_quest_queue {
    int total_quest;
    struct track_quest* tq_st;
    struct track_quest* tq_fs;
    struct track_quest* tq_cr;
};

struct pending_queue {
    int vruntime;
    int track_loc;
    int momentum;
    struct track_quest* tq_processing;
    struct track_quest* pq_cr;
    struct track_quest* assist;
};

void m_track_queue_init(struct track_quest_queue** tq_hd, struct pending_queue** pq_hd, int track_loc);
void m_track_queue_reset(struct track_quest_queue* tq_hd, struct pending_queue* pq_hd);
void m_track_queue_update(struct track_quest_queue* tq_hd, int track_id, int reqdata, int vreqtime);
int m_track_quest_time(struct track_quest* tq, int current_head_loc);
struct track_quest* m_track_queue_select(struct track_quest_queue* tq_hd, struct pending_queue* pq_hd);

#endif