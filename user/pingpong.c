#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(){
    int ftop[2];
    int ptof[2];
    pipe(ftop);
    pipe(ptof);
    char buff[1];
    int pid = fork();
    if(pid < 0){
        printf("error!\n");
        exit(0);
    }
    if(pid == 0){       //son
        close(ftop[0]);
        close(ptof[1]);
        read(ftop[1],buff,1);
        printf("%d: received ping\n", getpid());
        write(ptof[0],buff,1);
        close(ftop[0]);
        close(ptof[1]);
        exit(0);
    }
    else if(pid >= 1){      //father
        close(ftop[1]);
        close(ptof[0]);
        write(ftop[0],"a",1);
        wait(0);
        read(ptof[1],buff,1);
        printf("%d: received pong\n", getpid());
        close(ftop[0]);
        close(ptof[1]);
    }
    return 0;
}