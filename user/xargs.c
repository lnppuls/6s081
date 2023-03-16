#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUFSIZE 512
#define MAXARG 20
void run(char *proname,char **args){
    if(fork() == 0){
        exec(proname,args);
        exit(0);
    }
}
int main(int argc,char *argv[]){
    if(argc <= 2) exit(0);
    int num = 0;
    char buf[BUFSIZE];
    char *arguments[128];
    // char *proname = argv[1];
    char **args = arguments;
    for(int i = 2;i < argc;i++){
        *args = argv[i];
        args++;
        num++;
    }
    char lobuf[BUFSIZE];
    char * p = lobuf;
    int n;
    while((n = read(0,buf,sizeof(buf)),n > 0)){
        for(int i = 0;i < n;i++) *(p++) = buf[i];
    }
    int i = 0,j;
    char now[128];
    while(1){
        if(buf[i-1] == '\n') break;
        for(j = 0;(lobuf[i] != ' ' )&&( lobuf[i] != '\0')&&(lobuf[i] != '\n');i++,j++) now[j] = lobuf[i];
        now[j] = '\0';
        // printf("now:%s\n",now);
        *args= now;
        args++;
        // printf("args:%s,len:%d\n",*(args-1),strlen(*(args-1)));
        i++;
        num++;
    }
    printf("num = %d,len = %d\n",num,strlen(*arguments));
    wait(0);
    exit(0);
}
    
    // for(int i = 2; i < argc;i++)
    //     strcpy(arguments[++num],argv[i]);
    // 
    // *(p-1) = '\0';
    // // for(int i = 0;i < 20;i++) printf("i:%d,%c\n",i,lobuf[i]);
    // 
    // char *prop[] = arguments;
    // run(argv[1],&prop);
    // while (wait(0) != -1)
    // {
        
    // }
    
    // exit(0);