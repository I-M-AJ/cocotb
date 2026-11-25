#include "../modules/adder/shared_buffer.h"
#include "svdpi.h"

static bridge_handle bridge;

int sv_bridge_start(void) {
    return bridge_open(&bridge, 1);
}

int sv_bridge_poll(unsigned int *word, int can_take, unsigned int fifo_count) {
    int result = 0;
    if (bridge_lock(&bridge) != 0) return -1;
    bridge.memory->fifo_count = fifo_count;
    if (bridge.memory->shutdown) {
        result = -2;
    } else if (can_take && bridge.memory->input_valid) {
        *word = bridge.memory->input_word;
        bridge.memory->input_valid = 0;
        result = 1;
    }
    bridge_unlock(&bridge);
    if (result == 0) Sleep(1);
    return result;
}

int sv_bridge_publish(unsigned int word) {
    int result = 0;
    if (bridge_lock(&bridge) != 0) return -1;
    if (bridge.memory->result_valid) {
        result = 0;
    } else {
        bridge.memory->result_word = word;
        bridge.memory->result_valid = 1;
        result = 1;
    }
    bridge_unlock(&bridge);
    return result;
}

void sv_bridge_close(void) {
    bridge_close(&bridge);
}
