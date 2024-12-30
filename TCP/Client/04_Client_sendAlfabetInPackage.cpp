#pragma once
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <random>
std::default_random_engine gen(std::random_device{}());



// Klient TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po połączeniu wysyła cały alfabet w pakietach po klika liter

int main(int argc, char **argv) {
    int sock = getaddrinfo_socket_connect(argc, argv);

    std::uniform_int_distribution<int> dist(7, 15);

    for (int i = 0; i < 20; i++) {
        size_t msgLen = dist(gen);
        char buf[msgLen];
        int numLen = sprintf(buf, "%d", i);
        memset(buf + numLen, '.', msgLen - numLen);

        write(sock, buf, msgLen);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}



int getaddrinfo_socket_connect(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *resolved;
    if (int err = getaddrinfo(argv[1], argv[2], &hints, &resolved)) {
        fprintf(stderr, "Resolving address failed: %s\n", gai_strerror(err));
        exit(1);
    }

    int sock = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);
    if (connect(sock, resolved->ai_addr, resolved->ai_addrlen)) {
        perror("Failed to connect");
        exit(1);
    }
    freeaddrinfo(resolved);

    return sock;
}

