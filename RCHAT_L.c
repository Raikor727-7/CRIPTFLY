#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

void *receber(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[512];
    int bytes;

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("servidor: %s\n", buffer);
        if (strcmp(buffer, "/exit") == 0) {
            printf("\033[31mconexao encerrada\n\033[0m");
            break;
        }
        printf("Voce: \n");
        fflush(stdout);
    }
    return NULL;
}

void modo_Cliente() {
    int client_socket;
    struct sockaddr_in serverAddr;
    char ip[32];
    int port;

    printf("qual ip do servidor: ");
    scanf("%s", ip);
    printf("digite a porta: ");
    scanf("%d", &port);
    getchar(); // limpar buffer

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("erro ao criar socket");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    struct hostent *he = gethostbyname(ip);
    if (he == NULL) {
        printf("Nao foi possivel resolver o host\n");
        close(client_socket);
        return;
    }
    memcpy(&serverAddr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(client_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("falha na conexao");
        close(client_socket);
        return;
    }

    printf("conexao realizada!!\n");

    pthread_t thread;
    pthread_create(&thread, NULL, receber, (void*)&client_socket);

    while (1) {
        char mensagem[512];
        fgets(mensagem, sizeof(mensagem), stdin);
        mensagem[strcspn(mensagem, "\n")] = 0;
        send(client_socket, mensagem, strlen(mensagem), 0);
        if (strcmp(mensagem, "/exit") == 0) break;
    }

    close(client_socket);
}

void modo_Servidor() {
    printf("Bem vindo ao RChat\n\n");
    printf("configurando seu server...\n");

    int listen_socket, client_socket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
        perror("erro ao criar socket");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(7727);

    if (bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("erro no bind");
        close(listen_socket);
        return;
    }

    if (listen(listen_socket, SOMAXCONN) < 0) {
        perror("erro no listen");
        close(listen_socket);
        return;
    }

    system("clear");
    char host[256];
    struct hostent *host_entry;
    char *ip;

    if (gethostname(host, sizeof(host)) == -1) {
        perror("Erro ao obter hostname");
    } else {
        host_entry = gethostbyname(host);
        if (host_entry == NULL) {
            printf("Erro ao obter IP\n");
        } else {
            ip = inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]);
            printf("Servidor aberto em %s:%d\n", ip, 7727);
        }
    }

    client_socket = accept(listen_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (client_socket < 0) {
        perror("erro no accept");
        close(listen_socket);
        return;
    }

    printf("cliente conectado: %s\n", inet_ntoa(clientAddr.sin_addr));

    pthread_t thread;
    pthread_create(&thread, NULL, receber, (void*)&client_socket);

    while (1) {
        char msg[512];
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;
        send(client_socket, msg, strlen(msg), 0);
        if (strcmp(msg, "/exit") == 0) break;
    }

    close(client_socket);
    close(listen_socket);
}

int main() {
    while (1) {
        int opcao;
        printf("Escolha o modo:\n1- Servidor\n2- Cliente\n99- Sair\nOpcao: ");
        scanf("%d", &opcao);
        getchar();

        if (opcao == 1) {
            modo_Servidor();
            printf("conexao encerrada...");
            usleep(2000 * 1000);
            system("clear");
        } else if (opcao == 2) {
            modo_Cliente();
            printf("conexao encerrada...");
            usleep(2000 * 1000);
            system("clear");
        } else if (opcao == 99) {
            break;
        } else {
            printf("opcao invalida\n");
            usleep(2000 * 1000);
            system("clear");
        }
    }
    return 0;
}
