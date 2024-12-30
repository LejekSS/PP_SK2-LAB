#pragma once
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>


// Klient UDP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po połączeniu wysyła cały alfabet litera po literze

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    addrinfo *resolved, hints = {.ai_family = AF_INET, .ai_protocol = IPPROTO_UDP};
    if (int err = getaddrinfo(argv[1], argv[2], &hints, &resolved)) {
        fprintf(stderr, "Resolving address failed: %s\n", gai_strerror(err));
        return 1;
    }

    int sock = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);

    for (char letter = 'a'; letter <= 'z'; ++letter)
        sendto(sock, &letter, 1, 0, resolved->ai_addr, resolved->ai_addrlen);

    freeaddrinfo(resolved);
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

