#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Prosty serwer UDP, przyjmuje dwa arguemnty, (Adres IP i port);
// Nasłuchuje i odbiera ciąg znakow; odsyla je pod adres z ktorego nadeszly zamieniajc literty na duze

int main(int argc, char ** argv) {
    if(argc!=2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    char *end;
    long port = strtol(argv[1], &end,10);
    if(*end != 0 || port < 1 || port > 65535){
        printf("Invalid argument: %s is not a port number\nUsage: %s <ip> <port>\n", argv[2], argv[0]);
        return 1;
    }

    sockaddr_in myAddr {};
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons((int)port);

    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("socket failed");
        return 1;
    }

    auto fail = bind(fd, (sockaddr*) &myAddr, sizeof(myAddr));
    if(fail){
        perror("bind failed");
        return 1;
    }
    char buffer[UINT16_MAX];
    sockaddr_in remoteAddr;
    socklen_t remoteAddrSize = sizeof(remoteAddr);

    ssize_t ssize = recvfrom(fd, buffer, UINT16_MAX, 0, (sockaddr*)&remoteAddr, &remoteAddrSize);
    if(ssize == -1){
        perror("recvfrom failed");
        close(fd);
        return 1;
    }

    for(char *it=buffer; (*it=toupper(*it)); ++it);

    ssize = sendto(fd, buffer, ssize, 0, (sockaddr*)&remoteAddr, remoteAddrSize);
    if(ssize == -1){
        perror("sendto failed");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}