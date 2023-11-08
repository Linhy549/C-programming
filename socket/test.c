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


struct Protocol {
    char LOG[6]; // Size includes space for the null-terminator
    char LOG_END[8];
    
    char MSG[6];
    char MSG_END[8];
    
    char LOGOUT[8];
    char LOGOUT_END[10];
    
    char LOGIN_LIST[12];
    char LOGIN_LIST_END[14];
    
    char INFO[6];
    char INFO_END[8];
    
    char TO[5];
    char TO_END[7];
    
    char FROM[7];
    char FROM_END[9];
    
    char BODY[7];
    char BODY_END[9];
} protocol = {"<LOG>", "</LOG>",
              "<MSG>", "</MSG>",
              "<LOGOUT>", "</LOGOUT>",
              "<LOGIN_LIST>", "</LOGIN_LIST>",
              "<INFO>", "</INFO>",
              "<TO>", "</TO>",
              "<FROM>", "</FROM>",
              "<BODY>", "</BODY>",
 };

int main(){
    char data[] = "t66577";
    char final[5];
    final[0] = '\0';
    strcat(final, protocol.LOG);
    strcat(final, data);
    strcat(final, protocol.LOG_END);

    printf("total size data : %zu\n", strlen(data) + strlen(protocol.LOG) + strlen(protocol.LOG_END));
    printf("final data : %s\n", final);
    return 0;
}