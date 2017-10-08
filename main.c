#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAX_NUM_CONNECTIONS 1

int main(int argc, char *argv[]) {

    FILE *outputFile;
    FILE *requestedFile;
    int serverPort, bufferSize;
    int serverSocket, clientSocket, bindReturn, clientLength;
    struct sockaddr_in serverAddress, clienteAddress;
    int n;
    struct timeval start, end;

    //Server port for connection.
    serverPort = atoi(argv[1]);

    //Buffer size.
    bufferSize = atoi(argv[2]);

    char buffer[bufferSize];
    
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
    serverAddress.sin_port = htons(serverPort);

    bindReturn = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    if (bindReturn < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(serverSocket, MAX_NUM_CONNECTIONS); //Inicialmente setado para receber apenas 1 conexao
    clientLength = sizeof(clienteAddress);

    while (1) {

        /* Accept actual connection from the client */
        clientSocket = accept(serverSocket, (struct sockaddr *) &clienteAddress, &clientLength);

        if (clientSocket < 0) {
            perror("ERROR on accept");
            exit(1);
        }


        gettimeofday(&start, NULL);

        /* If connection is established then start communicating */
        bzero(buffer, bufferSize);
        n = read(clientSocket, buffer, bufferSize);

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        strtok(buffer, "\n");

        requestedFile = fopen(buffer, "r");

        if (requestedFile == NULL) {
            n = write(clientSocket, "ERROR: Requested file not found.", 32);

            if (n < 0) {
                perror("ERROR: Falha no envio da mensagem.");
                exit(1);
            }

        } else {

            while ( fgets(buffer, bufferSize, requestedFile) != NULL ) {

                /* Write a response to the client */
                n = write(clientSocket, buffer, bufferSize);

                if (n < 0) {
                    perror("ERROR: Falha no envio da mensagem.");
                    exit(1);
                }
            }

            fclose(requestedFile);

            close(clientSocket);
        }

        gettimeofday(&end, NULL);


        outputFile = fopen("result.txt", "a");

//        printf("TIME: %ld\n", ( (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec) ) );

        fprintf(outputFile, "%d,", bufferSize );
        fprintf(outputFile, "%ld\n",( (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec) ) );

        fclose(outputFile);

    }

}