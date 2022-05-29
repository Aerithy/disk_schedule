#ifndef _TRACK_SCHEDULE_H_
#define _TRACK_SCHEDULE_H_

#include <stdio.h>
#include "magnetic_track.h"
void track_show(struct pending_queue* pq_hd);

int track_sche_process(struct pending_queue* pq_hd);

int track_schedule_FCFS(struct pending_queue* pq_hd, struct track_quest_queue* tq);

int track_schedule_SSTF(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd);

int track_schedule_SCAN(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd, int scan_move_style);

int track_schedule_FSCAN(struct pending_queue* pq_hd, struct track_quest_queue* tq_hd, int scan_move_style);
#endif