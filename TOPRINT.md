### TCP CLIENT

'''cpp
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Prosty klient TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Łączy się do serwera; Odbiera wiadomość; Zamyka połączenie

int main(int argc, char ** argv) {
    if(argc!=3){
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    in_addr addr;
    if( ! inet_aton(argv[1], &addr)){
        printf("Invalid argument: %s is not an IP address\nUsage: %s <ip> <port>\n", argv[1], argv[0]);
        return 1;
    }

    char * end;
    long port = strtol(argv[2], &end,10);
    if(*end != 0 || port < 1 || port > 65535){
        printf("Invalid argument: %s is not a port number\nUsage: %s <ip> <port>\n", argv[2], argv[0]);
        return 1;
    }

    sockaddr_in targetAddr {};
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_addr = addr;
    targetAddr.sin_port = htons((uint16_t) port);

    int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd == -1){
        perror("socket failed");
        return 1;
    }

    int fail = connect(fd, (sockaddr*) &targetAddr, sizeof(targetAddr));
    if(fail){
        perror("connect failed");
        return 1;
    }

    char buffer[255];
    ssize_t readSize;
    while(1) {
        readSize = read(fd, buffer, 255);
        if(readSize == 0)
            break;
        if(readSize == -1){
            perror("read failed");
            close(fd);
            return 1;
        }
        write(1, buffer, readSize);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}
'''

'''cpp
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "error.h"
#include <netdb.h>

// Prosty klient TCP - uzywa getaddrinfo, przyjmuje dwa arguemnty, (Adres IP i port);
// Łączy się do serwera; Odbiera wiadomość; Zamyka połączenie

int main(int argc, char ** argv){
    if(argc!=3) error(1,0,"Need 2 args");

  
    addrinfo *resolved;

    addrinfo hints {.ai_family=AF_INET, .ai_protocol=IPPROTO_TCP};


    int res = getaddrinfo(argv[1], argv[2], &hints, &resolved);

    if(res)

        error(1, 0, "getaddrinfo: %s", gai_strerror(res));

    if(!resolved)
        error(1, 0, "getaddrinfo returned empty results");


    int sock;
    addrinfo *iterator = resolved;
    while(true){
        sock = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
        if(sock == -1) error(1, errno, "socket failed”);

        res = connect(sock, iterator->ai_addr, iterator->ai_addrlen);
        if(!res)
            break;

        error(0, errno, "connect failed");
        close(sock);

        iterator = iterator->ai_next;
        if(!iterator)
            if(sock == -1) error(1, errno, "connect to all addresses returned by getaddrinfo failed");
    }
    freeaddrinfo(resolved);


    char buffer[255];
    ssize_t readSize;
    while(1) {
        readSize = read(sock, buffer, 255);
        if(readSize ==  0) break;
        if(readSize == -1)error(1,errno,"read failed");
        write(1, buffer, readSize);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
'''

'''cpp
#pragma once
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>


// Klient TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po połączeniu w pętli wysyłą dane, wypisuje numer i ilośc wysłanych danych

int main(int argc, char **argv) {
    int sock = getaddrinfo_socket_connect(argc, argv);

    char data[10];
    int sent = 1;
    while (true) {
        write(sock, data, sizeof(data));
        printf("   Sent %d\r", 10 * sent++);
        fflush(stdout);
    }
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
'''

'''cpp
#pragma once
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>


// Klient TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po połączeniu wysyła cały alfabet litera po literze

int main(int argc, char **argv) {
    int sock = getaddrinfo_socket_connect(argc, argv);

    for (char letter = 'a'; letter <= 'z'; ++letter)
        write(sock, &letter, 1);

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
'''

'''cpp
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
'''

'''cpp
#pragma once
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>


// Klient TCP, przyjmuje dwa arguemnty, (Adres IP i port);
// Po połączeniu ustawia tryb nieblokujĺący;
// w pętli wysyłą dane, wypisuje numer i ilośc wysłanych danych
// jeśli wysłał mniej danych niz chcial, program się kończy

int main(int argc, char **argv) {
    int sock = getaddrinfo_socket_connect(argc, argv);

    fcntl(sock, F_SETFL, O_NONBLOCK);

    char data[10];
    int sent = 0;
    while (true) {
        int written = write(sock, data, sizeof(data));
        if (written == 10) {
            printf("   Sent %d\r", 10 * ++sent);
            fflush(stdout);
        } else if (written == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                printf("\nNext write would block\n");
            else
                printf("\nWrite went wrong\n");
            break;
        } else {
            printf("\nSent only %d out of 10 bytes\n", written);
            break;
        }
    }
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
'''

### TCP SERVER