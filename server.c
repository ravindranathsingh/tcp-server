#include <stdio.h>
#include <winsock2.h>
#include <stdbool.h>

#define PORT_NUMBER 8080
#define SA struct sockaddr

void chat(SOCKET connection_socket) {
    char message_buffer[1024];
    int bytes_read, bytes_written;

    for (;;) {
        bytes_read = recv(connection_socket, message_buffer, sizeof(message_buffer), 0);
        if (bytes_read <= 0) {
            printf("Client disconnected or error reading message.\n");
            break;
        }
        message_buffer[bytes_read] = '\0'; // Null-terminate the received message
        printf("Client says: %s\n", message_buffer);

        printf("Enter your message: ");
        fgets(message_buffer, sizeof(message_buffer), stdin);
        if (strcmp(message_buffer, "exit\n") == 0) {
            printf("Server Exit...\n");
            break;
        }

        bytes_written = send(connection_socket, message_buffer, strlen(message_buffer), 0);
        if (bytes_written <= 0) {
            printf("Error writing to socket.\n");
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    SOCKET server_socket, connection_socket;
    struct sockaddr_in server_address, client_address;

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Error creating socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT_NUMBER);
    if (bind(server_socket, (SA*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    int client_address_length = sizeof(client_address);
    connection_socket = accept(server_socket, (SA*)&client_address, &client_address_length);
    if (connection_socket == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    chat(connection_socket);

    closesocket(connection_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
