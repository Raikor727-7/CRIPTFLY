// rchat.c - versão corrigida para Linux (pthread)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>

#define DEFAULT_PORT 7727

void *receber(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char buffer[512];
    ssize_t bytes;

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("\nremoto: %s\n", buffer);
        if (strcmp(buffer, "/exit") == 0) {
            printf("\033[31mconexao encerrada\n\033[0m");
            break;
        }
        printf("Voce: ");
        fflush(stdout);
    }

    if (bytes == 0) {
        printf("\nconexao fechada pelo par\n");
    } else if (bytes < 0) {
        perror("recv");
    }

    close(sock);
    return NULL;
}

void modo_Cliente(void) {
    int client_socket;
    struct sockaddr_in serverAddr;
    char ip[64];
    int port;

    printf("qual ip do servidor: ");
    if (scanf("%63s", ip) != 1) return;
    printf("digite a porta: ");
    if (scanf("%d", &port) != 1) return;
    getchar(); // limpar '\n' do buffer

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        // tenta resolver como hostname
        struct addrinfo hints, *res = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        if (getaddrinfo(ip, NULL, &hints, &res) == 0 && res) {
            struct sockaddr_in *sa = (struct sockaddr_in *)res->ai_addr;
            serverAddr.sin_addr = sa->sin_addr;
            freeaddrinfo(res);
        } else {
            fprintf(stderr, "IP invalido e nao consegui resolver hostname\n");
            close(client_socket);
            return;
        }
    }

    if (connect(client_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        close(client_socket);
        return;
    }

    printf("conexao realizada!!\n");

    int *pclient = malloc(sizeof(int));
    if (!pclient) { close(client_socket); return; }
    *pclient = client_socket;

    pthread_t tid;
    if (pthread_create(&tid, NULL, receber, pclient) != 0) {
        perror("pthread_create");
        free(pclient);
        close(client_socket);
        return;
    }
    pthread_detach(tid);

    while (1) {
        char mensagem[512];
        if (fgets(mensagem, sizeof(mensagem), stdin) == NULL) break;
        mensagem[strcspn(mensagem, "\n")] = 0;
        if (send(client_socket, mensagem, strlen(mensagem), 0) < 0) {
            perror("send");
            break;
        }
        if (strcmp(mensagem, "/exit") == 0) break;
    }

    close(client_socket);
}

void modo_Servidor(void) {
    printf("Bem vindo ao RChat\n\n");
    printf("configurando seu server...\n");

    int listen_socket = -1, client_socket = -1;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);

    if (bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        close(listen_socket);
        return;
    }

    if (listen(listen_socket, SOMAXCONN) < 0) {
        perror("listen");
        close(listen_socket);
        return;
    }

    system("clear");
    printf("Servidor aberto na porta %d\n", DEFAULT_PORT);

    client_socket = accept(listen_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (client_socket < 0) {
        perror("accept");
        close(listen_socket);
        return;
    }

    printf("cliente conectado: %s\n", inet_ntoa(clientAddr.sin_addr));

    int *pclient = malloc(sizeof(int));
    if (!pclient) { close(client_socket); close(listen_socket); return; }
    *pclient = client_socket;

    pthread_t tid;
    if (pthread_create(&tid, NULL, receber, pclient) != 0) {
        perror("pthread_create");
        free(pclient);
        close(client_socket);
        close(listen_socket);
        return;
    }
    pthread_detach(tid);

    while (1) {
        char msg[512];
        if (fgets(msg, sizeof(msg), stdin) == NULL) break;
        msg[strcspn(msg, "\n")] = 0;
        if (send(client_socket, msg, strlen(msg), 0) < 0) {
            perror("send");
            break;
        }
        if (strcmp(msg, "/exit") == 0) break;
    }

    close(client_socket);
    close(listen_socket);
}

int main(void) {
    while (1) {
        int opcao;
        printf("Escolha o modo:\n1- Servidor\n2- Cliente\n99- Sair\nOpcao: ");
        if (scanf("%d", &opcao) != 1) break;
        getchar();

        if (opcao == 1) {
            modo_Servidor();
            printf("conexao encerrada...\n");
            sleep(2);
            system("clear");
        } else if (opcao == 2) {
            modo_Cliente();
            printf("conexao encerrada...\n");
            sleep(2);
            system("clear");
        } else if (opcao == 99) {
            break;
        } else {
            printf("opcao invalida\n");
            sleep(2);
            system("clear");
        }
    }
    return 0;
}
