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