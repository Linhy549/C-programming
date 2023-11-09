#include <stdio.h>

int main() {
    FILE *file = fopen("set.txt", "rb"); // Open the file in binary mode

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the data
    char data[file_size + 1]; // +1 for null terminator

    // Read the file into the data array
    fread(data, sizeof(char), file_size, file);
    data[file_size] = '\0'; // Null-terminate the data

    // Close the file
    fclose(file);

    // Print or use the read data
    printf("Read data:\n%s\n", data);

    return 0;
}
