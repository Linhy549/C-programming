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
            char buffer[600] = "";
            frameData(64, dataArr, buffer);
            printf("\nReady to write to file:\n");
                int j = 0;
                while(buffer[j]!= '\0'){
                    printf("%c", buffer[j]);
                    fputc(buffer[j], binFile);
                    j++;
                }
            printf("\n\n");
        }
    }

    dataArr[index] = '\0';
    // printf("\n\n%s", dataArr);
    char buffer[600] = "";
    frameData(leftBlocks, dataArr, buffer);

    printf("\nRead to write to file:\n");
    int j = 0;
    while(buffer[j]!= '\0'){
        printf("%c", buffer[j]);
        fputc(buffer[j], binFile);
        j++;
    }
    printf("\n\n");

    // free(buffer);
    fclose(binFile);
    fclose(file);
    
    return 0;
}

char* getFrameData(char fileName[]){
    int charInFile = countCharInFile(fileName);

    FILE *file = fopen(fileName, "r");
    char* buffer = (char*)malloc(charInFile * sizeof(char));
    int c;
    int index = 0;

    while((c = getc(file)) != EOF){
        buffer[index] = c;
        index++;
    }
    // int length = strlen(buffer)
    // printf("\n# of data in file: %s", strlen(buffer));
    
    return buffer;

}


