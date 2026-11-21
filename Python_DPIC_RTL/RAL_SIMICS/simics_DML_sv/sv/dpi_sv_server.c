#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "svdpi.h"

#ifdef _WIN32
  #define DPI_EXPORT __declspec(dllexport)
#else
  #define DPI_EXPORT
#endif

extern int sv_write_a(uint32_t val);
extern int sv_write_b(uint32_t val);
extern int sv_read_sum(uint32_t *val);
extern int sv_reset(void);

static int recv_line(SOCKET s, char *buf, int maxlen) {
    int i = 0;
    while (i < maxlen - 1) {
        char c;
        int n = recv(s, &c, 1, 0);
        if (n <= 0) return -1;
        if (c == '\n') break;
        if (c != '\r') buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

static void send_str(SOCKET s, const char *str) {
    send(s, str, (int)strlen(str), 0);
}

DPI_EXPORT int sv_dpi_server_run(int port) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "[SV-DPI] WSAStartup failed\n");
        return 1;
    }

    const char *env_port = getenv("SIMICS_SV_PORT");
    if (env_port && atoi(env_port) > 0) {
        port = atoi(env_port);
    }

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        fprintf(stderr, "[SV-DPI] socket() failed\n");
        WSACleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons((unsigned short)port);

    if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "[SV-DPI] bind() on port %d failed (%d)\n", port, WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    if (listen(listen_sock, 4) == SOCKET_ERROR) {
        fprintf(stderr, "[SV-DPI] listen() failed\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    printf("[SV-DPI] Listening on 127.0.0.1:%d\n", port);
    fflush(stdout);

    int running = 1;
    while (running) {
        SOCKET client = accept(listen_sock, NULL, NULL);
        if (client == INVALID_SOCKET) break;

        char line[128];
        while (recv_line(client, line, sizeof(line)) >= 0) {
            unsigned int val = 0;
            if (sscanf(line, "A %u", &val) == 1) {
                sv_write_a((uint32_t)val);
                send_str(client, "OK\n");
            } else if (sscanf(line, "B %u", &val) == 1) {
                sv_write_b((uint32_t)val);
                send_str(client, "OK\n");
            } else if (strcmp(line, "SUM") == 0) {
                uint32_t sum_val = 0;
                sv_read_sum(&sum_val);
                char resp[64];
                snprintf(resp, sizeof(resp), "%u\n", sum_val);
                send_str(client, resp);
            } else if (strcmp(line, "RST") == 0) {
                sv_reset();
                send_str(client, "OK\n");
            } else if (strcmp(line, "QUIT") == 0) {
                send_str(client, "BYE\n");
                break;
            } else if (strcmp(line, "SHUTDOWN") == 0) {
                send_str(client, "BYE\n");
                running = 0;
                break;
            } else {
                send_str(client, "ERR\n");
            }
        }
        closesocket(client);
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
