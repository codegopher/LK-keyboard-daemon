#define BSIZE 96 // 6*16 - три строки по 16 на нажатие, три - на отпускание клавиши
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// rm output.txt ; gcc -std=c99  daemon.c && ./a.out - тест этой программы

// Как преобразовывать код в имя клавиши?
// Надо бы собрать все это как демона

int main(){
  FILE *kbdfile;
  FILE *output;
  char* path = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
  kbdfile = fopen(path,"rb"); // файл с событиями клавиатуры, только для чтения
  char buf[BSIZE];
  long int keys[256] = {0}; // memset(keys,0,256);
  long int count = 0;
  int start = time(NULL); // Время начала отсчета

  while(1){
    fread(buf, BSIZE, 1, kbdfile);
    // printf("Pressed %d\n",buf[12]);
    keys[buf[12]+128]++; // Число нажатий данной клавиши
    count++; // Общее число нажатий

    if (difftime(time(NULL),start)>=3.0) {
      puts("\n3 seconds passed, writing...");
      output = fopen("output.txt","ab+");
      fprintf(output,"\n_______\n %d pressures \n", count);
      for (int i=0;i<256;i++) {
        if(keys[i]!=0) {fprintf(output," #%.3d:%.3d ",i,keys[i]);}
      }
      fclose(output);
      memset(keys,0,sizeof(keys));
      count = 0;
      start = time(NULL); // Время начала отсчета
      //break;
    } 

  }
  return 0;
}