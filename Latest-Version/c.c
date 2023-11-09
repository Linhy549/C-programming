#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

# define ID_LENGTH 9
int clientfd2;//客户端socket
char client_name[30];
char *IP = "127.0.0.1"; // 服务器的IP
short PORT = 6666;//服务器服务端口
typedef struct sockaddr meng;
char name[30];//设置支持的用户名长度
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

void start(){

    pthread_t id;
    void* recv_thread(void*);

    //创建一个线程用于数据的接收，一个用于数据的发送

    pthread_create(&id,0,recv_thread,0);

    char buf2[100] = {};

    sprintf(buf2,"%s进入了群聊",name);

    time(&nowtime);

    printf("进入的时间是: %s\n",ctime(&nowtime));

    send(clientfd2,buf2,strlen(buf2),0);

    while(1){

        char buf[100];

        scanf("%s",buf);

        char msg[500] = {};

        sprintf(msg,"%s发送的信息是:%s ",name,buf);

        send(clientfd2,msg,strlen(msg),0);

        if (strcmp(buf,"quit") == 0){

            memset(buf2,0,sizeof(buf2));//初始化

            sprintf(buf2,"%s退出了群聊",name);

            send(clientfd2,buf2,strlen(buf2),0);

            break;

        }

    }

    close(clientfd2);

}

void LoginList_Pro(char buf[]){
    char* startTag = strstr(buf, protocol.LOGIN_LIST);
    char* endTag = strstr(buf, protocol.LOGIN_LIST_END);

    if (startTag != NULL && endTag != NULL) {
        // Calculate the length of the content between the tags
        size_t contentLength = endTag - (startTag + strlen(protocol.LOGIN_LIST));

        // Ensure the content length does not exceed the destination array size - 1
        
        // Copy the content between the tags into the destination array
        strncpy(buf, startTag + strlen(protocol.LOGIN_LIST), contentLength);
        buf[contentLength] = '\0'; // Null-terminate the destination array

        // Print the extracted content
        printf("\n=====================\n");
        printf("Current Online user: %s\n", buf);
    }
}

void MSG_Pro(){
    char user_id[9];
    char msg[500] = {};
    char frame[700] = {};
    frame[0] = '\0';
    msg[0] = '\0';
    printf("\n====================\n");
    printf("Enter user-id you want to chat: ");
    scanf("%s", user_id);
    printf("Enter message: ");
    scanf("%s", msg);

    FILE *file = fopen("set.txt", "rb"); // Open the file in binary mode

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the data
    char data[file_size + 1]; // +1 for null terminator

    // Read the file into the data array
    fread(data, sizeof(char), file_size, file);
    data[file_size] = '\0'; // Null-terminate the data

    // Close the file
    fclose(file);

    // Print or use the read data
    printf("Read data:\n%s\n", data);

    strcat(frame, protocol.MSG);
    strcat(frame, protocol.FROM);
    strcat(frame, client_name);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, user_id);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);
    strcat(frame, data);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);


    send(clientfd2, frame, strlen(frame), 0);
}

void MSG_Pro_Get(char arr[]){
    char c1[20], c2[20], text[100];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[700] = {};
    frame[0] = '\0';

    if (fromStart != NULL && fromEnd != NULL) {
        strncpy(c1, fromStart + 6, fromEnd - (fromStart + 6));
        c1[fromEnd - (fromStart + 6)] = '\0'; // Null-terminate the string
    }

    if (bodyStart != NULL && bodyEnd != NULL) {
        strncpy(text, bodyStart + 6, bodyEnd - (bodyStart + 6));
        text[bodyEnd - (bodyStart + 6)] = '\0'; // Null-terminate the string
    }

    printf("%s to you: %s\n", c1, text);
}

void* recv_thread(void* p){
    printf("#Log - Client recv thread creating...\n");
    while(1){

        char buf[500] = {};

        if (recv(clientfd2,buf,sizeof(buf),0) <= 0){
            break;
        }

        if(strstr(buf, "<LOGIN_LIST>") != NULL){
            LoginList_Pro(buf);
        } 

        if(strstr(buf, "<MSG>") != NULL){
            MSG_Pro_Get(buf);
        }
    }

}

int check_id(char *id){
    clientfd2 = socket(PF_INET,SOCK_STREAM,0);//创建套接字
    struct sockaddr_in addr;//将套接字存在sockaddr_in结构体中
    addr.sin_family = PF_INET;//地址族
    addr.sin_port = htons(PORT);//端口号 可随意设置，不过不可超过规定的范围
    addr.sin_addr.s_addr = inet_addr(IP);//inet_addr()函数将点分十进制的字符串转换为32位的网络字节顺序的ip信息

    //发起连接
    if (connect(clientfd2,(meng*)&addr,sizeof(addr)) == -1){

        perror("无法连接到服务器");
        exit(-1);

    }

    char buff[30];
    buff[0] = '\0';
    strcat(buff, protocol.LOG);
    strcat(buff, id);
    strcat(buff, protocol.LOG_END);
    send(clientfd2, buff, strlen(buff), 0);

    char loginFlag[2];
    recv(clientfd2,loginFlag,strlen(loginFlag),0);
    printf("#Log- Recv loginFlag: %s\n", loginFlag);
    strcpy(client_name, id);

    if (1)
    {
        pthread_t tid;
        pthread_create(&tid,0,recv_thread,&clientfd2);
        return 1;
    }
    else
    {
        return 0;
    }
}

void getAll(){
    // send(clientfd2,id,strlen(id),0);
}

int main(){
    int loop = 1;
    while(loop){
        printf("=== Welcome to One-One chat Application ===\n");
        printf("1. Login\n");
        printf("2. Exit\n");
        printf("Please enter: ");

        int option;

        scanf("%d", &option);
        
        switch(option) 
        {
            case 1:
                char id[ID_LENGTH]; // id 8-bits long
                printf("\nPlease enter your ID: ");
                scanf("%s", id);
                printf("\nYour Id: %s\n", id);


                if(check_id(id)){

                    int loop2 = 1;

                    while(loop2) {
                        printf("=== User Id: %s ===\n", id);
                        printf("1. Show online users\n");
                        printf("2. Private-chat\n");
                        printf("3. Exit\n");
                        printf("Please enter: ");

                        scanf("%d", &option);

                        

                        switch (option)
                        {
                            case 1:
                                char buf[500] = {};
                                printf("Show online users - fun\n");
                                strcat(buf, protocol.LOGIN_LIST);
                                strcat(buf, client_name);
                                strcat(buf, protocol.LOGIN_LIST_END);
                                printf("socket : %d\n", clientfd2);
                                printf("mes : %s\n", buf);
                                send(clientfd2, buf, strlen(buf), 0);
                                break;
                            case 2:
                                printf("Private-chat - fun\n");
                                MSG_Pro();
                                break;
                            case 3:
                                printf("exit\n");
                                close(clientfd2);
                                loop2 = 0;
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    printf("=== Invalid Id ===\n");
                }
                break;

            case 2:
                printf("Exit\n");
                loop = 0;
                break;
            default:
                printf("Invalid\n");
        }
    }


    start();

    return 0;

}
