#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>

// Node structure for a linked list
typedef struct Node {
    char data;
    struct Node* next;
} Node;

// Structure for the queue
typedef struct {
    int current;
    Node* front;
    Node* rear;
} Queue;


/* Node server config */
int fd;
int nodefd;
char* IP = "127.0.0.1";
short PORT = 6667;
typedef struct sockaddr meng;
int client_count = 0;
time_t nowtime;

/*queue*/
char temp[100];
int flagEnd = 0;
Queue q1, q2, q3, q4, q5;
sem_t s1, s2, s3, s4, s5;

void node_server();
void serverDecoder(char data[]);
void init();
int isMax(Queue *q);
int isQueueEmpty(Queue *q);
char dequeue(Queue *q);
void enqueue(Queue *q, char data);
void initializeQueue(Queue *q);
void *writer(void *arg);
void replaceChar(Queue q, char target);

void *charA(void *arg){
    while(q1.current != 0){
        printf("\n A \n");
        sem_wait(&s1);
        replaceChar(q1, 'a');
        sem_post(&s1);
    }
    pthread_exit(NULL);
}

void* charE(void* arg) {

    while(q2.current != 0){
        printf("\n E \n");
        sem_wait(&s2);
        replaceChar(q2, 'e');
        sem_post(&s2);
    }
    pthread_exit(NULL);
}

void* charI(void* arg) {

    while(q2.current != 0){
        sem_wait(&s2);
        replaceChar(q2, 'i');
        sem_post(&s2);
    }
    pthread_exit(NULL);
}

void* charO(void* arg) {

    while(q2.current != 0){
        sem_wait(&s2);
        replaceChar(q2, 'o');
        sem_post(&s2);
    }
    pthread_exit(NULL);
}

void* charU(void* arg) {

    while(q2.current != 0){
        sem_wait(&s2);
        replaceChar(q2, 'u');
        sem_post(&s2);
    }
    pthread_exit(NULL);
}

void replace(){
    for (int i = 0; temp[i] != '\0'; i++) {
        // Print "buffer: " at the beginning of each line
        if (i % 5 == 0) {
            printf("buffer: ");
        }

        // Convert vowels to uppercase
        char currentChar = temp[i];
        if (currentChar == 'a' || currentChar == 'e' || currentChar == 'i' || currentChar == 'o' || currentChar == 'u') {
            currentChar = toupper(currentChar);
        }

        printf("%c", currentChar);
        sleep(0.6);
        // Print a newline every 5 characters
        if ((i + 1) % 5 == 0) {
            printf("\n");
        }
    }

    // Print a newline if the last line is not complete
    if (strlen(temp) % 5 != 0) {
        printf("\n");
    }
}

void printInfo(){
    replace();
    // printf("Curretn buffer data has been modify: %s\n", temp);
}

void replaceChar(Queue q, char target) {
    char c = q.front->data;
    if(target == c && target == 'a'){
        c = 'A';
    } else if (target == c && target == 'e'){
        c = 'E';
    } else if (target == c && target == 'i'){
        c = 'I';
    } else if (target == c && target == 'o'){
        c = 'O';
    } else if (target == c && target == 'u'){
        c = 'U';
    }
    q.front->data = c;

}

int serverEncode(char buf[]){
    send(fd, buf, strlen(buf), 0);
    return 1;
}

void* writer(void* arg) {
    int isDone = 0;
    while (!isDone){
        isDone = serverEncode(temp);
    }

    pthread_exit(NULL);
}

void initializeQueue(Queue* q) {
    q->front =  NULL;
    q->rear = NULL;
    q->current = 0;
}

void enqueue(Queue* q, char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->current = q->current + 1;
}

char dequeue(Queue* q) {
    if (q->front == NULL) {
        // Queue is empty
        return '\0';
    }

    char data = q->front->data;
    Node* temp = q->front;

    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->current = q->current - 1;
    return data;
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

int isMax(Queue* q){
    if(q->current == 5){
        return 1;
    } else {
        return 0;
    }
}

void init(){

    nodefd = socket(PF_INET,SOCK_STREAM,0);

    if (nodefd == -1){
        perror("Creatling socket failed!");
        exit(-1);
    }


    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);


    if (bind(nodefd,(meng*)&addr,sizeof(addr)) == -1){
       perror("Binding failed!");
       exit(-1);
    }

    if (listen(nodefd,100) == -1){
        perror("Listening failed!");
        exit(-1);
    }

}

void serverDecoder(char data[]){
    printf("Server Decoder: %s\n", data);
    
    strcat(temp, data);

    // Size of the buffer
    const int bufferSize = 5;

    // Calculate the length of the data string
    int dataLength = strlen(data);

    // Queue for the buffer
    initializeQueue(&q1);
    initializeQueue(&q2);
    initializeQueue(&q3);
    initializeQueue(&q4);
    initializeQueue(&q5);

    
    // sem_init(&s1, 0, 1);
    // sem_init(&s2, 0, 1);
    // sem_init(&s3, 0, 1);
    // sem_init(&s4, 0, 1);
    // sem_init(&s5, 0, 1);
    

    char final[strlen(data)];
    final[0] = '\0';
    int in = 0;
    // Loop through the data string
    for (int i = 0; i < dataLength; i++) {
        // Enqueue characters into the buffer


        // If the buffer is full or we reach the end of the data string
        if (q1.current == 5 || i == dataLength - 1) {
            // Print the buffer
            // printf("Buffer: ");

            while (!isQueueEmpty(&q1)) {
                char c = dequeue(&q1);
                temp[in] = c;
                in++;
            }
            sem_init(&s1, 0, 1);
            sem_init(&s2, 0, 1);
            sem_init(&s3, 0, 1);
            sem_init(&s4, 0, 1);
            sem_init(&s5, 0, 1);
            pthread_t p1, p2, p3, p4, p5, p6;
            pthread_create(&p1, NULL, charA, NULL);
            pthread_create(&p2, NULL, charE, NULL);
            pthread_create(&p3, NULL, charI, NULL);
            pthread_create(&p4, NULL, charO, NULL);
            pthread_create(&p5, NULL, charU, NULL);
            pthread_create(&p6, NULL, writer, NULL);

            pthread_join(p1, NULL);
            pthread_join(p2, NULL);
            pthread_join(p3, NULL);
            pthread_join(p4, NULL);
            pthread_join(p5, NULL);
            pthread_join(p6, NULL);

            sem_destroy(&s1);
            printInfo();
            in = 0;
            strcat(final, temp);
        }
    }
    flagEnd = 1;
    temp[0] = '\0';
    // printf("Final : %s\n", final);

}

void node_server(){
    printf("Starts Node Server\n");

    while(1){

        struct sockaddr_in fromaddr;

        socklen_t len = sizeof(fromaddr);

        // printf("Waiting for client to connect...\n");
        fd = accept(nodefd,(meng*)&fromaddr,&len);


        if (fd == -1){
            continue;
        }

        int i = 0;


        //check Id
        char data[30] = {};
        recv(fd, data, sizeof(data), 0);
        // char id[30];
        // strcpy(id, temp);
        
        // printf("Recv: %s\n", data);
        serverDecoder(data);
        temp[0] = '\0';
        close(fd);
    }

    close(nodefd);
    // printf("Recv: %s\n", id);
}

int main() {
    init();
    node_server();
}
