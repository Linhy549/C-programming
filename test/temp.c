#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <time.h>

int clientfd2;//客户端socket

char* IP = "127.0.0.1";//服务器的IP

short PORT = 6666;//服务器服务端口

typedef struct sockaddr meng;

char name[30];//设置支持的用户名长度

time_t nowtime;

void init(){

    clientfd2 = socket(PF_INET,SOCK_STREAM,0);//创建套接字

    struct sockaddr_in addr;//将套接字存在sockaddr_in结构体中

    addr.sin_family = PF_INET;//地址族

    addr.sin_port = htons(PORT);//端口号 可随意设置，不过不可超过规定的范围

    addr.sin_addr.s_addr = inet_addr(IP);//inet_addr()函数将点分十进制的字符串转换为32位的网络字节顺序的ip信息

//发起连接

    if (connect(clientfd2,(meng*)&addr,sizeof(addr)) == -1){

        perror("Cannot connect to server");

        exit(-1);

    }

    printf("Client connected\n");

}

void start(){

    pthread_t id;

    void* recv_thread(void*);

//创建一个线程用于数据的接收，一个用于数据的发送

    pthread_create(&id,0,recv_thread,0);

    char buf2[100] = {};

    sprintf(buf2,"%sjoin group chat",name);

    time(&nowtime);

    printf("Time Joined: %s\n",ctime(&nowtime));

    send(clientfd2,buf2,strlen(buf2),0);

    while(1){

        char buf[100] = {};

        scanf("%s",buf);

        char msg[500] = {};

        sprintf(msg,"%smessage:%s",name,buf);

        send(clientfd2,msg,strlen(msg),0);

        if (strcmp(buf,"quit") == 0){

            memset(buf2,0,sizeof(buf2));//初始化

            sprintf(buf2,"%shas been quited",name);

            send(clientfd2,buf2,strlen(buf2),0);

            break;

        }

    }

    close(clientfd2);

}

void* recv_thread(void* p){

    while(1){
        
        char buf[100] = {};
        if (recv(clientfd2,buf,sizeof(buf),0) <= 0){
            break;
        }

        printf("%s\n",buf);
    }

}

int main(){

    init();
    printf("Enter your name：");
    scanf("%s",name);
    printf("\n\n*****************************\n");
    printf("Welcome%s join group chat\n",name);
    printf("  Enter 'quit' to exit\n");
    printf("\n*****************************\n\n");
    start();
    return 0;

}
