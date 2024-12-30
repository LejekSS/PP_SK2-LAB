# PP_SK2-LAB
Zebrane kody z zadań wykonywanych w ramach SK2
```cpp
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "error.h"
#include <netdb.h>

int main(int argc, char ** argv){
    if(argc!=3) error(1,0,"Need 2 args");

    // Zmienna w której getaddrinfo ma zapisać gdzie są wyniki
    addrinfo *resolved;

    // Podpowiedzi – informacja dla getaddrinfo, że ma zwracać tylko:
    //↓ adresy IPv4     //↓ wyniki dotyczące TCP
    addrinfo hints {.ai_family=AF_INET, .ai_protocol=IPPROTO_TCP};

    // Wywołanie getaddrinfo dla hosta (ip) i usługi (portu) zapisanego jako tekst
    int res = getaddrinfo(argv[1], argv[2], &hints, &resolved);

    // getaddrinfo przy poprawnym wykonaniu zwraca 0, w przeciwnym razie **kod** błędu
    if(res)
        // kod błędu można przetłumaczyć na tekst funkcją gai_strerror
        error(1, 0, "getaddrinfo: %s", gai_strerror(res));

    // Poprawne wywołanie getaddrinfo może zwrócić pusty rezultat (tzn. nazwa istnieje, ale nie ma adresu)
    if(!resolved)
        error(1, 0, "getaddrinfo returned empty results");

    int sock;
    addrinfo *iterator = resolved;
    while(true){
        // Pola family, socktype i protocol struktury zwracanej przez getaddrinfo to wartości oczekiwane przez socket
        sock = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
        if(sock == -1) error(1, errno, "socket failed");

        // Pola addr i addrlen struktury zwracanej przez getaddrinfo to wartości oczekiwane przez connect
        res = connect(sock, iterator->ai_addr, iterator->ai_addrlen);
        if(!res)
            break;

        error(0, errno, "connect failed");
        close(sock);

        // jeśli nie powiodła się próba połączenia pod pierwszy podany adres, można sprawdzić czy nie zostało zwróconych więcej adresów
        iterator = iterator->ai_next;
        if(!iterator)
            if(sock == -1) error(1, errno, "connect to all addresses returned by getaddrinfo failed");
    }

    // należy pamiętać o zwolnieniu pamięci zaalokowanej przez getaddrinfo
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

```