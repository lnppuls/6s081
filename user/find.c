#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char * path,char *filename){
    int fd;
    char buf[512],*p;
    struct dirent de;
    struct stat st;
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
  switch(st.type){
  case T_FILE:
    // printf("%s    %d\n%s    %d\n",fmtname(path),strlen(fmtname(path)),filename,strlen(filename));
    if(strcmp(path + strlen(path) - strlen(filename),filename) == 0){
        printf("%s\n",path);
    }
    // else printf("no:  %s\n",fmtname(path));
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if((strcmp("/.",buf + strlen(buf) - 2) != 0)&& (strcmp("/..",buf + strlen(buf) - 3) != 0)){       //every dir has its own . and ..
            // printf("get!\n");
            find(buf,filename);
        }
    }
    break;
  }
  close(fd);
}


int main(int argc,char *argv[]){
    if(argc != 3){
        printf("error!\n");
        exit(0);
    }
    // printf("%s\n",argv[2]);
    find(argv[1],argv[2]);
    exit(0);
}