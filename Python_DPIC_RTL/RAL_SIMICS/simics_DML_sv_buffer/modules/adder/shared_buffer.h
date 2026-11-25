#ifndef SHARED_BUFFER_H
#define SHARED_BUFFER_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BRIDGE_MAGIC 0x53564231u
#define BRIDGE_DEFAULT_NAME "Local\\simics_sv_buffer"

typedef struct {
    uint32_t magic;
    uint32_t server_pid;
    uint32_t input_valid;
    uint32_t input_word;
    uint32_t result_valid;
    uint32_t result_word;
    uint32_t fifo_count;
    uint32_t shutdown;
} bridge_memory;

typedef struct {
    HANDLE mapping;
    HANDLE mutex;
    HANDLE server;
    bridge_memory *memory;
} bridge_handle;

static void bridge_close(bridge_handle *bridge) {
    if (bridge->memory) UnmapViewOfFile(bridge->memory);
    if (bridge->mapping) CloseHandle(bridge->mapping);
    if (bridge->mutex) CloseHandle(bridge->mutex);
    if (bridge->server) CloseHandle(bridge->server);
    memset(bridge, 0, sizeof(*bridge));
}

static int bridge_open(bridge_handle *bridge, int create) {
    const char *name = getenv("SIMICS_SV_BUFFER_NAME");
    char mutex_name[256];
    if (!name || !*name) name = BRIDGE_DEFAULT_NAME;
    if (snprintf(mutex_name, sizeof(mutex_name), "%s_mutex", name) >= (int)sizeof(mutex_name)) {
        fprintf(stderr, "[SV bridge] Shared-memory name is too long\n");
        return -1;
    }

    if (create) {
        bridge->mutex = CreateMutexA(NULL, FALSE, mutex_name);
        if (!bridge->mutex || GetLastError() == ERROR_ALREADY_EXISTS) goto fail;
        bridge->mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                             0, sizeof(bridge_memory), name);
        if (!bridge->mapping || GetLastError() == ERROR_ALREADY_EXISTS) goto fail;
    } else {
        bridge->mutex = OpenMutexA(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE, mutex_name);
        if (!bridge->mutex) goto fail;
        bridge->mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
        if (!bridge->mapping) goto fail;
    }
    bridge->memory = (bridge_memory *)MapViewOfFile(bridge->mapping, FILE_MAP_ALL_ACCESS, 0, 0,
                                                     sizeof(bridge_memory));
    if (!bridge->memory) goto fail;

    if (create) {
        memset(bridge->memory, 0, sizeof(*bridge->memory));
        bridge->memory->server_pid = GetCurrentProcessId();
        bridge->memory->magic = BRIDGE_MAGIC;
    } else {
        if (bridge->memory->magic != BRIDGE_MAGIC) goto fail;
        bridge->server = OpenProcess(SYNCHRONIZE, FALSE, bridge->memory->server_pid);
        if (!bridge->server) goto fail;
    }
    return 0;

fail:
    fprintf(stderr, "[SV bridge] Cannot %s shared memory '%s' (Windows error %lu)\n",
            create ? "create" : "open", name, (unsigned long)GetLastError());
    bridge_close(bridge);
    return -1;
}

static int bridge_lock(bridge_handle *bridge) {
    DWORD wait;
    if (!bridge->memory ||
        (bridge->server && WaitForSingleObject(bridge->server, 0) != WAIT_TIMEOUT))
        return -1;
    wait = WaitForSingleObject(bridge->mutex, 5000);
    if (wait == WAIT_OBJECT_0) return 0;
    if (wait == WAIT_ABANDONED) ReleaseMutex(bridge->mutex);
    fprintf(stderr, "[SV bridge] Shared-memory mutex unavailable (%lu)\n", (unsigned long)wait);
    return -1;
}

static void bridge_unlock(bridge_handle *bridge) {
    if (!ReleaseMutex(bridge->mutex))
        fprintf(stderr, "[SV bridge] ReleaseMutex failed (%lu)\n", (unsigned long)GetLastError());
}

#endif
