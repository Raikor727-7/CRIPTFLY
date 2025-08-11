#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main(){
    WSADATA wsadata;
    SOCKET client_socket;
    SOCKET connect_socket;
    struct sockaddr_in serverAddr;
    char ip[32];
    int port;

    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        printf("erro ao iniciar WSA\n");
        return 1;
    }

    printf("qual ip do servidor: ");
    scanf("%s", ip);
    printf("digite a porta: ");
    scanf("%d", &port);

    //criaremos o socket
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET){
        printf("erro ao criar socket\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    //conexao com o servidor
    if (connect(client_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        printf("falha na conexao\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("conexao realizada!!\n");

    //troca de mensagens
    char mensagem[1024];
    char buffer[1024];
    printf("digite uma mensagem (/exit para sair): ");
    getchar(); //limpeza de buffer


    while (1){
        //enviar mensagens
        printf("voce: ");
        fgets(mensagem, sizeof(mensagem), stdin);

        mensagem[strcspn(mensagem, "\n")] = 0;
        send(client_socket, mensagem, strlen(mensagem), 0);

        if (strcmp(mensagem, "/exit") == 0) break;

        //receber mensagens
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer) -1, 0);
        if (bytes <= 0){
            printf("conexao encerrada\n");
            break;
        }

        // buffer[bytes] = '\0';
        printf("servidor: %s\n", buffer);

        if(strcmp(buffer, "/exit") == 0) break;
        
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}