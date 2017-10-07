#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>
#include <arpa/inet.h>

#define PORT_NUMBER 10001
#define MAX_NUM_CONNECTIONS 1
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {


    FILE *requestedFile;
    int serverSocket, clientSocket, bindReturn, clientLength;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddress, clienteAddress;
    int n;

    //Criacao do socket para o servidor
    //Params: AF_INET - IPv4 protocols ; SOCK_STREAM - Stream socket ; IPPROTO_TCP - TCP transport protocol
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    //Inicializacao do socket
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;
    serverAddress.sin_port = htons(PORT_NUMBER);

    /* Now bind the host address using bind() call.*/
    bindReturn = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    printf("Bind");
    printf(" %d\n", bindReturn);

    if (bindReturn < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

    listen(serverSocket, MAX_NUM_CONNECTIONS); //Inicialmente setado para receber apenas 1 conexao
    clientLength = sizeof(clienteAddress);

    printf("Client Length");
    printf(" %d\n", clientLength);

    while (1) {

        /* Accept actual connection from the client */
        clientSocket = accept(serverSocket, (struct sockaddr *) &clienteAddress, &clientLength);

        if (clientSocket < 0) {
            perror("ERROR on accept");
            exit(1);
        }


        /* If connection is established then start communicating */
        bzero(buffer, BUFFER_SIZE);
        n = read(clientSocket, buffer, BUFFER_SIZE);

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        printf("File name: %s\n", buffer);

        strtok(buffer, "\n");

        requestedFile = fopen(buffer, "r");

        if (requestedFile == NULL) {
            n = write(clientSocket, "ERROR: Requested file not found.", 32);

            if (n < 0) {
                perror("ERROR: Falha no envio da mensagem.");
                exit(1);
            }

        } else {

            while ( fgets(buffer, BUFFER_SIZE, requestedFile) != NULL ) {

                printf("Pacote enviado: %s\n", buffer);
                /* Write a response to the client */
                n = write(clientSocket, buffer, BUFFER_SIZE);

                if (n < 0) {
                    perror("ERROR: Falha no envio da mensagem.");
                    exit(1);
                }
            }

            printf("Fim de envio de arquivo.");

            fclose(requestedFile);

            close(clientSocket);

            printf("Socket fechado");
        }

    }

}