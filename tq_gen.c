#include "tq_gen.h"

int gen_quest_time() {
    static int s;
    int gen_time = rand() % QUEST_TIME_FIELD + s;
    s = gen_time + 1;

    float x = (float)(rand() % 1000) / 1000.0;
    if (x < 0.2)
        s += GAP;
    return gen_time;
}

int gen_quest_track() {
    return rand() % MAX_TRACK;
}