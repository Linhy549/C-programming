#ifndef ENCDEC_H
#define ENCDEC_H

/* Physical layer*/
void charToBinary(char c, char binaryArray[]);

void addParity(char binaryArray[]);

void sizeToBit(int size, char sizeArr[]);

/* Data link layer*/
void frameData(int size, char dataArr[], char output[]);

void deframeData(char arr[]);

void errorInsert(char arr[]);

char* getFrameData(char fileName[]);

void generateCRC(char data[], char crc[], char divisor[]);

int detectCRC(char received[], char divisor[]);

/* Error module - to generate an error bit*/
int randomNum(int left, int right);

/* Application layer*/
int countCharInFile(char fileName[]);

int readFile(int frameSize, char fileName[]);

#endif
