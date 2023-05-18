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
#define SERVER_IP 5188

#define SIZE 1024
#define SIZE_MODE 1
#define SIZE_FILENAME 99
#define SIZE_FIMECONTENT SIZE-SIZE_MODE-SIZE_FILENAME
//第一个char是F，紧接着前99个是文件名，紧接着924个是file content

void do_read(int sockid,int *flag)
{
	char receive[100]={0};
	int r_size=read(sockid,receive,sizeof(receive));
	if(strcmp(receive,"quit\n")==0)
	{
		printf("对方已结束聊天\n");
		*flag=0;
		return;
	}
	if(r_size>0)
	{
		printf("\t\t\t");
		fputs(receive,stdout);
	}
}

void do_write(int sockid,int *flag)
{
    memset(send,0,strlen(send));
	int w_size=read(0,send,sizeof(send));//用户输入存到send


	if(send[0]=='Q')
	{
		printf("您已下线\n");
		write(sockid,send,SIZE);//把q发给对方，通知对方自己下线了
		*flag=0;
		return;
	 }

    if(w_size>0)
	{
        if(send[0]=='F'){

            char filename[SIZE_FILENAME]={0};
            strcpy(filename,send+SIZE_MODE,SIZE_FILENAME);
            FILE *fp = fopen(filename, "r");
            if(NULL == fp)
            {
                printf("%s 文件不存在！\n", file_name);
                return;
            }
            else
            {
                fwrite(send,sizeof(char),SIZE_FIMECONTENT,fp);
                fclose(fp);
                write(sockid,send,SIZE);
                printf("%s 发送成功!\n", file_name);
        }
            else if(send[0]=='C'){
                write(sockid,send,SIZE);
                printf("%s 发送成功!\n", file_name);
            }
            }
            else {
                    printf("输入格式错误,请重新输入");
                }


	}
    memset(send,0,strlen(send));
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

