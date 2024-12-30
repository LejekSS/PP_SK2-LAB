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