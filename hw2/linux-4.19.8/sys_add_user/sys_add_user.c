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

asmlinkage long sys_add_user(struct user_data *user) {
    int fd;
    char command[COMMAND_SIZE];
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = ksys_open(DEVICE_NAME, 0, 10); // 10 = a+

    sprintf(command, "add %s %s %s %s %s",
            user->first_name,
            user->last_name,
            user->phone_number,
            user->email,
            user->age);
    
    ksys_write(fd, command, COMMAND_SIZE);
    ksys_close(fd);
    
    set_fs(old_fs);
    
    return 0;
}
