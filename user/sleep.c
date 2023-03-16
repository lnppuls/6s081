#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if(argc == 1){
      printf("no ticks!\n");
      exit(0);  
    } 
    else if(argc > 2){
        printf("too many ticks!\n");
        exit(0);
    }
    int n;
    n = atoi(argv[1]);
    printf("%d\n",n);
    sleep(n);
    printf("over and nothing!\n");
    exit(0);
}
