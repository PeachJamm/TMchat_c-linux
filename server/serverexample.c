#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888

typedef struct {
    char sender[20];
    char receiver[20];
    char content[1024];
} Message;

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[sizeof(Message)];

    // 创建一个 TCP socket 对象
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口号
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    // 绑定 socket 对象到本地地址和端口号
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (listen(server_socket, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 接收客户端的连接请求，建立连接
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // 接收消息对象
        if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        Message *message = (Message *) buffer;
        printf("Received message: from %s to %s: %s\n", message->sender, message->receiver, message->content);

        // 处理多次通信，直到收到结束信号
        while (1) {
            // 接收消息对象
            if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
                perror("recv failed");
                exit(EXIT_FAILURE);
            }

            // 检查是否收到结束信号
            if (strcmp(buffer, "ctrl+Q") == 0) {
                printf("Connection closed.\n");
                break;
            }

            // 显示接收到的消息内容
            Message *message = (Message *) buffer;
            printf("Received message: from %s to %s: %s\n", message->sender, message->receiver, message->content);
        }

        // 关闭连接
        close(client_socket);
    }

    // 关闭服务器 socket 对象
    close(server_socket);

    return 0;
}
