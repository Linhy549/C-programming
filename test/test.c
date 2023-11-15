#include <stdio.h>

// Function to calculate the parity bits for a Hamming code
void calculateParityBits(int data[4], int parityBits[7]) {
    // Calculate parity bits
    parityBits[2] = data[0] ^ data[1] ^ data[3];
    parityBits[4] = data[0] ^ data[2] ^ data[3];
    parityBits[5] = data[1] ^ data[2] ^ data[3];

    // Calculate even parity for each bit position
    for (int i = 0; i < 7; i++) {
        int countOnes = 0;
        for (int j = 0; j < 7; j++) {
            if ((i & (1 << j)) != 0) {
                countOnes += parityBits[j];
            }
        }
        parityBits[i] = countOnes % 2;
    }
}

// Function to generate a Hamming code
void generateHammingCode(int data[4], int hammingCode[7]) {
    // Initialize parity bits to 0
    for (int i = 0; i < 7; i++) {
        hammingCode[i] = 0;
    }

    // Set data bits in the Hamming code
    hammingCode[2] = data[0];
    hammingCode[4] = data[1];
    hammingCode[5] = data[2];
    hammingCode[6] = data[3];

    // Calculate parity bits
    calculateParityBits(data, hammingCode);
}

// Function to detect and correct errors in a received Hamming code
int detectAndCorrectErrors(int receivedCode[7]) {
    // Calculate syndrome bits
    int syndrome[3];
    syndrome[0] = receivedCode[2] ^ receivedCode[4] ^ receivedCode[6];
    syndrome[1] = receivedCode[2] ^ receivedCode[5] ^ receivedCode[6];
    syndrome[2] = receivedCode[4] ^ receivedCode[5] ^ receivedCode[6];

    // Convert binary to decimal to find the error position
    int errorPosition = syndrome[0] + 2 * syndrome[1] + 4 * syndrome[2];

    // If errorPosition is not zero, correct the error
    if (errorPosition != 0) {
        printf("Error detected and corrected at bit position %d.\n", errorPosition);
        // Flip the incorrect bit
        receivedCode[errorPosition - 1] = !receivedCode[errorPosition - 1];
        return 1; // Error corrected
    } else {
        printf("No errors detected.\n");
        return 0; // No errors detected
    }
}

int main() {
    int data[4] = {1, 0, 1, 1}; // 4-bit message
    int hammingCode[7]; // 7-bit Hamming code

    // Generate Hamming code
    generateHammingCode(data, hammingCode);

    // Simulate an error by flipping a bit
    hammingCode[2] = !hammingCode[2];

    // Detect and correct errors
    int errorCorrected = detectAndCorrectErrors(hammingCode);

    // Display the original and corrected codes
    printf("Original Hamming Code: ");
    for (int i = 0; i < 7; i++) {
        printf("%d", hammingCode[i]);
    }
    printf("\n");

    if (errorCorrected) {
        printf("Corrected Hamming Code: ");
        for (int i = 0; i < 7; i++) {
            printf("%d", hammingCode[i]);
        }
        printf("\n");
    }

    return 0;
}
