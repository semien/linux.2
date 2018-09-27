#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE_FILE_NAME "/dev/phone_book"
#define COMMAND_SIZE 128
#define RESULT_SIZE 128

int main(int argc, char* argv[]){
    int fd;
    char pb_command[COMMAND_SIZE];
    char result[RESULT_SIZE];

    fd = open(DEVICE_FILE_NAME, O_RDWR);
    if (fd < 0) {
        printf ("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

    while(1){
        printf("> ");
        if(!fgets(pb_command, COMMAND_SIZE, stdin)){
          printf("command reading error\n");
          continue;
        } 
        if (pb_command[0]=='\0' || pb_command[0]=='\n') continue;
        if (!strncmp(pb_command, "exit", 4)){
            close(fd);
            return 0;
        }
        write(fd, pb_command, COMMAND_SIZE);
        read(fd, result, RESULT_SIZE);
        printf("%s\n",result);
    }
    close(fd);
    return 0;
}
