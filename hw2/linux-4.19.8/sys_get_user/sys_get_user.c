#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>


#define  DEVICE_NAME "/dev/phone_book"
#define  COMMAND_SIZE 128

struct user_data {
    char first_name[16];
    char last_name[16];
    char phone_number[16];
    char email[32];
    char age[4];
};

asmlinkage long sys_get_user(const char *surname, unsigned int len, struct user_data *output) {
    int fd;
    char command[COMMAND_SIZE] = "get ";
    char result[COMMAND_SIZE];
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);
    
    fd = ksys_open(DEVICE_NAME, 0, 10); // 10 = a+

    strncat(command, surname, len);

    ksys_write(fd, command, COMMAND_SIZE);
    ksys_read(fd, result, COMMAND_SIZE);

    sscanf(result, "%s %s %s %s %s",
            output->first_name,
            output->last_name,
            output->phone_number,
            output->email,
            output->age);

    ksys_close(fd);
    
    set_fs(old_fs);
    
    return 0;
}
