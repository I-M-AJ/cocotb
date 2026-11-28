#include "../modules/adder/shared_buffer.h"
#include "svdpi.h"

#define C_FIFO_DEPTH 1024u

static bridge_handle bridge;
static uint32_t c_fifo[C_FIFO_DEPTH];
static uint32_t c_fifo_head = 0;
static uint32_t c_fifo_tail = 0;
static uint32_t c_fifo_count = 0;

static void drain_input_mailbox(void) {
    if (bridge.memory->input_valid && c_fifo_count < C_FIFO_DEPTH) {
        c_fifo[c_fifo_tail] = bridge.memory->input_word;
        c_fifo_tail = (c_fifo_tail + 1u) % C_FIFO_DEPTH;
        c_fifo_count++;
        bridge.memory->input_valid = 0;
    }
}

int sv_bridge_start(void) {
    c_fifo_head = 0;
    c_fifo_tail = 0;
    c_fifo_count = 0;
    return bridge_open(&bridge, 1);
}

int sv_bridge_poll(unsigned int *word, int can_take) {
    int result = 0;
    if (bridge_lock(&bridge) != 0) return -1;
    if (bridge.memory->shutdown) {
        result = -2;
    } else {
        drain_input_mailbox();
        if (can_take && c_fifo_count > 0) {
            *word = c_fifo[c_fifo_head];
            c_fifo_head = (c_fifo_head + 1u) % C_FIFO_DEPTH;
            c_fifo_count--;
            result = 1;
            drain_input_mailbox();
        }
    }
    bridge.memory->fifo_count = c_fifo_count;
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
