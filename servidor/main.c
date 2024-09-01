#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 1024

const char *vocales = "aeiou";
const char *consonantes = "bcdfghjklmnpqrstvwxyz";
const char *alfanumerico = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char *menu = "\nIndique su eleccion: \n 1. Generar Usuario \n 2. Generar Contrasena \n\n 0. Finalizar conexion \n";
const char *aux = " - Oprima Enter para Continuar - ";

void generarUsuario(int length, char *user) {
    if (length < 5 || length > 15) {
        strcpy(user, " - Longitud Invalida - ");
        printf(" - Longitud Invalida - \n");
        return;
    }

    const char *letraActual;
    int esVocal = rand() % 2; // 0 - 1

    for (int i = 0; i < length; i++) {
        if (esVocal) {
            letraActual = vocales;
        } else {
            letraActual = consonantes;
        }

        user[i] = letraActual[rand() % strlen(letraActual)];
        esVocal = !esVocal; // Alternar entre vocales y consonantes
    }
    user[length] = '\0';
    printf("Usuario: %s\n", user);
}

void generarContrasena(int length, char *password) {
    if (length < 8 || length >= 50) {
        strcpy(password, " - Longitud Invalida - ");
        printf(" - Longitud Invalida - \n");
        return;
    }

    for (int i = 0; i < length; i++) {
        password[i] = alfanumerico[rand() % strlen(alfanumerico)];
    }
    password[length] = '\0';
    printf("Contrasena: %s\n", password);
}

void controlCliente(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int recvSize;
    int longitud;
    int longRec;
    const char *error;
    const char *indicar;

    send(clientSocket, menu, strlen(menu), 0);

    while (1) {
        recvSize = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (recvSize > 0) {
            buffer[recvSize] = '\0';
            int eleccion = atoi(buffer);
            switch(eleccion){
            case 1:
                indicar = "\nIndique Longitud (valor entre 5 y 15)";
                send(clientSocket, indicar, strlen(indicar), 0);
                longRec = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
                buffer[longRec] = '\0';
                longitud = atoi(buffer);
                generarUsuario(longitud, response);
                send(clientSocket, aux, strlen(aux), 0);
                send(clientSocket, response, strlen(response), 0);
                break;
            case 2:
                indicar = "\nIndique Longitud (valor entre 8 y 50)";
                send(clientSocket, indicar, strlen(indicar), 0);
                longRec = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
                buffer[longRec] = '\0';
                longitud = atoi(buffer);
                generarContrasena(longitud, response);
                send(clientSocket, aux, strlen(aux), 0);
                send(clientSocket, response, strlen(response), 0);
                break;
            default:
                error = "- Comando no reconocido - \n";
                printf("%s", error);
                send(clientSocket, error, strlen(error), 0);
                break;
            }
            send(clientSocket, menu, strlen(menu), 0);

        } else if (recvSize == 0) {
            printf("Error al recibir datos. Error code: %d\n", WSAGetLastError());
            break;
        } else {
            printf("Conexion cerrada por el cliente\n");
            break;
        }
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    srand(time(NULL)); // Inicializar la semilla para la generación aleatoria

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al iniciar Winsock. Error code: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Error al crear el socket. Error code: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error al vincular. Error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Error al escuchar. Error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Servidor esperando en puerto %d...\n", PORT);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Error al aceptar. Error code: %d\n", WSAGetLastError());
            continue;
        }

        printf("Cliente conectado\n");
        controlCliente(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return EXIT_SUCCESS;
}
