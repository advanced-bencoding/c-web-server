#include <stdio.h>
#include <winsock2.h>

int initializeWSA(WSADATA *wsa)
{
    if (WSAStartup(MAKEWORD(2, 2), wsa) != 0)
    {
        printf("Could not initialize winsock. Error Code: %d.", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized successfully.\n");
    return 0;
}

void runCleanup(SOCKET server_socket)
{
    WSACleanup();
    if (server_socket != INVALID_SOCKET)
    {
        closesocket(server_socket);
    }
}

SOCKET createServerSocket()
{
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Failed to create socket. Error Code: %d", WSAGetLastError());
        runCleanup(server_socket);
    }
    else
    {
        printf("Socket created successfully.\n");
    }
    return server_socket;
}

int bindServerSocket(SOCKET server_socket)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Could not bind server socket. Error Code: %d", WSAGetLastError());
        runCleanup(server_socket);
        return 1;
    }
    printf("Server socket binded successfully.\n");
    return 0;
}

SOCKET acceptIncomingRequest(SOCKET server_socket)
{
    SOCKET client_socket;
    struct sockaddr_in client;
    int client_length = sizeof(struct sockaddr_in);

    client_socket = accept(server_socket, (struct sockaddr *)&client, &client_length);
    if (client_socket == INVALID_SOCKET)
    {
        printf("Could not accept incoming connection request. Error Code: %d", WSAGetLastError());
    }
    else
    {
        printf("Connection accepted.\n");
    }
    return client_socket;
}

int receiveMessage(SOCKET client_socket, int max_buffer_length, char *buffer)
{
    int recv_size = recv(client_socket, buffer, max_buffer_length - 1, 0);
    if (recv_size == SOCKET_ERROR)
    {
        printf("Could not recieve message. Error Code: %d", WSAGetLastError());
        return 1;
    }
    buffer[recv_size] = '\0';
    printf("Recieved message from client: %s\n", buffer);
    return 0;
}

int main()
{
    WSADATA wsa;
    if (initializeWSA(&wsa) != 0)
    {
        return 1;
    }

    SOCKET server_socket = createServerSocket();
    if (server_socket == INVALID_SOCKET)
    {
        return 1;
    }

    if (bindServerSocket(server_socket) != 0)
    {
        return 1;
    }

    listen(server_socket, 3);

    SOCKET client_socket;
    // keep alive to listen to requests
    while (TRUE)
    {
        client_socket = acceptIncomingRequest(server_socket);

        int max_buffer_length = 2000;
        char client_message[max_buffer_length];

        if (receiveMessage(client_socket, max_buffer_length, client_message) != 0)
        {
            // todo: return err response
            closesocket(client_socket);
            continue;
        }

        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_socket, response, strlen(response), 0);
        closesocket(client_socket);
    }

    runCleanup(server_socket);

    return 0;
};