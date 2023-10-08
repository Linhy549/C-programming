#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "encDec.h"

int countCharInFile(char fileName[]){
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        perror("Error opening the file");
        return 1;
    }

    int count = 0;
    int c;

    while((c = getc(file)) != EOF){
        // printf("%c###", c);
        count++;
    }

    fclose(file);
    return count;
}

int readFile(int frameSize, char fileName[]){
    int charInFile = countCharInFile(fileName);
    int leftBlocks = charInFile % frameSize;

    FILE *binFile = fopen("filename.binf", "wb");
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        perror("Error opening the file");
        return 1;
    }

    int c;
    char dataArr[64];
    int index = 0;
    int count = 1;
    while((c = getc(file)) != EOF){
        dataArr[index] = (char)c;
        index++;

        if(index == 64){
            dataArr[index] = '\0';
            index = 0;
            char buffer[600];
            frameData(64, dataArr, buffer);

            // printf("1Frame %d: ", count);
            count++;
            for(char *ptr = buffer; *ptr != '\0'; ++ptr){
                fwrite(ptr, sizeof(char), 1, binFile);
                printf("%c", *ptr);
            }
            printf("\n");
        }
    }

    dataArr[index] = '\0';
    // printf("\n\n%s", dataArr);
    char buffer[600];
    frameData(leftBlocks, dataArr, buffer);

    // printf("Frame %d: ", count);
    // for(char *ptr = buffer; *ptr != '\0'; ++ptr){
    //     printf("%c", *ptr);
    //     fwrite(*ptr, sizeof(char), leftBlocks, binFile);
    // }
    // printf("#2 Ready to write to file...\n");
    // fwrite(buffer, sizeof(char), leftBlocks * 8 + 24, binFile);
    int j = 0;
    while(buffer[j]!= '\0'){
        fputc(buffer[j], binFile);
        j++;
    }
    printf("\n");

    // free(buffer);
    fclose(binFile);
    fclose(file);
    
    return 0;
}

int main(int argc, char *argv[]){ 
    // printf("IN appl");
    readFile(64, argv[1]);
}

