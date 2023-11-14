#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

int main() {
    char name[] = {"00010110000101100000010011000010010000110100001111000001"};
    char pre[25];
    char rem[(int)strlen(name) + 5 - 23];
    strncpy(pre, name, 24);
    pre[24] = '\0'; // Null-terminate fix
    strcpy(rem, name + 24);

    // printf("%s\n", pre);
    printf("%s\n", pre);
    printf("%s\n", rem);
    return 0;
}
