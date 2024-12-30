#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <set>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Prosty serwer UDP, przyjmuje dwa arguemnty, (Adres IP i port);
// Nasłuchuje i odbiera ciąg znakow;
// Rozsyła odebrany ciąg znakw do wszystkich klientw, ktorzy kiedykolwiek wyslali dane do serwera

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

    std::set<sockaddr_in> remotes;

    while(1){
        char buffer[UINT16_MAX];
        sockaddr_in remoteAddr{};
        socklen_t remoteAddrSize = sizeof(remoteAddr);

        ssize_t msgsize = recvfrom(fd, buffer, UINT16_MAX, 0, (sockaddr*)&remoteAddr, &remoteAddrSize);
        if(msgsize == -1){
            perror("recvfrom failed");
            close(fd);
            return 1;
        }

        remotes.insert(remoteAddr);

        for(const sockaddr_in & remote : remotes){
            ssize_t _ = sendto(fd, buffer, msgsize, 0, (const sockaddr*)&remote, sizeof(remote));
            if(_ == -1){
                perror("sendto failed");
                close(fd);
                return 1;
            }
        }
    }
}

bool operator<(const sockaddr_in& l, const sockaddr_in& r){
    uint64_t left  = ((uint64_t)l.sin_addr.s_addr << 32) | l.sin_port;
    uint64_t right = ((uint64_t)r.sin_addr.s_addr << 32) | r.sin_port;
    return left < right;
}