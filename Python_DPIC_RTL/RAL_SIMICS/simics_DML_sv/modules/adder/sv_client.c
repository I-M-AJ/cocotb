#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "sv_client.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SOCKET g_sock = INVALID_SOCKET;
static int g_wsa_init = 0;

static int ensure_connected(void) {
    if (g_sock != INVALID_SOCKET) return 0;

    if (!g_wsa_init) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            fprintf(stderr, "[Simics-SV] WSAStartup failed\n");
            return -1;
        }
        g_wsa_init = 1;
    }

    int port = 5555;
    const char *env_port = getenv("SIMICS_SV_PORT");
    if (env_port && atoi(env_port) > 0) {
        port = atoi(env_port);
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons((unsigned short)port);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "[Simics-SV] Failed to connect to QuestaSim SV server on 127.0.0.1:%d\n", port);
        closesocket(s);
        return -1;
    }

    g_sock = s;
    return 0;
}

static int send_cmd(const char *cmd, char *resp, int resp_len) {
    if (ensure_connected() != 0) return -1;

    if (send(g_sock, cmd, (int)strlen(cmd), 0) == SOCKET_ERROR) {
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        return -1;
    }

    int i = 0;
    while (i < resp_len - 1) {
        char c;
        int n = recv(g_sock, &c, 1, 0);
        if (n <= 0) {
            closesocket(g_sock);
            g_sock = INVALID_SOCKET;
            return -1;
        }
        if (c == '\n') break;
        if (c != '\r') resp[i++] = c;
    }
    resp[i] = '\0';
    return 0;
}

void sv_write_a(uint32_t val) {
    char cmd[64], resp[64];
    snprintf(cmd, sizeof(cmd), "A %u\n", (unsigned int)(val & 0xffu));
    send_cmd(cmd, resp, sizeof(resp));
}

void sv_write_b(uint32_t val) {
    char cmd[64], resp[64];
    snprintf(cmd, sizeof(cmd), "B %u\n", (unsigned int)(val & 0xffu));
    send_cmd(cmd, resp, sizeof(resp));
}

uint32_t sv_read_sum(void) {
    char resp[64] = {0};
    if (send_cmd("SUM\n", resp, sizeof(resp)) != 0) return 0;
    return (uint32_t)strtoul(resp, NULL, 10);
}

void sv_reset(void) {
    char resp[64];
    send_cmd("RST\n", resp, sizeof(resp));
}

void sv_disconnect(void) {
    if (g_sock != INVALID_SOCKET) {
        char resp[64];
        send_cmd("QUIT\n", resp, sizeof(resp));
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
    }
}
