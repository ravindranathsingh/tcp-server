#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT_NUMBER 8080
#define BUFFER_SIZE 1024

void chat(int connection_socket) {
    char msg_buffer[BUFFER_SIZE];
    int index;

    for (;;) {
        memset(msg_buffer, 0, sizeof(msg_buffer));
        printf("Enter the string : ");
        index = 0;
        while ((msg_buffer[index++] = getchar()) != '\n')
            ;

        if (send(connection_socket, msg_buffer, strlen(msg_buffer), 0) == SOCKET_ERROR) {
            printf("Error sending data to server\n");
            break;
        }

        memset(msg_buffer, 0, sizeof(msg_buffer));
        if (recv(connection_socket, msg_buffer, sizeof(msg_buffer), 0) == SOCKET_ERROR) {
            printf("Error receiving data from server\n");
            break;
        }

        printf("From Server : %s", msg_buffer);

        if ((strncmp(msg_buffer, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    int iResult;
    int connection_socket;
    struct sockaddr_in server_address;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connection_socket == INVALID_SOCKET) {
        printf("socket creation failed...\n");
        WSACleanup();
        return 1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT_NUMBER);

    if (connect(connection_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        printf("connection with the server failed...\n");
        closesocket(connection_socket);
        WSACleanup();
        return 1;
    }

    chat(connection_socket);

    closesocket(connection_socket);
    WSACleanup();

    return 0;
}
