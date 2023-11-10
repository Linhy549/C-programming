#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "encDec.h"


int randomNum(int left, int right){
    srand(((unsigned)time(NULL)) + (unsigned)rand());
    int a = rand() % (right - left + 1) + left;
    return a;
}

void frameData(int size, char dataArr[], char buffer[]){

    char syn [600]= "0001011000010110";

    char sizeArr[8];
    sizeToBit(size, sizeArr);

    strcat(syn, sizeArr);

    printf("\n\n");

    
    // printf("1.Buffer size after: %zu\n", strlen(buffer));
    int num1 = randomNum(0, size - 1);
    int num2 = randomNum(0, 7);

    for(int i = 0; i < size; i++){
        char binaryArray[9];
        charToBinary(dataArr[i], binaryArray);
        addParity(binaryArray);

        /*Error inset*/
        // if(num1 == i){
        //     if(binaryArray[num2] =='0') binaryArray[num2] =='1';
        //     else binaryArray[num2] ='0';
        //     printf("The error char is: %c\nThis char is in position of %d in current data frame\nThe index: %d has been modify\n", dataArr[i], num1, num2);
        // }
        
        strcat(syn, binaryArray);
        // printf("2.Buffer size after: %zu\n", strlen(buffer));

    }


    printf("\n\n");
    strcat(buffer, syn);

}


