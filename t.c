#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 10

// Node structure for the queue
typedef struct Node {
    char* data;
    struct Node* next;
} Node;

// Queue structure
typedef struct {
    Node* array[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Queue;

// Function to initialize the queue
void initializeQueue(Queue* queue) {
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) {
        queue->array[i] = NULL;
    }

    queue->head = -1;
    queue->tail = -1;
}

// Function to check if the queue is empty
int isEmpty(Queue* queue) {
    return queue->head == -1;
}

// Function to enqueue a string
void enqueue(Queue* queue, const char* data) {
    if (isEmpty(queue)) {
        queue->head = 0;
        queue->tail = 0;
    } else {
        queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;

        // Check if the queue is full
        if (queue->tail == queue->head) {
            printf("Queue is full. Cannot enqueue.\n");
            queue->tail = (queue->tail - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
            return;
        }
    }

    // Allocate memory for the string and copy it into the queue
    queue->array[queue->tail] = (Node*)malloc(sizeof(Node));
    queue->array[queue->tail]->data = strdup(data);
    queue->array[queue->tail]->next = NULL;
}

// Function to dequeue a string
void dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        return;
    }

    // Deallocate memory for the string and move the head to the next node
    free(queue->array[queue->head]->data);
    free(queue->array[queue->head]);
    queue->array[queue->head] = NULL;

    // Update the head
    if (queue->head == queue->tail) {
        // If there was only one element in the queue, reset head and tail
        queue->head = -1;
        queue->tail = -1;
    } else {
        queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    }
}

// Function to print the contents of the queue
void printQueue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue: ");
    int index = queue->head;
    do {
        printf("%s ", queue->array[index]->data);
        index = (index + 1) % MAX_QUEUE_SIZE;
    } while (index != (queue->tail + 1) % MAX_QUEUE_SIZE);

    printf("\n");
}

// Function to free the memory allocated for the queue
void freeQueue(Queue* queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
}

// Main function for testing the queue
int main() {
    Queue myQueue;
    initializeQueue(&myQueue);

    enqueue(&myQueue, "First");
    enqueue(&myQueue, "Second");
    enqueue(&myQueue, "Third");

    printQueue(&myQueue);

    dequeue(&myQueue);
    printQueue(&myQueue);

    enqueue(&myQueue, "Fourth");
    printQueue(&myQueue);

    freeQueue(&myQueue);

    return 0;
}
