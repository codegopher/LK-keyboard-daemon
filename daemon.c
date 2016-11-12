#define BSIZE 96 // 6*16 - три строки по 16 на нажатие, три - на отпускание клавиши

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(){ 
  pid_t pid, sid;
  pid = fork();
  if (pid < 0) {
    puts("fork fail");
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    printf("Daemon pid: %d\n",pid);
    exit(EXIT_SUCCESS);
  }
  umask(0);
  sid = setsid();
  if (sid < 0) {
    puts("session fail");
    exit(EXIT_FAILURE);
  }
  if ((chdir("/")) < 0) {
    puts("chdir fail");
    exit(EXIT_FAILURE);
  }
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  FILE *kbdfile;
  FILE *output;
  char* path = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
  kbdfile = fopen(path,"rb"); // файл с событиями клавиатуры, только для чтения
  char buf[BSIZE];
  long int keys[256] = {0}; // Массив числа нажатий данной клавиши
  long int pcount = 0; // Число нажатий за период времени
  char wcount = 0; // Число записей в файл
  int start = time(NULL); // Время начала отсчета

  while(1){
    fread(buf, BSIZE, 1, kbdfile);
    keys[buf[12]+128]++; // Число нажатий данной клавиши
    pcount++; // Общее число нажатий

    if (difftime(time(NULL),start)>=30.0) {
      if (wcount == 5) {
        output = fopen("output.txt","w"); // Храним последние пять записей
        wcount = 0;
      }
      else {
         output = fopen("output.txt","ab+");
      }
      fprintf(output,"\n_______\n %d pressures \n", pcount);
      for (int i=0;i<256;i++) {
        if(keys[i]!=0) {fprintf(output," #%.3d:%.3d ",i,keys[i]);}
      }
      fclose(output);
      wcount++;
      memset(keys,0,sizeof(keys));
      pcount = 0;
      start = time(NULL); // Время начала отсчета
    } 

  }
 exit(EXIT_SUCCESS);
}