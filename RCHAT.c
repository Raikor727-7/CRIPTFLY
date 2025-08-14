#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall receber(void *socket_desc){
    SOCKET sock = *(SOCKET*)socket_desc;
    char buffer[512];
    int bytes;

    while((bytes = recv(sock, buffer, sizeof(buffer) -1, 0)) > 0){
        buffer[bytes] = '\0';
        printf("servidor: %s\n", buffer);
        printf("Voce: ");
        fflush(stdout);
    }
    return 0;
}

void modo_Cliente(){
    WSADATA wsadata;
    SOCKET client_socket;
    SOCKET connect_socket;
    struct sockaddr_in serverAddr;
    char ip[32];
    int port;

    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        printf("erro ao iniciar WSA\n");
        return;
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
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    //conexao com o servidor
    if (connect(client_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        printf("falha na conexao\n");
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    printf("conexao realizada!!\n");

    HANDLE thread;
    unsigned threadID;
    thread = (HANDLE)_beginthreadex(NULL, 0, receber, (void*)&client_socket, 0, &threadID);

    while(1){
        char mensagem[512];
        fgets(mensagem, sizeof(mensagem), stdin);

        mensagem[strcspn(mensagem, "\n")] = 0;
        send(client_socket, mensagem, strlen(mensagem), 0);
        if(strcmp(mensagem, "/exit") == 0) break; 
    }

    closesocket(client_socket);
    WSACleanup();
}

void modo_Servidor(){
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
        return;
    }

    //criamos e validamos o socket para a conexao do servidor
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //af_inet para dizer q é ipv4, sock para tcp, e definir protocolo de conexao
    if (listen_socket == INVALID_SOCKET){
        printf("erro ao criar porta");
        WSACleanup(); //limpa porta (boa pratica)
        return;
    }

    serverAddr.sin_family =AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(7727);

    //agora vamos associar o socket a porta
    if (bind(listen_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        printf("erro no bind\n");
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    if(listen(listen_socket, SOMAXCONN) == SOCKET_ERROR){
        printf("erro no listen\n");
        closesocket(listen_socket);
        WSACleanup();
        return;
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
        return;
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
    
    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();

}

int main(){
    while(1){
        int opcao;
        printf("Escolha o modo:\n1- Servidor\n2- Cliente\nOpcao: ");
        scanf("%d", &opcao);
        getchar();

        if(opcao == 1){
            modo_Servidor();
        }
        else if(opcao == 2){
            modo_Cliente();
        }
        else if(opcao == 99){
            break;
        }
        else{
            printf("opcao invalida\n");
            Sleep(2000);
            system("cls");
        }
    }
}
