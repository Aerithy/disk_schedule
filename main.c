/**
 * @file main.c
 * @author 19281252 (19281252@bjtu.edu.cn)
 * @brief Mobile head disk scheduling algorithm
 *        FCFS
 *        SSTF
 *        SCAN
 *        CSCAN
 *        FSCAN
 * @version 0.1
 * @date 2022-05-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include "track_schedule.h"
#include "magnetic_track.h"
#include "tq_gen.h"

int main(void) {
    srand(time(NULL));
    struct track_quest_queue* tq_hd;
    struct track_quest_queue* ws_hd;
    struct pending_queue* pq_hd;
    struct track_quest* tq;

    int FCFStime = 0;
    int SSTFtime = 0;
    int SCANtime = 0;
    int CSCANtime = 0;
    int FSCANtime = 0;

    int total_process_time = 0;
    int t = 0;
    int _t = 0;

    m_track_queue_init(&tq_hd, &pq_hd, 100);

    for (int i = 0; i < TEST_SAMPLE; ++i) {
        m_track_queue_update(tq_hd, gen_quest_track(), BYTES_PER_TRACK >> 1, gen_quest_time());
        tq = tq_hd->tq_cr->pr_qt;
        printf("%d. track_id %d, \tquest data amount %d, \tquest time %d\n", i + 1, tq->track_id, tq->reqdata, tq->vreqtime);
    }

    while ((t = track_schedule_SCAN(pq_hd, tq_hd, -1)) > 0) {
        total_process_time = t;
    }

    SCANtime = total_process_time;
    _t = total_process_time;

    m_track_queue_reset(tq_hd, pq_hd);

    while ((t = track_schedule_SCAN(pq_hd, tq_hd, 1)) > 0) {
        total_process_time = t;
    }

    CSCANtime = total_process_time - _t;
    _t = total_process_time;

    m_track_queue_reset(tq_hd, pq_hd);

    while ((t = track_schedule_FSCAN(pq_hd, tq_hd, -1)) > 0) {
        total_process_time = t;
    }

    FSCANtime = total_process_time - _t;
    _t = total_process_time;

    m_track_queue_reset(tq_hd, pq_hd);

    while ((t = track_schedule_FCFS(pq_hd, tq_hd)) > 0) {
        total_process_time = t;
    }

    FCFStime = total_process_time - _t;
    _t = total_process_time;

    m_track_queue_reset(tq_hd, pq_hd);

    while ((t = track_schedule_SSTF(pq_hd, tq_hd)) > 0) {
        total_process_time = t;
    }

    SSTFtime = total_process_time - _t;

    printf("\nSCAN: \n");
    printf("Average processing time: %.2f\n", (float)SCANtime / TEST_SAMPLE);
    printf("\nCSCAN: \n");
    printf("Average processing time: %.2f\n", (float)CSCANtime / TEST_SAMPLE);
    printf("\nFSCAN: \n");
    printf("Average processing time: %.2f\n", (float)FSCANtime / TEST_SAMPLE);
    printf("\nFCFS: \n");
    printf("Average processing time: %.2f\n", (float)FCFStime / TEST_SAMPLE);
    printf("\nSSTF: \n");
    printf("Average processing time: %.2f\n", (float)SSTFtime / TEST_SAMPLE);
    return 0;
}