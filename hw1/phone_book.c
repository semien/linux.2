#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Semien Ishkhanyan");
MODULE_DESCRIPTION("Kernel-mode phone book.");

#define DEVICE_NAME "phone_book"
#define COMMAND_SIZE 128
#define RESULT_SIZE 128

static int device_open_count = 0;
static int device_major_number;
static char pb_command[COMMAND_SIZE] = "example command";
static char result[RESULT_SIZE] = "example result";

struct record {
    char first_name[16];
    char last_name[16];
    char phone_number[16];
    char email[32];
    char age[4];
    struct list_head list;
};

struct record records_list;

static void find_info(void) {
    int num;
    char* command = pb_command + 5;
    char input_last_name[16];
    struct record* cur_rec;
    strcpy(input_last_name, command);
    input_last_name[strlen(input_last_name) - 1] = '\0';

    num = 0;
    result[0] = '\0';
    list_for_each_entry(cur_rec, &records_list.list, list) {
        if (!strcmp(cur_rec->last_name, input_last_name)) {
            ++num;
            strcat(result, cur_rec->first_name);
            strcat(result, " ");
            strcat(result, cur_rec->last_name);
            strcat(result, " ");
            strcat(result, cur_rec->phone_number);
            strcat(result, " ");
            strcat(result, cur_rec->email);
            strcat(result, " ");
            strcat(result, cur_rec->age);
            strcat(result, "\n");
            printk(KERN_INFO "info record: name: %s, lastname: %s, phone: %s, email: %s, age: %s \n", 
                cur_rec->first_name, cur_rec->last_name, cur_rec->phone_number, cur_rec->email, cur_rec->age);    
        }
    }
    if (num == 0){
        printk(KERN_INFO "info record: not such record: %s", input_last_name);
        strcpy(result, "not such record");
    }
}

static void write_word(char* source, int* start, char* word){
    int i = 0;
    while(source[*start] != ' ' && source[*start] != '\n' && source[*start] != '\0') {
        word[i++] = source[(*start)++];
    }
    word[i] = '\0';
    (*start)++;
}

static void add_record(void){
    char* command = pb_command + 4;
    struct record *new_record;
    int i = 0;
    new_record = (struct record*)kmalloc(sizeof(struct record), GFP_KERNEL);
    write_word(command, &i, new_record->first_name);
    write_word(command, &i, new_record->last_name);
    write_word(command, &i, new_record->phone_number);
    write_word(command, &i, new_record->email);
    write_word(command, &i, new_record->age);
    
    INIT_LIST_HEAD(&new_record->list);
    list_add_tail(&(new_record->list), &(records_list.list));
    printk(KERN_INFO "add record: name: %s, lastname: %s, phone: %s, email: %s, age: %s \n", 
        new_record->first_name, new_record->last_name, new_record->phone_number, new_record->email, new_record->age);
    strcpy(result, "added");
}

static void delete_record(void){
    char* command = pb_command + 7;
    int num;
    char input_last_name[16];
    struct record* cur_rec;
    struct record* tmp;
    strcpy(input_last_name, command);
    input_last_name[strlen(input_last_name) - 1] = '\0';

    num = 0;
    list_for_each_entry_safe(cur_rec, tmp, &records_list.list, list) {
        if (!strcmp(cur_rec->last_name, input_last_name)) {
            ++num;
            printk(KERN_INFO "delete record: name: %s, lastname: %s, phone: %s, email: %s, age: %s \n", 
                cur_rec->first_name, cur_rec->last_name, cur_rec->phone_number, cur_rec->email, cur_rec->age);
            list_del(&cur_rec->list);
            kfree(cur_rec);
            strcpy(result, "deleted");
        }
    }
    if (num == 0){
        printk(KERN_INFO "delete record: not such record: %s", input_last_name);
        strcpy(result, "not such record");
    }
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    if (copy_to_user(buffer, result, RESULT_SIZE)) {
        return -EFAULT;
    } else {
        return 1;
    }
}

static ssize_t device_write(struct file *flip, const char *command, size_t len, loff_t *offset) {
    strcpy(pb_command, command);
    if (!strncmp(pb_command, "info", 4)){
        find_info();
        return 0;
    }
    if (!strncmp(pb_command, "add", 3)){
        add_record();
        return 0;
    }
    if (!strncmp(pb_command, "delete", 6)){
        delete_record();
        return 0;
    }
    strncpy(result, "invalid command", RESULT_SIZE);
    return 0;
}

static struct file_operations file_opers = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .owner = THIS_MODULE,
};

static int __init phone_book_init(void) {
    INIT_LIST_HEAD(&(records_list.list));
    device_major_number = register_chrdev(0, "phone_book", &file_opers);
    if (device_major_number < 0) {
        printk(KERN_ALERT "Could not register device\n");
        return device_major_number;
    } else {
        printk(KERN_INFO "phone_book module registered, device major number: %d\n", device_major_number);
        return 0;
    }
}
static void __exit phone_book_exit(void) {
    unregister_chrdev(device_major_number, DEVICE_NAME);
    printk(KERN_INFO "phone_book module unregistered!\n");
}

module_init(phone_book_init);
module_exit(phone_book_exit);
