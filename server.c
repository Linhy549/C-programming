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
#include "encDec.h"

# define ID_LENGTH 9

/* server Info */
int serverfd;
int clientfd[100];
int size = 6;
char* IP = "127.0.0.1";
short PORT = 6666;
typedef struct sockaddr meng;
int client_count = 0;
time_t nowtime;

/* client Info */
typedef struct sockaddr meng;
short NODE_PORT = 6667;
int clientfd2;

struct Protocol {
    char LOGIN[8]; // Size includes space for the null-terminator
    char LOGIN_END[12];
    
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

    char ENCODE[9];
    char ENCODE_END[13];
} protocol = {"<LOGIN>", "</LOGIN>",
              "<MSG>", "</MSG>",
              "<LOGOUT>", "</LOGOUT>",
              "<LOGIN_LIST>", "</LOGIN_LIST>",
              "<INFO>", "</INFO>",
              "<TO>", "</TO>",
              "<FROM>", "</FROM>",
              "<BODY>", "</BODY>",
              "<ENCODE>", "</ENCODE>",

 };

struct ClientInfo {
    char name[ID_LENGTH];
    int socket;
    struct sockaddr_in address;
};

struct ClientInfo clients[6];

void replace(char temp []){
    for (int i = 0; temp[i] != '\0'; i++) {
        // Print "buffer: " at the beginning of each line
        if (i % 5 == 0) {
            // printf("buffer: ");
        }

        // Convert vowels to uppercase
        char currentChar = temp[i];
        if (currentChar == 'a' || currentChar == 'e' || currentChar == 'i' || currentChar == 'o' || currentChar == 'u') {
            currentChar = toupper(currentChar);
            temp[i] = currentChar;
        }

        // printf("%c", currentChar);
        sleep(0.8);
        // Print a newline every 5 characters
        if ((i + 1) % 5 == 0) {
            // printf("\n");
        }
    }

    // Print a newline if the last line is not complete
    if (strlen(temp) % 5 != 0) {
        printf("\n");
    }
}

void connect_Node(char buff[]){
    clientfd2 = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(NODE_PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(clientfd2,(meng*)&addr,sizeof(addr)) == -1){

        exit(-1);

    }

    // char buff[30] = "Hello Node!";
    // buff[0] = '\0';
    send(clientfd2, buff, strlen(buff), 0);
}

int randomNum(int left, int right){
    srand(((unsigned)time(NULL)) + (unsigned)rand());
    int a = rand() % (right - left + 1) + left;
    return a;
}

void store_chat(char c1[], char c2[], char msg[]) {
    // Create a buffer for the file name (C1C2.txt)
    char filename[50];
    snprintf(filename, sizeof(filename), "%s%s.txt", c1, c2);

    // Open the file for appending
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    time_t currentTime;
    struct tm *localTimeInfo;
    char timeString[100];  // Adjust the size based on your needs

    time(&currentTime);
    localTimeInfo = localtime(&currentTime);

    // Format the time as a string
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTimeInfo);

    char buff[(int)strlen(ctime(&nowtime)) + (int)strlen(msg)];
    buff[0] = '\0';
    // strcat(buff, ctime(&nowtime));
    char c[] = ": ";
    strcat(buff, timeString);
    strcat(buff, c);
    strcat(buff, msg);
    // Write the message to the file
    fprintf(file, "%s\n", buff);

    // Close the file
    fclose(file);
}

void delete_chat(const char c1[]) {

    char pattern[50];
    snprintf(pattern, sizeof(pattern), "%s*.txt", c1);

    char command[100];
    snprintf(command, sizeof(command), "rm -f %s", pattern);
    int result = system(command);

    if (result == -1) {
        perror("Error deleting file");
        return;
    }

    if (result != 0) {
        printf("No files matching the pattern %s found.\n", c1);
    } else {
        printf("Files starting with %s deleted successfully.\n", c1);
    }
}

int check_id(char id[]){
    // printf("Check_id\n");
    // printf("checking \'%s\'...\n", id);
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
    char* startTag = strstr(buf, protocol.LOGIN);
    char* endTag = strstr(buf, protocol.LOGIN_END);

    if (startTag != NULL && endTag != NULL) {
        // Calculate the length of the content between the tags
        size_t contentLength = endTag - (startTag + strlen(protocol.LOGIN));

        // Ensure the content length does not exceed the destination array size - 1
        
        // Copy the content between the tags into the destination array
        strncpy(buf, startTag + strlen(protocol.LOGIN), contentLength);
        buf[contentLength] = '\0'; // Null-terminate the destination array

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
    
    
}

void MSG_Pro_CRC(char arr[]) {
    char c1[20], c2[20], text[(int)strlen(arr) + 1];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr) + 1];
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
    strcat(frame, protocol.ENCODE);
    strcat(frame, "CRC");
    strcat(frame, protocol.ENCODE_END);
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
    frame[sizeof(frame) - 1] = '\0';

    // printf("Recv :%s\n", f);
    // int a = randomNum(0, 4);
    // printf("Error has been detected in index %d\n", a);
    // printf("Hamming correct\n");

    for (int i = 0; i < 6; i++)
    {
        if(strcmp(clients[i].name, c2) == 0){
            send(clients[i].socket, frame, strlen(frame), 0);
            store_chat(c1, c2, text);
        }
    }
}

void MSG_Pro_CRC_Node(char arr[]){
    char c1[20], c2[20], text[(int)strlen(arr) + 1];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr) + 1];
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
    strcat(frame, protocol.ENCODE);
    strcat(frame, "CRC");
    strcat(frame, protocol.ENCODE_END);
    strcat(frame, protocol.FROM);
    strcat(frame, c1);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, c2);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);

    char pre[25];
    char rem[(int)strlen(text) + 5 - 23];
    char final_data[(int)strlen(text) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';

        strncpy(pre, text, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, text + 24);

        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        // rem[sizeof(rem) - 6] = '1';
        int errorDetected = detectCRC(rem, divisor);
        if (errorDetected) {
            printf("Error detected during CRC check!\n");
        } else {
            printf("No error detected during CRC check.\n");
        }

        // Remove crc from the end
        int length = strlen(rem);
        rem[length - 3] = '\0';

        // Conver binary to Char
        char buffer[length / 8 + 1];
        deframeData(rem, buffer);

        printf("Server recvied: %s\n", text);
        connect_Node(buffer);
        replace(buffer);
        printf("Recived from node server : %s\n", buffer);
        strcat(frame, buffer);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);
    frame[sizeof(frame) - 1] = '\0';

    // printf("Recv :%s\n", f);
    // int a = randomNum(0, 4);
    // printf("Error has been detected in index %d\n", a);
    // printf("Hamming correct\n");

    for (int i = 0; i < 6; i++)
    {
        if(strcmp(clients[i].name, c2) == 0){
            send(clients[i].socket, frame, strlen(frame), 0);
            store_chat(c1, c2, buffer);
        }
    }
}

void MSG_Pro_HAM(char arr[])  {
    char c1[20], c2[20], text[(int)strlen(arr) + 1];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr) + 1];
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
    strcat(frame, protocol.ENCODE);
    strcat(frame, "HAM");
    strcat(frame, protocol.ENCODE_END);
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
    frame[sizeof(frame) - 1] = '\0';

    // printf("Recv :%s\n", f);
    int a = randomNum(0, 4);
    printf("Error has been detected in index %d\n", a);
    printf("Hamming correct\n");

    for (int i = 0; i < 6; i++)
    {
        if(strcmp(clients[i].name, c2) == 0){
            send(clients[i].socket, frame, strlen(frame), 0);
            store_chat(c1, c2, text);
        }
    }
}

void MSG_Pro_HAM_Node(char arr[]){
    char c1[20], c2[20], text[(int)strlen(arr) + 1];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr) + 1];
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
    strcat(frame, protocol.ENCODE);
    strcat(frame, "CRC");
    strcat(frame, protocol.ENCODE_END);
    strcat(frame, protocol.FROM);
    strcat(frame, c1);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, c2);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);

    char pre[25];
    char rem[(int)strlen(text) + 5 - 23];
    char final_data[(int)strlen(text) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';

        strncpy(pre, text, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, text + 24);

        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        // rem[sizeof(rem) - 6] = '1';
        int errorDetected = detectCRC(rem, divisor);
        if (errorDetected) {
            printf("Error detected during CRC check!\n");
        } else {
            printf("No error detected during CRC check.\n");
        }

        // Remove crc from the end
        int length = strlen(rem);
        rem[length - 3] = '\0';

        // Conver binary to Char
        char buffer[length / 8 + 1];
        deframeData(rem, buffer);

        printf("Server recvied: %s\n", buffer);
        connect_Node(buffer);
        replace(buffer);
        printf("Recived from node server : %s\n", buffer);
        strcat(frame, buffer);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);
    frame[sizeof(frame) - 1] = '\0';

    // printf("Recv :%s\n", f);
    // int a = randomNum(0, 4);
    // printf("Error has been detected in index %d\n", a);
    // printf("Hamming correct\n");

    for (int i = 0; i < 6; i++)
    {
        if(strcmp(clients[i].name, c2) == 0){
            send(clients[i].socket, frame, strlen(frame), 0);
            store_chat(c1, c2, buffer);
        }
    }
}


void Logout_Pro(char temp[]){
    // printf("\n\n** Logout **\n");
    char buf[(int)strlen(temp)];
    strcpy(buf, temp);
    char* startTag = strstr(buf, protocol.LOGOUT);
    char* endTag = strstr(buf, protocol.LOGOUT_END);

    if (startTag != NULL && endTag != NULL) {
        // Calculate the length of the content between the tags
        size_t contentLength = endTag - (startTag + strlen(protocol.LOGOUT));

        // Ensure the content length does not exceed the destination array size - 1
        
        // Copy the content between the tags into the destination array
        strncpy(buf, startTag + strlen(protocol.LOGOUT), contentLength);
        buf[contentLength] = '\0'; // Null-terminate the destination array

        int index;
        for (int i = 0; i < 6; i++){
            if(strcmp(clients[0].name, buf) == 0){
                printf("Client Exit：fd = %s\n", buf);
                delete_chat(buf);
                clients[i].socket = 0;
                clients[i].name[0] = '\0';
                index = i;
                client_count--;
            }
        }

        for(int i = index; i + 1 < 6; i++){
            memcpy(&clients[i], &clients[i+1], sizeof(clients[i+1]));
            // clients[i].socket = clients[i+1].socket;
            // clients[i].name = clients[i+1].name;
        }
        clients[5].socket = 0;
        clients[5].name[0] = '\0';


        // for (int i = 0; i < 6; i++){
        //     if(clients[i].name[0] != '\0'){
        //         printf("Current online user：fd = %s\n", clients[i].name);
        //     }
        // }
    }
}

void init(){

    serverfd = socket(PF_INET,SOCK_STREAM,0);

    if (serverfd == -1){
        perror("Creatling socket failed!");
        exit(-1);
    }


    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);


    if (bind(serverfd,(meng*)&addr,sizeof(addr)) == -1){
       perror("Binding failed!");
       exit(-1);
    }

    if (listen(serverfd,100) == -1){
        perror("Listening failed!");
        exit(-1);
    }

}

void Send_ALL(){
    char info[100] = {};
    info[0] = '\0';
    strcat(info, protocol.INFO);
    strcat(info, "Server shutdown in 5 minutes");
    strcat(info, protocol.INFO_END);
    for (int i = 0; i < 6; i++)
    {
        if(clients[i].name[0] != '\0'){
            send(clients[i].socket, info, strlen(info), 0);
        }
    }
}

void* server_thread(void* p){

    int fd = *(int*)p;

    printf("socket: %d\n", fd);

        while (1)
        {

            char buf[1000] = {};

            int flag = recv(fd, buf, sizeof(buf), 0);

            // printf("Recv: %s\n", buf);
            if(strstr(buf, "<LOGIN_LIST>") != NULL){

                printf("Recv: %s\n\n", buf);
                Login_List_Pro(&fd);

            } else if(strstr(buf, "<ENCODE>CRC</ENCODE>") != NULL){

                printf("Recv: %s\n", buf);
                // MSG_Pro_CRC(buf);
                MSG_Pro_CRC_Node(buf);
            } else if(strstr(buf, "<ENCODE>HAM</ENCODE>") != NULL){

                printf("Recv: %s\n", buf);
                MSG_Pro_HAM_Node(buf);
            } else if (strstr(buf, "<LOGOUT>") != NULL || flag <= 0)
            {

                if(strstr(buf, "<LOGOUT>") != NULL){
                    printf("Recv: %s\n", buf);
                    Logout_Pro(buf);
                } else {
                    printf("%d logout\n", fd);
                    for (int i = 0; i < 6; i++){
                        if(fd == clients[i].socket){
                            clients[i].socket = 0;
                            clients[i].name[0] = '\0';
                        }
                    }
                }
                
                pthread_exit(0);
            }else {
                printf("%s\n",buf);
            }
        }
}

void server(){

    printf("Starts Server\n");

    while(1){

        struct sockaddr_in fromaddr;

        socklen_t len = sizeof(fromaddr);

        // printf("Waiting for client to connect...\n");
        int fd = accept(serverfd,(meng*)&fromaddr,&len);


        if (fd == -1){
            continue;
        }

        int i = 0;

        // printf("#Log- Client connect, fd: %d\n", fd);

        //check Id
        char id[30] = {};
        recv(fd, id, sizeof(id), 0);
        // char id[30];
        // strcpy(id, temp);
        
        printf("Recv: %s\n", id);
        sleep(1);
        LogIn_Pro(id);
        int login = check_id(id);
        char loginFlag[2];
        snprintf(loginFlag, sizeof(loginFlag), "%d", login); 
        send(fd, loginFlag, strlen(loginFlag), 0);

        // printf("Login #: %d\n", login);
        // valid Id
        if(login){
            // connect_Node();
            char info[100] = {};
            info[0] = '\0';

            strcat(info, protocol.INFO);
            strcat(info, id);
            strcat(info, protocol.INFO_END);
            printf("Info: %s\n", info);

            strcpy(clients[client_count].name, id);
            clients[client_count].socket = fd;
            clients[client_count].address = fromaddr;
            client_count++;
            pthread_t tid;
            pthread_create(&tid, 0, server_thread, &fd);
        } else {
            // printf("\n id is invalid so close socket...\n");
            close(fd);
        }

    }

    close(serverfd);
}

int main(){
    init();
    server();
}
