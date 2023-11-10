#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "encDec.h"

void charToBinary(char c, char binaryArray[]){
    for(int i = 7; i >= 0; i--){
        binaryArray[7 - i] = (c & (1 <<i )) ? '1' : '0';
    }
    binaryArray[8] = '\0';
}

void addParity(char binaryArray[]){
    int count = 0;
    for(int i = 1; i <= 7; i++){
        if(binaryArray[i] == '1') count++;
    }
    if(count % 2 == 0) binaryArray[0] = '1';
}

void sizeToBit(int size, char sizeArr[]){
    sizeArr[8] = '\0';
    for(int i = 7; i >= 0; i--){
        sizeArr[7 - i] = ((size & (1 << i)) ? '1' : '0');
    }
    
}

