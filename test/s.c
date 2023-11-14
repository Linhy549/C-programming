#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

int serverfd;//服务器socket
int clientfd[100];//客户端的socketfd,100个元素，clientfd[0]~clientfd[99]
int size = 6;//用来控制进入聊天室的人数为50以内
char* IP = "127.0.0.1";//主机ip地址
short PORT = 6666;//端口号
typedef struct sockaddr meng;
int client_count = 0;
time_t nowtime;

struct ClientInfo {
    char name[8];
    int socket;
    struct sockaddr_in address;
};

struct ClientInfo clients[6];

void init(){

    serverfd = socket(PF_INET,SOCK_STREAM,0);

    if (serverfd == -1){
        perror("创建socket失败");
        exit(-1);
    }

    //为套接字设置ip协议 设置端口号  并自动获取本机ip转化为网络ip
    struct sockaddr_in addr;//存储套接字的信息
    addr.sin_family = PF_INET;//地址族
    addr.sin_port = htons(PORT);//设置server端端口号，你可以随便设置,当sin_port = 0时，系统随机选择一个未被使用的端口号
    addr.sin_addr.s_addr = inet_addr(IP);//把127.0.0.1改为自己的server端的ip地址,当sin_addr = INADDR_ANY时，表示从本机的任一网卡接收数据
    //绑定套接字

    if (bind(serverfd,(meng*)&addr,sizeof(addr)) == -1){
       perror("绑定失败");
       exit(-1);
    }

    if (listen(serverfd,100) == -1){//监听最大连接数
        perror("设置监听失败");
        exit(-1);
    }

}

void SendAll(char* msg){

    int i;

    for (i = 0;i < size;i++){

        if (clientfd[i] != 0){

            printf("发送给%d\n",clientfd[i]);

            printf("发送的信息是: %s\n",msg);

            //写入文件

            char buf[1024];


            FILE *logs = fopen("log.txt", "a+");

            if(logs== NULL)

            {

            printf("open file erroe: \n");

            }else{

                sprintf(buf, "进入时间：%s\tIP地址：%s\n",ctime(&nowtime),IP);

                fputs(buf,logs);

                sprintf(buf, "所发信息：%s\n",msg);

                fputs(buf,logs);

                fclose(logs);

            }

            send(clientfd[i],msg,strlen(msg),0);

        }

    }

}

void* server_thread(void *p){

    int fd = *(int*)p;
    
    printf("###\n");
    printf("Log - create server_thread\n");
    printf("Clients obj add: %p\n",&clients);
    struct ClientInfo client;
    for(int i = 0; i < 6; i++){
        printf("fd: %d\n", fd);
        printf("socket: %s\n", clients[i].name);
        // if (clients[i].socket == fd){
        //     client = clients[i];
        //     printf("\t- User name: %s\n",client.name);
        //     break;
        // }
    }
    int temp = client.socket;
    printf("\t- pthread = %d\n", temp);

    while(1){

        char buf[100] = {};

        if (recv(fd,buf,sizeof(buf),0) <= 0){

            int i;

            for (i = 0;i < size;i++){

                if (fd == clientfd[i]){

                    clientfd[i] = 0;
                    break;

                }

            }

            printf("退出：fd = %d 退出了。\n",fd);

            char buf[1024];

            FILE *logs = fopen("log.txt", "a");

            if(logs== NULL){

                printf("open file erroe: \n");

            } else {

                sprintf(buf, "退出时间：%s\tIP地址：%s\n",ctime(&nowtime),IP);

                fputs(buf,logs);

                fclose(logs);

            }

            pthread_exit(0);

        }

        //把服务器接受到的信息发给所有的客户端

        SendAll(buf);

    }

}

void* login(void* p){

    int fd = *(int*)p;
    printf("####\n");
    printf("Login - waiting user authenticate...\n");
    
    while(1){
        char buf[8] = {};
        printf("\t- waiting for user input\n");
        if (recv(fd, buf, sizeof(buf), 0) <= 0){
            break;
        }
        strcpy(clients[client_count].name, buf);
        printf("\t- user input: %s\n", clients[client_count].name);
        break;
    }
    return NULL;
}

void server(){

    printf("服务器启动\n");

    while(1){

        struct sockaddr_in fromaddr;

        socklen_t len = sizeof(fromaddr);

        int fd = accept(serverfd,(meng*)&fromaddr,&len);
        //调用accept进入堵塞状态，等待客户端的连接

        if (fd == -1){

            printf("客户端连接出错...\n");

            continue;

        }

        int i = 0;

        for (i = 0;i < size;i++){

            if (clientfd[i] == 0){

                //记录客户端的socket

                clientfd[i] = fd;

                // Add to clients list
                clients[client_count].socket = fd;
                clients[client_count].address = fromaddr;

                printf("线程号= %d\n", clients[client_count].socket);
                printf("线程号= %d\n", fd); //
                printf("id address: %p\n", &clients[client_count]);
                ++client_count;
                //有客户端连接之后，启动线程给此客户服务

                pthread_t tid;
                pthread_create(&tid,0,login,&fd);
                pthread_join(tid, NULL);
                pthread_create(&tid, NULL, server_thread, &fd);
                // client_count = client_count + 1;
                break;

            }

            if (size == i){

                //发送给客户端说聊天室满了

                char* str = "对不起，聊天室已经满了!";

                send(fd,str,strlen(str),0);

                close(fd);

            }

        }

    }

}

int main(){
    init();
    server();
}
