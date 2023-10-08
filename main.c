#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int pid = fork();
    if( pid == 0 ){ 
        printf("Child!!!!\n");
        
        execl("service", "service", "file.txt", NULL );
    }
    else if( pid > 0){
        wait(NULL);
        printf("Parent!!!!\n");
    }

    else printf(" No fork this time! \n");
}