#ifndef ENCDEC_H
#define ENCDEC_H

/* Physical layer*/
void charToBinary(char c, char binaryArray[]);

void addParity(char binaryArray[]);

void sizeToBit(int size, char sizeArr[]);

/* Data link layer*/
void frameData(int size, char dataArr[], char output[]);

char* getFrameData(char fileName[]);
/* Error module - to generate an error bit*/
int randomNum(int left, int right);

/* Application layer*/
int countCharInFile(char fileName[]);

int readFile(int frameSize, char fileName[]);

#endif
