#include "sv_client.h"
#include "shared_buffer.h"

static bridge_handle bridge;

static int connect_bridge(void) {
    if (!bridge.memory && bridge_open(&bridge, 0) != 0) return -1;
    if (bridge_lock(&bridge) != 0) {
        bridge_close(&bridge);
        return -1;
    }
    return 0;
}

int sv_buffer_push(uint32_t word) {
    int result = 0;
    if (connect_bridge() != 0) return -1;
    if (bridge.memory->shutdown || bridge.memory->input_valid) {
        result = -1;
    } else {
        bridge.memory->input_word = word;
        bridge.memory->input_valid = 1;
    }
    bridge_unlock(&bridge);
    return result;
}

int sv_buffer_result(uint32_t *word) {
    int result = 0;
    if (connect_bridge() != 0) return -1;
    if (!bridge.memory->result_valid) {
        result = -1;
    } else {
        *word = bridge.memory->result_word;
        bridge.memory->result_valid = 0;
    }
    bridge_unlock(&bridge);
    return result;
}

uint32_t sv_buffer_status(void) {
    uint32_t status;
    if (connect_bridge() != 0) return 0;
    status = 0x80000000u | (bridge.memory->fifo_count << 2);
    if (!bridge.memory->input_valid && !bridge.memory->shutdown) status |= 1u;
    if (bridge.memory->result_valid) status |= 2u;
    bridge_unlock(&bridge);
    return status;
}

int sv_buffer_shutdown(void) {
    if (connect_bridge() != 0) return -1;
    bridge.memory->shutdown = 1;
    bridge_unlock(&bridge);
    return 0;
}
