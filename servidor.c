#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <process.h>  // _beginthreadex//

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall receber(void *socket_desc) {
    SOCKET sock = *(SOCKET*)socket_desc;
    char buffer[512];
    int bytes;

    while ((bytes = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0){
       buffer[bytes] = '\0';
       printf("\nCliente: %s\n", buffer);
       printf("Voce: ");
       fflush(stdout); 
    }
}


int main(){
    printf("Bem vindo ao RChat\n\n");
    printf("configurando seu server...");
    WSADATA wsadata;
    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    char buffer[512];
    char msg[512];

    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
    int clientAddrLen = sizeof(clientAddr);

    //inicializando as portas
    if (WSAStartup (MAKEWORD(2,2), &wsadata) != 0){
        printf("erro no start do wsa\n");
        return 1;
    }

    //criamos e validamos o socket para a conexao do servidor
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //af_inet para dizer q é ipv4, sock para tcp, e definir protocolo de conexao
    if (listen_socket == INVALID_SOCKET){
        printf("erro ao criar porta");
        WSACleanup(); //limpa porta (boa pratica)
        return 1;
    }

    serverAddr.sin_family =AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(7727);

    //agora vamos associar o socket a porta
    if (bind(listen_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        printf("erro no bind\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    if(listen(listen_socket, SOMAXCONN) == SOCKET_ERROR){
        printf("erro no listen\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    system("cls");
    char host[256];
    struct hostent *host_entry;
    char *ip;

    if (gethostname(host, sizeof(host)) == SOCKET_ERROR) {
        printf("Erro ao obter hostname\n");
    } else {
        host_entry = gethostbyname(host);
        if (host_entry == NULL) {
            printf("Erro ao obter IP\n");
        } else {
            ip = inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]);
            printf("Servidor aberto em %s:%d\n", ip, 7727);
        }
    }

    client_socket = accept(listen_socket, (SOCKADDR*)&clientAddr, &clientAddrLen);
    if (client_socket == SOCKET_ERROR){
        printf("erro no accept\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    printf("cliente conectado: %s\n", inet_ntoa(clientAddr.sin_addr));

    HANDLE thread;
    unsigned threadID;
    thread = (HANDLE)_beginthreadex(NULL, 0, receber, (void*)&client_socket, 0, &threadID);

    while (1){
        char msg[512];
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;
        send(client_socket, msg, strlen(msg), 0);
        if (strcmp(msg, "/exit") == 0) break;
    }
    

    // while (1){
    //     memset(buffer, 0, sizeof(buffer));
    //     int bytes = recv(client_socket, buffer, sizeof(buffer) -1, 0);

    //     if (bytes <= 0){
    //         printf("conexao encerrada\n");
    //         break;
    //     }

    //     buffer[bytes] = '\0';
    //     printf("cliente: %s\n", buffer);

    //     if (strcmp(buffer, "/exit") == 0) break;
    //     printf("voce: ");
    //     fgets(msg, sizeof(msg), stdin);
    //     msg[strcspn(msg, "\n")] = 0;
    //     send(client_socket, msg, (int)strlen(msg), 0);

    //     if (strcmp(msg, "/exit") == 0) break;
    // }
    
    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();






}