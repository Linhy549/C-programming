#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

# define ID_LENGTH 9

int serverfd;//服务器socket
int clientfd[100];//客户端的socketfd,100个元素，clientfd[0]~clientfd[99]
int size = 6;//用来控制进入聊天室的人数为50以内
char* IP = "127.0.0.1";//主机ip地址
short PORT = 6666;//端口号
typedef struct sockaddr meng;
int client_count = 0;
time_t nowtime;

struct Protocol {
    char LOG[6]; // Size includes space for the null-terminator
    char LOG_END[10];
    
    char MSG[6];
    char MSG_END[8];
    
    char LOGOUT[9];
    char LOGOUT_END[13];
    
    char LOGIN_LIST[13];
    char LOGIN_LIST_END[17];
    
    char INFO[7];
    char INFO_END[11];
    
    char TO[5];
    char TO_END[9];
    
    char FROM[7];
    char FROM_END[11];
    
    char BODY[7];
    char BODY_END[11];
} protocol = {"<LOG>", "</LOG>",
              "<MSG>", "</MSG>",
              "<LOGOUT>", "</LOGOUT>",
              "<LOGIN_LIST>", "</LOGIN_LIST>",
              "<INFO>", "</INFO>",
              "<TO>", "</TO>",
              "<FROM>", "</FROM>",
              "<BODY>", "</BODY>",
 };

struct ClientInfo {
    char name[ID_LENGTH];
    int socket;
    struct sockaddr_in address;
};

struct ClientInfo clients[6];

int check_id(char id[]){
    // printf("Check_id\n");
    printf("checking \'%s\'...\n", id);
    if (strlen(id) != 8)
    {
        return 0;  // Check if the array length is not 8.
    }

    if (!isalpha(id[0])) {
        return 0;  // Check if the first character is not a letter.
    }

    for (int i = 1; i < 8; i++) {
        if (!isdigit(id[i])) {
            return 0;  // Check if the remaining characters are not digits.
        }
    }

    // printf("## Valid id ##");
    return 1;  // All conditions are met.
}

void LogIn_Pro(char buf[]) {
    char* startTag = strstr(buf, protocol.LOG);
    char* endTag = strstr(buf, protocol.LOG_END);

    if (startTag != NULL && endTag != NULL) {
        // Calculate the length of the content between the tags
        size_t contentLength = endTag - (startTag + strlen(protocol.LOG));

        // Ensure the content length does not exceed the destination array size - 1
        
        // Copy the content between the tags into the destination array
        strncpy(buf, startTag + strlen(protocol.LOG), contentLength);
        buf[contentLength] = '\0'; // Null-terminate the destination array

        // Print the extracted content
        printf("Extracted content: %s\n", buf);
    }
}

void Login_List_Pro(void* p){
    int fd = *(int*)p;
    char buf[100];
    buf[0] = '\0';
    strcat(buf, protocol.LOGIN_LIST);
    for (int i = 0; i < 6; i++){
        if(clients[i].name[0] != '\0'){
            char temp[2] = ";";
            strcat(buf, clients[i].name);
            strcat(buf, temp);
        }
    }
    strcat(buf, protocol.LOGIN_LIST_END);
    send(fd, buf, strlen(buf), 0);
}

void MSG_Pro(char arr[]) {
    char c1[20], c2[20], text[100];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[500] = {};
    frame[0] = '\0';

    if (fromStart != NULL && fromEnd != NULL) {
        strncpy(c1, fromStart + 6, fromEnd - (fromStart + 6));
        c1[fromEnd - (fromStart + 6)] = '\0'; // Null-terminate the string
    }

    if (toStart != NULL && toEnd != NULL) {
        strncpy(c2, toStart + 4, toEnd - (toStart + 4));
        c2[toEnd - (toStart + 4)] = '\0'; // Null-terminate the string
    }

    if (bodyStart != NULL && bodyEnd != NULL) {
        strncpy(text, bodyStart + 6, bodyEnd - (bodyStart + 6));
        text[bodyEnd - (bodyStart + 6)] = '\0'; // Null-terminate the string
    }

    strcat(frame, protocol.MSG);
    strcat(frame, protocol.FROM);
    strcat(frame, c2);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, c1);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);
    strcat(frame, text);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);

    // printf("Prep data: %s\n", frame);
    for (int i = 0; i < 6; i++)
    {
        if(strcmp(clients[i].name, c2) == 0){
            printf("Prep data to %s\n", clients[i].name);
            // printf("Redirect mes: %s\n", frame);
            send(clients[i].socket, frame, strlen(frame), 0);
        }
    }
}

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

void* server_thread(void* p){

    int fd = *(int*)p;

    printf("socket: %d\n", fd);

    printf("Reading to print all client: \n");
    for (int i = 0; i < 6; i++){
        printf("%d. socket: %d, name: %s\n", i + 1, clients[i].socket, clients[i].name);
    }
        while (1)
        {

            char buf[1000] = {};

            int flag = recv(fd, buf, sizeof(buf), 0);

            if(strstr(buf, "<LOGIN_LIST>") != NULL){

                printf("client request <LOGIN_LIST>: %s\n\n", buf);
                Login_List_Pro(&fd);

            } else if(strstr(buf, "<MSG>") != NULL){

                printf("client request <MSG>: %s\n", buf);
                MSG_Pro(buf);
            }
            else if (strstr(buf, "<LOGOUT>") != NULL || flag <= 0)
            {
                printf("退出：fd = %d 退出了。\n", fd);
                for (int i = 0; i < 6; i++){
                    if(fd == clients[i].socket){
                        clients[i].socket = 0;
                        clients[i].name[0] = '\0';
                    }
                }
                pthread_exit(0);
            }else {
                printf("%s\n",buf);
            }

            // if (recv(fd, buf, sizeof(buf), 0) <= 0)
            // {

            //     int i;

            //     // for (i = 0; i < size; i++)
            //     // {

            //     //     if (fd == clientfd[i])
            //     //     {

            //     //         clientfd[i] = 0;
            //     //         break;
            //     //     }
            //     // }

            //     printf("退出：fd = %d 退出了。\n", fd);

            //     char buf[1024];

            //     FILE *logs = fopen("log.txt", "a");

            //     if (logs == NULL)
            //     {

            //         printf("open file erroe: \n");
            //     }
            //     else
            //     {

            //         sprintf(buf, "退出时间：%s\tIP地址：%s\n", ctime(&nowtime), IP);

            //         fputs(buf, logs);

            //         fclose(logs);
            //     }

            //     pthread_exit(0);
            // }
        }
}

void* login(void* p){

    int fd = *(int*)p;
    printf("Login - waiting user authenticate...\n");
    
    while(1){
        char buf[30] = {};
        printf("waiting...\n");
        if (recv(fd, buf, sizeof(buf), 0) <= 0){
            break;
        }
        strcpy(clients[client_count].name, buf);
        printf("Get name, done\n");
        break;
    }
    return NULL;
}

void server(){

    printf("服务器启动\n");

    while(1){

        struct sockaddr_in fromaddr;

        socklen_t len = sizeof(fromaddr);

        // printf("Waiting for client to connect...\n");
        int fd = accept(serverfd,(meng*)&fromaddr,&len);
        //调用accept进入堵塞状态，等待客户端的连接

        if (fd == -1){
            printf("客户端连接出错...\n");
            continue;
        }

        int i = 0;

        // printf("#Log- Client connect, fd: %d\n", fd);

        //check Id
        char id[30];
        recv(fd, id, sizeof(id), 0);
        printf("Recv: %s", id);
        LogIn_Pro(id);
        int login = check_id(id);
        char loginFlag[2];
        snprintf(loginFlag, sizeof(loginFlag), "%d", login); 
        send(fd, loginFlag, strlen(loginFlag), 0);

        printf("Login #: %d\n", login);
        // valid Id
        if(login){
                strcpy(clients[client_count].name, id);
                clients[client_count].socket = fd;
                clients[client_count].address = fromaddr;
                client_count++;
                pthread_t tid;
                pthread_create(&tid, 0, server_thread, &fd);
        } else {
            printf("\n id is invalid so close socket...\n");
            close(fd);
        }
        // for (i = 0;i < size;i++){

        //     if (clientfd[i] == 0){

        //         //记录客户端的socket

        //         clientfd[i] = fd;

        //         // Add to clients list
        //         clients[client_count].socket = fd;
        //         clients[client_count].address = fromaddr;



        //         //有客户端连接之后，启动线程给此客户服务

        //         pthread_t tid;
        //         // pthread_create(&tid,0,login,&fd);
        //         // pthread_join(tid, NULL);

        //         // pthread_create(&tid, 0, (void * (*)(void *))server_thread, &clients[client_count]);
        //         // client_count = client_count + 1;
        //         break;

        //     }

        //     if (size == i){

        //         //发送给客户端说聊天室满了

        //         char* str = "对不起，聊天室已经满了!";

        //         send(fd,str,strlen(str),0);

        //         close(fd);

        //     }

        // }

    }

    close(serverfd);
}

int main(){
    init();
    server();
}
