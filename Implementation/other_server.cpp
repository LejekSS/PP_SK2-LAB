#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void startTCPServer(uint16_t port) {
    sockaddr_in myAddr {};
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port);
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("TCP socket failed");
        return;
    }
    constexpr const int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (bind(fd, (sockaddr*)&myAddr, sizeof(myAddr)) == -1) {
        perror("TCP bind failed");
        close(fd);
        return;
    }
    if (listen(fd, 1) == -1) {
        perror("TCP listen failed");
        close(fd);
        return;
    }
    printf("TCP server listening on port %u\n", port);
    while (true) {
        sockaddr_in clientAddr {};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(fd, (sockaddr*)&clientAddr, &clientLen);
        if (clientFd == -1) {
            perror("TCP accept failed");
            continue;
        }
        printf("TCP connection from %s:%hu\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        const char* message = "TCP|2137";
        if (write(clientFd, message, strlen(message)) != (ssize_t)strlen(message)) {
            perror("TCP write failed");
        }
        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }
    close(fd);
}
void startUDPServer(uint16_t port) {
    sockaddr_in myAddr {};
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port);
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("UDP socket failed");
        return;
    }
    if (bind(fd, (sockaddr*)&myAddr, sizeof(myAddr)) == -1) {
        perror("UDP bind failed");
        close(fd);
        return;
    }
    printf("UDP server listening on port %u\n", port);
    while (true) {
        sockaddr_in clientAddr {};
        socklen_t clientLen = sizeof(clientAddr);
        char buffer[1024] = {};
        ssize_t received = recvfrom(fd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientLen);
        if (received == -1) {
            perror("UDP recvfrom failed");
            continue;
        }
        printf("UDP message from %s:%hu\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        const char* message = "TCP|2137";
        if (sendto(fd, message, strlen(message), 0, (sockaddr*)&clientAddr, clientLen) == -1) {
            perror("UDP sendto failed");
        }
    }
    close(fd);
}
int main() {
    printf("Select server type (1 for TCP, 2 for UDP): ");
    int choice;
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }
    printf("Enter port: ");
    uint16_t port;
    if (scanf("%hu", &port) != 1 || port < 1 || port > 65535) {
        printf("Invalid port. Exiting.\n");
        return 1;
    }
    if (choice == 1) {
        startTCPServer(port);
    } else {
        startUDPServer(port);
    }
    return 0;
}