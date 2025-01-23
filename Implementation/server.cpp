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
        const char* message = "Siema";
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
        const char* message = "Siema";
        if (sendto(fd, message, strlen(message), 0, (sockaddr*)&clientAddr, clientLen) == -1) {
            perror("UDP sendto failed");
        }
    }
    close(fd);
}
void handleServerConfiguration(const char* config) {
    char type[4];
    uint16_t port;
    if (sscanf(config, "%3s|%hu", type, &port) != 2) {
        printf("Invalid configuration: %s\n", config);
        return;
    }
    if (strcmp(type, "TCP") == 0) {
        startTCPServer(port);
    } else if (strcmp(type, "UDP") == 0) {
        startUDPServer(port);
    } else {
        printf("Unknown server type: %s\n", type);
    }
}
int main() {
    printf("Enter server IP: ");
    char ip[16];
    if (scanf("%15s", ip) != 1) {
        printf("Invalid IP input. Exiting.\n");
        return 1;
    }
    printf("Enter server port: ");
    uint16_t serverPort;
    if (scanf("%hu", &serverPort) != 1 || serverPort < 1 || serverPort > 65535) {
        printf("Invalid port input. Exiting.\n");
        return 1;
    }
    printf("Select protocol (1 for TCP, 2 for UDP): ");
    int protocolChoice;
    if (scanf("%d", &protocolChoice) != 1 || (protocolChoice != 1 && protocolChoice != 2)) {
        printf("Invalid protocol choice. Exiting.\n");
        return 1;
    }
    int fd = socket(PF_INET, (protocolChoice == 1 ? SOCK_STREAM : SOCK_DGRAM), 0);
    if (fd == -1) {
        perror("Client socket failed");
        return 1;
    }
    sockaddr_in serverAddr {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (!inet_aton(ip, &serverAddr.sin_addr)) {
        printf("Invalid IP address. Exiting.\n");
        close(fd);
        return 1;
    }
    if (protocolChoice == 1) {
        if (connect(fd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("TCP connect failed");
            close(fd);
            return 1;
        }
        char buffer[256] = {};
        ssize_t received = read(fd, buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';
            printf("Received configuration: %s\n", buffer);
            handleServerConfiguration(buffer);
        } else {
            perror("TCP read failed");
        }
    } else {
        const char* message = "Request configuration";
        if (sendto(fd, message, strlen(message), 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("UDP sendto failed");
            close(fd);
            return 1;
        }
        char buffer[256] = {};
        socklen_t serverLen = sizeof(serverAddr);
        ssize_t received = recvfrom(fd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverAddr, &serverLen);
        if (received > 0) {
            buffer[received] = '\0';
            printf("Received configuration: %s\n", buffer);
            handleServerConfiguration(buffer);
        } else {
            perror("UDP recvfrom failed");
        }
    }
    close(fd);
    return 0;
}