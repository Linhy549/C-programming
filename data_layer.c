#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "encDec.h"


// int randomNum(int left, int right){
//     srand(((unsigned)time(NULL)) + (unsigned)rand());
//     int a = rand() % (right - left + 1) + left;
//     return a;
// }


void frameData(int size, char dataArr[], char buffer[]){
    char temp[500];
    char syn[] = "0001011000010110";

    char sizeArr[9];
    sizeToBit(size, sizeArr);

    strcat(temp, syn);
    strcat(temp, sizeArr);


    // int num1 = randomNum(0, size - 1);
    // int num2 = randomNum(0, 7);

    
    size_t totalSize = strlen(syn) + strlen(sizeArr) + 8 * size + 1; // +1 for null-terminator
    // printf("%d, %d, %d\n", (int)strlen(syn), (int)strlen(sizeArr), (8 * size + 1));

    // Create a buffer to hold the combined string
    char combined[totalSize];

    // Copy the strings into the combined buffer
    strcpy(combined, syn);
    strcat(combined, sizeArr);
    // strcat(combined, binaryArray);

    // printf("Combined string: %s\n", combined);
    for(int i = 0; i < size; i++){
        char binaryArray[9];
        charToBinary(dataArr[i], binaryArray);
        // addParity(binaryArray);

        /*Error inset*/
        // if(num1 == i){
        //     if(binaryArray[num2] =='0') binaryArray[num2] =='1';
        //     else binaryArray[num2] ='0';
        //     printf("The error char is: %c\nThis char is in position of %d in current data frame\nThe index: %d has been modify\n", dataArr[i], num1, num2);
        // }
        strcat(combined, binaryArray);


    }
    // printf("%s\n", combined);
    strcat(buffer, combined);
    combined[sizeof(combined) - 1] = '\0';
}

void deframeData(char arr[], char buffer[]){
    int length = strlen(arr);

    int j = 0;
    for (int i = 0; i < length; i += 8){
        char temp[9];
        strncpy(temp, arr + i, 8);
        temp[8] = '\0';
        char c = binToChar(temp);
        buffer[j] = c;
        j++;
    }

    buffer[j] = '\0';
}

// Function to perform CRC generation
void generateCRC(char input[], char crc[], char divisor[]) {
    char data[strlen(input) + 1]; // +1 for the null terminator
    strcpy(data, input);

    int dataLen = strlen(data);
    int divisorLen = strlen(divisor);
    
    // Append zeros to the data to match the length of the divisor
    for (int i = 0; i < divisorLen - 1; i++) {
        data[dataLen + i] = '0';
    }

    // Perform modulo-2 division
    for (int i = 0; i < dataLen; i++) {
        if (data[i] == '1') {
            for (int j = 0; j < divisorLen; j++) {
                data[i + j] = (data[i + j] == divisor[j]) ? '0' : '1';
            }
        }
    }

    // Copy the remainder (CRC) to the output buffer
    strncpy(crc, data + dataLen, divisorLen - 1);
    crc[divisorLen - 1] = '\0';
}

// Function to perform CRC detection
int detectCRC(char received[], char divisor[]) {
    int receivedLen = strlen(received);
    int divisorLen = strlen(divisor);

    // Create a buffer to hold the received data and CRC
    char temp[receivedLen + divisorLen];
    strcpy(temp, received);

    // Perform modulo-2 division
    for (int i = 0; i < receivedLen; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < divisorLen; j++) {
                temp[i + j] = (temp[i + j] == divisor[j]) ? '0' : '1';
            }
        }
    }

    // Check if there is a remainder
    for (int i = receivedLen; i < receivedLen + divisorLen - 1; i++) {
        if (temp[i] == '1') {
            return 1; // Error detected
        }
    }

    return 0; // No error detected
}

void errorInsert_ham(char arr[]) {
    char temp[(int)strlen(arr)];
    strcpy(temp, arr);
    size_t length = strlen(arr);

    printf("\n\n Before Insert Error Bit: %s\n", arr);
    // Flip one random bit after the first 24 bits
    srand(time(NULL));
    size_t flipIndex = 24 + rand() % (length - 24);
    temp[flipIndex] = (temp[flipIndex] == '0') ? '1' : '0';

    // Print the index where the bit has been flipped
    printf("Bit flipped at index within Data block: %zu\n", flipIndex - 24);
    printf("\n After Insert Error Bit: %s\n", temp);
}

void errorInsert(char arr[]) {
    size_t length = strlen(arr);


    // Flip one random bit after the first 24 bits
    srand(time(NULL));
    size_t flipIndex = 24 + rand() % (length - 24);
    arr[flipIndex] = (arr[flipIndex] == '0') ? '1' : '0';

    // Print the index where the bit has been flipped
    printf("Bit flipped at index within Data block: %zu\n", flipIndex - 24);
}