#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1" // Dirección IP del servidor (localhost)
#define PORT 8080
#define BUFFER_SIZE 1024


int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char sendBuffer[BUFFER_SIZE];
    int recvSize;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al iniciar Winsock. Error Code: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Crear el socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Error al crear el socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Configurar la dirección del servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error al conectar al servidor. Error Code: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Conectado al servidor\n");

    // Recepcion del menu inicial
    int menu = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    buffer[menu] = '\0';
    printf("%s\n", buffer);


    while (1) {
        // Leer mensaje del usuario
        printf("Ingrese mensaje para enviar: ");
        fgets(sendBuffer, BUFFER_SIZE, stdin);
        sendBuffer[strcspn(sendBuffer, "\n")] = '\0'; // Eliminar el salto de línea

        // Enviar mensaje al servidor
        if (send(clientSocket, sendBuffer, strlen(sendBuffer), 0) == SOCKET_ERROR) {
            printf("Error al enviar el mensaje. Error Code: %d\n", WSAGetLastError());
            break;
        }

        // Salir si el usuario eligio "0"
        if (strcmp(sendBuffer, "0") == 0) {
            break;
        }

        // Recibir respuesta del servidor
        recvSize = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (recvSize > 0) {
            buffer[recvSize] = '\0';
            printf("\n%s\n", buffer);
        } else if (recvSize == 0) {
            printf("Conexión cerrada por el servidor\n");
            break;
        } else {
            printf("Error al recibir el mensaje. Error code: %d\n", WSAGetLastError());
            break;
        }
    }

    // Cerrar el socket
    closesocket(clientSocket);

    // Limpiar Winsock
    WSACleanup();

    return EXIT_SUCCESS;
}
