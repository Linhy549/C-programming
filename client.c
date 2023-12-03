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
#include "encDec.h"

# define ID_LENGTH 9
int clientfd2;
char client_name[30];
char *IP = "127.0.0.1"; 
short PORT = 6666;
typedef struct sockaddr meng;
char name[30];
time_t nowtime;

int hamming_flag = 1;
int crc_flag = 1;
int error_flag = 1;

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

void store_chat(char c1[], char c3[], char msg[]) {
    char c2[] = "Result";
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

void MSG_Pro_CRC(){

    char user_id[9] = {};
    user_id[0] = '\0';
    char msg[500];
    msg[0] = '\0';
    char frame[700] = {};
    frame[0] = '\0';


    printf("Enter user-id you want to chat: ");
    fgets(user_id, sizeof(user_id), stdin);
    fgets(user_id, sizeof(user_id), stdin);

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    fgets(msg, sizeof(msg), stdin);


    for (int i = 0; msg[i] != '\0'; i++) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
        }
    }

    char msg1[(int)strlen(msg) * 8 + 24 + 1];
    msg1[0] = '\0';
    frameData((int)strlen(msg), msg, msg1);

    

    char pre[25];
    char rem[(int)strlen(msg1) + 5 - 23];
    char final_data[(int)strlen(msg) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';
    if(crc_flag == 1){
        strncpy(pre, msg1, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, msg1 + 24);

        //
        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        generateCRC(rem, crc, divisor);
        printf("Generated CRC: %s\n", crc);
        // printf("pre: %s\n rem: %s\n", pre, rem);

        // Simulate transmission by appending CRC to data
        // strcat(rem, crc);

        // msg1[500];
        // msg1[0];
        strcat(final_data, pre);
        strcat(final_data, rem);
        strcat(final_data, crc);

    }


    strcat(frame, protocol.MSG);
    strcat(frame, protocol.ENCODE);
    strcat(frame, "CRC");
    strcat(frame, protocol.ENCODE_END);
    strcat(frame, protocol.MSG);
    strcat(frame, protocol.FROM);
    strcat(frame, client_name);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, user_id);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);
    if(error_flag){
        errorInsert(final_data);
        error_flag = 0;
    }
    // errorInsert(final_data);
    strcat(frame, final_data);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);

    // printf("data: %s\n", frame);
    send(clientfd2, frame, strlen(frame), 0);
}

void MSG_Pro_HAM(){

    char user_id[9] = {};
    user_id[0] = '\0';
    char msg[500];
    msg[0] = '\0';
    char frame[700] = {};
    frame[0] = '\0';


    printf("Enter user-id you want to chat: ");
    fgets(user_id, sizeof(user_id), stdin);
    fgets(user_id, sizeof(user_id), stdin);

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    fgets(msg, sizeof(msg), stdin);


    for (int i = 0; msg[i] != '\0'; i++) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
        }
    }

    char msg1[(int)strlen(msg) * 8 + 24 + 1];
    msg1[0] = '\0';
    frameData((int)strlen(msg), msg, msg1);

    

    char pre[25];
    char rem[(int)strlen(msg1) + 5 - 23];
    char final_data[(int)strlen(msg) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';
    if(crc_flag == 1){
        strncpy(pre, msg1, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, msg1 + 24);

        //
        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        generateCRC(rem, crc, divisor);
        // printf("Generated CRC: %s\n", crc);
        // printf("pre: %s\n rem: %s\n", pre, rem);

        // Simulate transmission by appending CRC to data
        // strcat(rem, crc);

        // msg1[500];
        // msg1[0];
        strcat(final_data, pre);
        strcat(final_data, rem);
        strcat(final_data, crc);

    }


    strcat(frame, protocol.MSG);
    strcat(frame, protocol.ENCODE);
    strcat(frame, "HAM");
    strcat(frame, protocol.ENCODE_END);
    strcat(frame, protocol.MSG);
    strcat(frame, protocol.FROM);
    strcat(frame, client_name);
    strcat(frame, protocol.FROM_END);
    strcat(frame, protocol.TO);
    strcat(frame, user_id);
    strcat(frame, protocol.TO_END);
    strcat(frame, protocol.BODY);
    if(error_flag){
        errorInsert_ham(final_data);
        error_flag = 0;
    }
    // errorInsert(final_data);
    strcat(frame, final_data);
    strcat(frame, protocol.BODY_END);
    strcat(frame, protocol.MSG_END);

    // printf("data: %s\n", frame);
    send(clientfd2, frame, strlen(frame), 0);
}

void MSG_Pro_Get_ham(char arr[]){
    char c1[20], c2[20], text[(int)strlen(arr)];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr)];
    frame[0] = '\0';

    if (fromStart != NULL && fromEnd != NULL) {
        strncpy(c1, fromStart + 6, fromEnd - (fromStart + 6));
        c1[fromEnd - (fromStart + 6)] = '\0'; // Null-terminate the string
    }

    if (bodyStart != NULL && bodyEnd != NULL) {
        strncpy(text, bodyStart + 6, bodyEnd - (bodyStart + 6));
        text[bodyEnd - (bodyStart + 6)] = '\0'; // Null-terminate the string
    }

    char pre[25];
    char rem[(int)strlen(text) + 5 - 23];
    char final_data[(int)strlen(text) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';

    if(crc_flag == 1){
        strncpy(pre, text, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, text + 24);

        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        // rem[sizeof(rem) - 6] = '1';
        int errorDetected = detectCRC(rem, divisor);
        // if (errorDetected) {
        //     printf("Error detected during CRC check!\n");
        // } else {
        //     printf("No error detected during CRC check.\n");
        // }

        // Remove crc from the end
        int length = strlen(rem);
        rem[length - 3] = '\0';

        // Conver binary to Char
        char buffer[length / 8 + 1];
        deframeData(rem, buffer);

        printf("%s to you: %s\n", c1, buffer);
        store_chat(c1, c2, buffer);
    }
}

void MSG_Pro_dataSet(){

    char temp[9] = {};
    temp[0] = '\0';
    printf("Enter user-id you want to chat: \n");
    fgets(temp, sizeof(temp), stdin);
    fgets(temp, sizeof(temp), stdin);
    
    char user_id[9] = {};
    strcpy(user_id, temp);
    printf("You have enter : %s\n", user_id);
    FILE *file = fopen("dataset.txt", "rb");
    if (file == NULL) {
        perror("Error opening file");
    }
    

    char msg[65];

    size_t bytesRead;

    while ((bytesRead = fread(msg, 1, 64, file)) > 0) {
        char frame[700] = {};
        frame[0] = '\0';
        msg[bytesRead] = '\0'; // Null-terminate the buffer

        char user_id[9] = {};
        user_id[0] = '\0';

        char msg1[(int)strlen(msg) * 8 + 24 + 1];
        msg1[0] = '\0';
        frameData((int)strlen(msg), msg, msg1);

        char pre[25];
        char rem[(int)strlen(msg1) + 5 - 23];
        char final_data[(int)strlen(msg) * 8 + 24 + 1 + 4];
        final_data[0] = '\0';

        strncpy(pre, msg1, 24);
        pre[24] = '\0'; // Null-terminate fix
        strcpy(rem, msg1 + 24);

        //
        char crc[10];  // Adjust the size based on your CRC polynomial
        char divisor[5] = "1101";
        generateCRC(rem, crc, divisor);
        printf("Generated CRC: %s\n", crc);
        // printf("pre: %s\n rem: %s\n", pre, rem);

        // Simulate transmission by appending CRC to data
        // strcat(rem, crc);

        // msg1[500];
        // msg1[0];
        strcat(final_data, pre);
        strcat(final_data, rem);
        strcat(final_data, crc);


        strcat(frame, protocol.MSG);
        strcat(frame, protocol.ENCODE);
        strcat(frame, "CRC");
        strcat(frame, protocol.ENCODE_END);
        strcat(frame, protocol.FROM);
        strcat(frame, client_name);
        strcat(frame, protocol.FROM_END);
        strcat(frame, protocol.TO);
        strcat(frame, temp);
        strcat(frame, protocol.TO_END);
        strcat(frame, protocol.BODY);
        strcat(frame, final_data);
        strcat(frame, protocol.BODY_END);
        strcat(frame, protocol.MSG_END);

        printf("data: %s\n", frame);
        send(clientfd2, frame, strlen(frame), 0);
        // Send the chunk via the socket
        // if (send(socket, buffer, bytesRead, 0) == -1) {
        //     perror("Error sending data");
        //     break;
        // }
        sleep(1);
    }
}

void MSG_Pro_Get_crc(char arr[]){
    char c1[20], c2[20], text[(int)strlen(arr)];
    const char *fromStart = strstr(arr, protocol.FROM);
    const char *fromEnd = strstr(arr,protocol.FROM_END);
    const char *toStart = strstr(arr, protocol.TO);
    const char *toEnd = strstr(arr, protocol.TO_END);
    const char *bodyStart = strstr(arr, protocol.BODY);
    const char *bodyEnd = strstr(arr, protocol.BODY_END);
    char frame[(int)strlen(arr)];
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

    char pre[25];
    char rem[(int)strlen(text) + 5 - 23];
    char final_data[(int)strlen(text) * 8 + 24 + 1 + 4];
    final_data[0] = '\0';

    if(crc_flag == 1){
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

        printf("%s to you: %s\n", c1, buffer);
        store_chat(c1, c2, buffer);
    }

    // printf("%s to you: %s\n", c1, rem);
}

void Logout_Pro(){
    char buff[50];
    buff[0] = '\0';
    strcat(buff, protocol.LOGOUT);
    strcat(buff, client_name);
    strcat(buff, protocol.LOGOUT_END);
    send(clientfd2, buff, strlen(buff), 0);
}

void* recv_thread(void* p){

    while(1){

        char buf[1000] = {};

        if (recv(clientfd2,buf,sizeof(buf),0) <= 0){
            break;
        }

        if(strstr(buf, "<LOGIN_LIST>") != NULL){
            LoginList_Pro(buf);
        } 

        if(strstr(buf, "<ENCODE>CRC</ENCODE>") != NULL){
            MSG_Pro_Get_crc(buf);
        }

        if(strstr(buf, "<ENCODE>HAM</ENCODE>") != NULL){
            MSG_Pro_Get_ham(buf);
        }


    }

}

int check_id(char *id){
    clientfd2 = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(clientfd2,(meng*)&addr,sizeof(addr)) == -1){

        exit(-1);

    }

    char buff[30];
    buff[0] = '\0';
    strcat(buff, protocol.LOGIN);
    strcat(buff, id);
    strcat(buff, protocol.LOGIN_END);
    send(clientfd2, buff, strlen(buff), 0);

    char loginFlag[2];
    recv(clientfd2,loginFlag,strlen(loginFlag),0);

    // printf("#Log- Recv loginFlag: %s\n", loginFlag);
    strcpy(client_name, id);

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

    pthread_t tid;
    printf("\nWelcome to 500-Chat\n");
    pthread_create(&tid, 0, recv_thread, &clientfd2);
    return 1;
}

int main(){
    int loop = 1;
    while(loop){
        printf("=== Welcome to One-One chat Application ===\n");
        printf("1. Login with CRC\n");
        printf("2. Login with Hamming \n");
        printf("3. Login with Hamming detection and correction\n");
        printf("4. Exit\n");
        printf("Please enter: ");

        int option;

        scanf("%d", &option);
        switch (option)
        {
            case 1:
                char id[ID_LENGTH]; // id 8-bits long
                printf("\nPlease enter your ID: ");
                scanf("%s", id);
                // printf("\nYour Id: %s\n", id);

                if(check_id(id)){

                    int loop2 = 1;

                    while(loop2) {
                        printf("=== User Id: %s ===\n", id);
                        printf("1. Show online users\n");
                        printf("2. Private-chat\n");
                        printf("3. Test-data set\n");
                        printf("4. Exit\n");
                        printf("Please enter: \n");

                        scanf("%d", &option);

                        switch (option)
                        {
                            case 1:
                                char buf[500] = {};
                                
                                strcat(buf, protocol.LOGIN_LIST);
                                strcat(buf, client_name);
                                strcat(buf, protocol.LOGIN_LIST_END);
                                // printf("socket : %d\n", clientfd2);
                                // printf("mes : %s\n", buf);
                                send(clientfd2, buf, strlen(buf), 0);
                                break;
                            case 2:
                                
                                MSG_Pro_CRC();
                                break;
                            case 3:
                                MSG_Pro_dataSet();
                                break;
                            case 4:
                                printf("exit\n");
                                Logout_Pro();
                                close(clientfd2);
                                loop2 = 0;
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    printf("\n * Please enter  valid id *\n");
                }
                break;

            case 2:
                id[ID_LENGTH]; // id 8-bits long
                printf("\nPlease enter your ID: ");
                scanf("%s", id);
                // printf("\nYour Id: %s\n", id);

                if(check_id(id)){

                    int loop2 = 1;

                    while(loop2) {
                        printf("=== User Id: %s ===\n", id);
                        printf("1. Show online users\n");
                        printf("2. Private-chat\n");
                        printf("3. Test-data set\n");
                        printf("4. Exit\n");
                        printf("Please enter: \n");

                        scanf("%d", &option);

                        switch (option)
                        {
                            case 1:
                                char buf[500] = {};
                                
                                strcat(buf, protocol.LOGIN_LIST);
                                strcat(buf, client_name);
                                strcat(buf, protocol.LOGIN_LIST_END);
                                // printf("socket : %d\n", clientfd2);
                                // printf("mes : %s\n", buf);
                                send(clientfd2, buf, strlen(buf), 0);
                                break;
                            case 2:
                                
                                MSG_Pro_HAM();
                                break;
                            case 3:
                                MSG_Pro_dataSet();
                                break;
                            case 4:
                                printf("exit\n");
                                Logout_Pro();
                                close(clientfd2);
                                loop2 = 0;
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    printf("\n * Please enter  valid id *\n");
                }
                break;
            case 3:     
                id[ID_LENGTH]; // id 8-bits long
                printf("\nPlease enter your ID: ");
                scanf("%s", id);
                // printf("\nYour Id: %s\n", id);

                if(check_id(id)){

                    int loop2 = 1;

                    while(loop2) {
                        printf("=== User Id: %s ===\n", id);
                        printf("1. Show online users\n");
                        printf("2. Private-chat\n");
                        printf("3. Test-data set\n");
                        printf("4. Exit\n");
                        printf("Please enter: \n");

                        scanf("%d", &option);

                        switch (option)
                        {
                            case 1:
                                char buf[500] = {};
                                
                                strcat(buf, protocol.LOGIN_LIST);
                                strcat(buf, client_name);
                                strcat(buf, protocol.LOGIN_LIST_END);
                                // printf("socket : %d\n", clientfd2);
                                // printf("mes : %s\n", buf);
                                send(clientfd2, buf, strlen(buf), 0);
                                break;
                            case 2:
                                
                                MSG_Pro_HAM();
                                break;
                            case 3:
                                MSG_Pro_dataSet();
                                break;
                            case 4:
                                printf("exit\n");
                                Logout_Pro();
                                close(clientfd2);
                                loop2 = 0;
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    printf("\n * Please enter  valid id *\n");
                }
                break;

            case 4:
                printf("Exit\n");
                loop = 0;
                break;
            default:
                printf("Invalid\n");
        }
    }


    return 0;

}
