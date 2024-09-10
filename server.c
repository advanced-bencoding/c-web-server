#include <stdio.h>
#include <winsock2.h>

int main()
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Could not initialize winsock. Error Code: %d.", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized successfully.\n");

    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Failed to create socket. Error Code: %d", WSAGetLastError());
        return 1;
    }
    printf("Socket created successfully.\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Could not bind server socket. Error Code: %d", WSAGetLastError());
        return 1;
    }
    printf("Server socket binded successfully.\n");

    listen(server_socket, 3);

    SOCKET client_socket;
    struct sockaddr_in client;
    int client_length = sizeof(struct sockaddr_in);

    // keep alive to listen to requests
    while (TRUE)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client, &client_length);
        if (client_socket == INVALID_SOCKET)
        {
            printf("Could not accept incoming connection request. Error Code: %d", WSAGetLastError());
            return 1;
        }
        printf("Connection accepted.\n");

        int client_message_buffer_space = 2000;
        char client_message[client_message_buffer_space];
        int recv_size = recv(client_socket, client_message, client_message_buffer_space, 0);
        if (recv_size == SOCKET_ERROR)
        {
            printf("Could not recieve message. Error Code: %d", WSAGetLastError());
            return 1;
        }
        client_message[recv_size] = '\0';
        printf("Recieved message from client: %s\n", client_message);

        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_socket, response, strlen(response), 0);
        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
};