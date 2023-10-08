#include <stdio.h>
#include <stdlib.h>
#include <time.h>
char binaryToChar(char binaryArray[]);
void charToBinary(char c, char binaryArray[]);
void addParity(char binaryArray[]);
void frame(int size, char dataArr[]);
void removeParity(char binaryArray[]);
void sizeToBit(int size, char sizeArr[]);
/////////////////////////////////////////
char binaryToChar(char binaryArray[]){
    printf("The data: %s\n", binaryArray);
    char c = 0;
    for(int i = 0; i < 8 ; i++){
        c = (c << 1) | (binaryArray[i] - '0');
    }
    // char c = strtol(binaryArray, NULL, 2);
    printf("char is : %c\n", c);
    return c;
}

void charToBinary(char c, char binaryArray[]){
    for(int i = 7; i >= 0; i--){
        binaryArray[7 - i] = (c & (1 <<i )) ? '1' : '0';
    }
    binaryArray[8] = '\0';
}

void addParity(char binaryArray[]){
    int count = 0;
    for(int i = 0; i <= 7; i++){
        if(binaryArray[i] == '1') count++;
    }
    if(count % 2 == 0) binaryArray[0] = '1';
}

// dataArr[]
void frame(int size, char dataArr[]){

    char syn []= "0001011000010110";

    char sizeArr[9];
    sizeToBit(size, sizeArr);


    int pre_size = 16 + 8;
    int index = 0;
    char buffer[600];
    printf("SYS + SIZE: \n");
    // printf("Buffer size: %zu\n", strlen(buffer));
    for(int i = 0; i < 16; i++){
        buffer[index] = syn[i];
        printf("%c", buffer[index]);
        index++; 
    }

    for(int i = 0; i < 8 ; i++){
        buffer[index] = sizeArr[i];
        printf("%c", buffer[index]);
        index++;
    }
    // printf("1.Buffer size after: %zu\n", strlen(buffer));
    // int num1 = randomNum(0, size - 1);
    // int num2 = randomNum(0, 7);
    printf("\nActural data: \n");
    for(int i = 0; i < 600; i++){
        if(dataArr[i] != '\0') {
            buffer[index] = dataArr[i];
            printf("%c", buffer[index]);
            index++;
        }
    }
    printf("\nCUrrent index: %d\n", index);
    buffer[index] = '\0';
    index = 0;
    printf("\n\n");
    


}

void removeParity(char binaryArray[]){
    binaryArray[0] = '0';
}

void sizeToBit(int size, char sizeArr[]){
    sizeArr[8] = '\0';
    for(int i = 7; i >= 0; i--){
        sizeArr[7 - i] = ((size & (1 << i)) ? '1' : '0');
    }
}

int readFile(int frameSize, char fileName[]){

    FILE *binFile = fopen("filename.binf", "wb");
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        perror("Error opening the file");
        return 1;
    }

    int c;


    // fwrite(ptr, sizeof(char), 1, binFile);
    while((c = getc(file)) != EOF){
        char charArr[9];    
        charToBinary(c , charArr);
        fputs(charArr, binFile);
        
    }

    fclose(binFile);
    fclose(file);
    
    return 0;
}

void converBack(char fileName[]){
    FILE *binFile = fopen("filename.outf", "wb");
    FILE *file = fopen(fileName, "r");

    int index = 0;
    char dataArr[8];
    int c;
    while((c = getc(file)) != EOF){
        // char temp;
        dataArr[index] = c;
        index++;
        if(index == 8){
            printf("Read data: %s", dataArr);
            char temp = binaryToChar(dataArr);
            index = 0;
            fputc(temp, binFile);
        }
    }
    fclose(binFile);
    fclose(file);
}

int readAndConvert(char fileName[]){
    FILE *binFile = fopen("framedata.txt", "wb");
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        perror("Error opening the file");
        return 1;
    }

    int c;


    // fwrite(ptr, sizeof(char), 1, binFile);
    int count = 0;
    char dataArr[600];
    int index = 0;
    while((c = getc(file)) != EOF){
        dataArr[index] = c;
        index++;
        count++;
        if(count == 512){
            dataArr[index] = '\0';
            printf("Test, how many data pasing to frame: %d\n", index);
            frame(64, dataArr);
            index = 0;
            count = 0;
        }
    }

    dataArr[index] = '\0';
    printf("Test, how many data pasing to frame for last frame: %d\n", index);
    frame(count, dataArr);


    fclose(binFile);
    fclose(file);
    
    return 0;
}

int main(){
    // readFile(64, "file.txt");
    // converBack("filename.binf");
    readAndConvert("filename.binf");

}