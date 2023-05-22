#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>



#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5188

#define SIZE 1024
#define SIZE_MODE 1
#define SIZE_FILENAME 99
#define SIZE_FILECONTENT (SIZE - SIZE_MODE - SIZE_FILENAME)

void strtrim(char* str)
{
    if (str == NULL)
        return;

    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[len - 1] = '\0';
        len--;
    }
}

void do_read(int sockid, int *flag)
{
    char receive[SIZE] = {0};
    int r_size = read(sockid, receive, sizeof(receive));

    if (strcmp(receive, "Quit\n") == 0)
    {
        printf("对方已结束聊天\n");
        *flag = 0;
        return;
    }

    if (r_size > 0)
    {
        if (receive[0] == 'F')
        {
            char filename[SIZE_FILENAME] = {0};
            char filecontent[SIZE_FILECONTENT] = {0};
            strncpy(filename, receive + SIZE_MODE, SIZE_FILENAME);
            strncpy(filecontent, receive + SIZE_MODE + SIZE_FILENAME, SIZE_FILECONTENT);
            FILE *fp = fopen(filename, "wb");
            fwrite(filecontent, sizeof(char), SIZE_FILECONTENT, fp);
            fclose(fp);
            printf("%s Recieve succeed!\n", filename);
        }
        else if (receive[0] == 'C')
        {
            printf("Received: %s\n", receive);
            fputs(receive + SIZE_MODE + SIZE_FILENAME, stdout);

        }
    }
    else
    {
        printf("Recieve error！\n");
    }
}

void do_write(int sockid, int *flag)
{
    char send[SIZE] = {0};
    int w_size = read(0, send, sizeof(send));

    if (strcmp(send, "Quit\n") == 0)
    {
        printf("您已下线\n");
        write(sockid, send, SIZE);
        *flag = 0;
        return;
    }

    if (w_size > 0)
    {
        if (send[0] == 'F')
        {
            char filename[SIZE_FILENAME] = {0};
            strncpy(filename, send + SIZE_MODE+1, SIZE_FILENAME - 1);
            filename[SIZE_FILENAME - 1] = '\0';
            strtrim(filename);
            FILE *fp = fopen(filename, "rb");
            if (fp == NULL)
            {
                printf("%s File doesn't exist！\n", filename);
                return;
            }
            else
            {
                size_t read_size = fread(send + SIZE_MODE + SIZE_FILENAME, sizeof(char), SIZE_FILECONTENT, fp);
                fclose(fp);

                if (read_size > 0)
                {
                    write(sockid, send, SIZE);
                    printf("%s already sent!\n", filename);
                }
                else
                {
                    printf("Fail to read file！\n");
                }

            }
        }
        else if (send[0] == 'C')
        {
            write(sockid, send, SIZE);
        }
        else
        {
            printf("Format error,Please re-enter\n");
        }
    }
    memset(send, 0, sizeof(send));
}

int internet()
{
    int flag = 1;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    socklen_t addrlen = sizeof(addr);
    if (connect(sockid, (struct sockaddr *)&addr, addrlen) < 0)
    {
        printf("连接失败\n");
        return 0;
    }

    printf("Please message in the following format：F filename/C message/Quit\n");
    char send[SIZE] = {0}; // 发送缓冲区
    while (flag)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockid, &fds);
        FD_SET(STDIN_FILENO, &fds);

        // 使用select函数监听套接字和标准输入
        int maxfd = (sockid > STDIN_FILENO) ? sockid : STDIN_FILENO;
        int activity = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            printf("select error\n");
            return 0;
        }

        // 套接字上有数据可读
        if (FD_ISSET(sockid, &fds))
        {
            do_read(sockid, &flag);
        }

        // 标准输入有数据可读
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            do_write(sockid, &flag);
        }
    }

    close(sockid);
    return 0;
}

int main()
{
    internet();
    return 0;
}