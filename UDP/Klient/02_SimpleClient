#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// Prosty klient UDP, przyjmuje dwa arguemnty, (Adres IP i port);
// Wysyła stały ciąg znakow pod wskazany adres
// Odbiera ciąg znakow z wskazanego adresu; konczy działanie

int main(int argc, char ** argv) {
    if(argc!=3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    in_addr addr;
    if(!inet_aton(argv[1], &addr)){
        printf("Invalid argument: %s is not an IP address\nUsage: %s <ip> <port>\n", argv[1], argv[0]);
        return 1;
    }
    char * end;
    long port = strtol(argv[2], &end,10);
    if(*end != 0 || port < 1 || port > 65535){
        printf("Invalid argument: %s is not a port number\nUsage: %s <ip> <port>\n", argv[2], argv[0]);
        return 1;
    }
    
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd == -1){
        perror("socket failed");
        return 1;
    }

    sockaddr_in targetAddr {};
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_addr = addr;
    targetAddr.sin_port = htons((int)port);

    ssize_t ssize = sendto(fd, "Hello", 5, 0, (sockaddr*)&targetAddr, sizeof(targetAddr));
    if(ssize == -1){
        perror("sendto failed");
        close(fd);
        return 1;
    }

    char buffer[UINT16_MAX];
    ssize = read(fd, buffer, UINT16_MAX);
    if(ssize == -1){
        perror("read failed");
        close(fd);
        return 1;
    }

    write(1, buffer, ssize);
    close(fd);
    return 0;
}