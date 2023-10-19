#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "encDec.h"

//Test
int main(int argc, char *argv[]){
    int first_pipe[2];

    pipe(first_pipe);
    int pid = fork();
    if( pid == 0 ){ 
        close(first_pipe[0]);
        printf("Child!!!!\n");
        
        // execlp("service", "service", "filename.inpf", NULL );
        readFile(64, "filename.inpf");
        char * buffer = getFrameData("filename.binf");

        int index = 0;
        int cur_index = 0;
        int length = strlen(buffer) - 2;
        char send_data [537];
        int count = 1;
        while(index < length){
            send_data[cur_index] = buffer[index];
            index++;
            cur_index++;

            // int count = 1;
            if(cur_index == 536){
                printf("\n#%d send data to pipe():\n", count);
                count++;
                send_data[cur_index] == '\0';
                write(first_pipe[1], send_data, 537);
                printf("%s\n", send_data);
                cur_index = 0;
            }     
            if(index == length){
                printf("\nLast frame and index: %d\n", cur_index);
                printf("\nSIZE :%d\n", cur_index + 1);
                send_data[cur_index] == '\0';
                
                write(first_pipe[1], send_data, cur_index);
                printf("%s\n", send_data);
            }  
        }
        // execl("service", "service", "filename.inpf", NULL );
        close(first_pipe[1]);
        printf("Child!!!!\n");
    }
    else if( pid > 0){
        wait(NULL);

        close(first_pipe[1]);
        char recieve_data[537];
        int bytesData;
        int i = 1;
        while(( bytesData = read(first_pipe[0], recieve_data, sizeof(recieve_data))) > 0){
            int j = 0;
            printf("\n%d data recieve - write:\n",i);
            write(STDERR_FILENO, recieve_data, bytesData);
            printf("\n%d data recieve - buffer:\n",i);
            printf("%s", recieve_data);
            // while(recieve_data[j] != '\0'){
            //     printf("%c",recieve_data[j]);
            //     j++;
            // }
            // printf("%d", bytesData);
            printf("\n");
            i++;
        }
        // read(first_pipe[0], recieve_data, sizeof(recieve_data));
        printf("Parent!!!!\n");
    }

    else printf(" No fork this time! \n");
}