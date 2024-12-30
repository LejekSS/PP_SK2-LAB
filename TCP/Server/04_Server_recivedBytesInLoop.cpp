#pragma once
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Serwer TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po nadejściu połączenia odbytuje o 10 bajtw danych, wypisuje je

int main(int argc, char **argv) {
    int servSock = socket_bind_listen(argc, argv, SOCK_STREAM);

    while (true) {
        int clientSock = accept(servSock, nullptr, nullptr);
        printf("Accepted a new connection\n");
        while (true) {
            char data[11]{};
            int len = read(clientSock, data, sizeof(data) - 1);
            if (len < 1)
                break;
            printf(" Received %2d bytes: |%s|\n", len, data);
        }
        close(clientSock);
        printf("Connection closed\n");
    }
}


int socket_bind_listen(int argc, char **argv, int sock_type) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    sockaddr_in localAddress{.sin_family = AF_INET,
                             .sin_port = htons(atoi(argv[1])),
                             .sin_addr = {htonl(INADDR_ANY)}};

    int servSock = socket(PF_INET, sock_type, 0);

    if (sock_type == SOCK_STREAM) {
        const int one = 1;
        setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }

    if (bind(servSock, (sockaddr *)&localAddress, sizeof(localAddress))) {
        perror("Bind failed!");
        exit(1);
    }

    if (sock_type == SOCK_STREAM)
        listen(servSock, 1);

    return servSock;
}
