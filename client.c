#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5188

#define SIZE 1024
#define SIZE_MODE 1
#define SIZE_FILENAME 99
#define SIZE_FIMECONTENT SIZE-SIZE_MODE-SIZE_FILENAME
//第1个char是F，紧接着前99个是文件名，紧接着924个是file content

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
            char filecontent[SIZE_FIMECONTENT] = {0};
            strncpy(filename, receive + SIZE_MODE, SIZE_FILENAME);
            strncpy(filecontent, receive + SIZE_MODE + SIZE_FILENAME, SIZE_FIMECONTENT);
            FILE *fp = fopen(filename, "w");
            fwrite(filecontent, sizeof(char), SIZE_FIMECONTENT, fp);
            fclose(fp);
            printf("%s 接收文件成功!\n", filename);
        }
        else if (receive[0] == 'C')
        {
            fputs(receive + SIZE_MODE + SIZE_FILENAME, stdout);
        }
    }
    else
    {
        printf("接收出错！\n");
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
            strncpy(filename, send + SIZE_MODE, SIZE_FILENAME);
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("%s 文件不存在！\n", filename);
                return;
            }
            else
            {
                fread(send + SIZE_MODE + SIZE_FILENAME, sizeof(char), SIZE_FIMECONTENT, fp);
                fclose(fp);
                write(sockid, send, SIZE);
                printf("%s 发送成功!\n", filename);
            }
        }
        else if (send[0] == 'C')
        {
            write(sockid, send, SIZE);
        }
        else
        {
            printf("输入格式错误,请重新输入\n");
        }
    }
    memset(send, 0, sizeof(send));
}
int internet()
{
    int  flag=1;
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(SERVER_PORT);
    addr.sin_addr.s_addr=inet_addr(SERVER_IP);
    int sockid=socket(AF_INET,SOCK_STREAM,0);
    socklen_t addrlen=sizeof(addr);
    if(connect(sockid,(struct sockaddr *)&addr,addrlen)<0)
    {
        printf("connect\n");
        return 0;
    }
    int flags=fcntl(sockid,F_GETFL,0);
    fcntl(sockid,F_SETFL,flags|O_NONBLOCK);
    flags=fcntl(0,F_GETFL,0);
    fcntl(0,F_SETFL,flags|O_NONBLOCK);

    printf("请按照一下格式输入内容：F filename/C message/Quit\n");
    char send[SIZE]={0};//发送缓存区
    while(flag)
    {
        do_read(sockid,&flag);
        do_write(sockid,&flag);
    }
    close(sockid);
    return 0;
}

int main()
{
    internet();
    return 0;
}
