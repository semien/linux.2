#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Semien Ishkhanyan");
MODULE_DESCRIPTION("Keyboard Interruptions");

#define DEVICE_NAME "keystats"
#define TASKLET_NAME "keytasklet"
#define COMMAND_SIZE 128
#define RESULT_SIZE 128
#define KEYBOARD_IRQ 1
#define BUFSIZE 1024*1024

static int device_open_count = 0;
static int device_major_number;
static time_t clicktimes[BUFSIZE];
static unsigned int current_pos = 0;
static int my_dev_id;

// bottom half
void tasklet_action(unsigned long flag) {
    struct timespec curtime;
    getnstimeofday(&curtime);
    clicktimes[current_pos++] = curtime.tv_sec;
    if (current_pos == BUFSIZE) 
        current_pos = 0;
    
}

DECLARE_TASKLET(tasklet, tasklet_action, 0);

static irqreturn_t key_handler(int irq, void* dev_id) {
    int scancode = inb(0x60);
    if (!(scancode & 0x80)) // тасклет запускается только при нажатии
        tasklet_schedule(&tasklet);
    return IRQ_HANDLED;
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
    unsigned int num = 0;
    unsigned int index;
    time_t current_time;
    struct timespec curtime;
    char answer[32];
    getnstimeofday(&curtime);
    current_time = curtime.tv_sec;
    if (current_pos) {
        index = current_pos - 1;
    } else {
        index = BUFSIZE-1;
    }
    while (clicktimes[index]-current_time < 60 && clicktimes[index] != 0) {
        ++num;
        if (index) {
            --index;
        } else {
            index = BUFSIZE-1;
        }
    }
    memset(answer,0,32*sizeof(char));    
    sprintf(answer, "%u\n", num);
    return simple_read_from_buffer(buffer, len, offset, answer, 32);
}
 

static struct file_operations file_opers = {
    .read = device_read,
    .open = device_open,
    .release = device_release,
    .owner = THIS_MODULE,
};

static int __init keystats_init(void) {
    device_major_number = register_chrdev(0, DEVICE_NAME, &file_opers);
    if (device_major_number < 0) {
        printk(KERN_ALERT "Could not register device\n");
        return device_major_number;
    } else {
        printk(KERN_INFO "Keystats module registered, device major number: %d\n", device_major_number);
    }
    // register handler
    if (request_irq(KEYBOARD_IRQ, key_handler, IRQF_SHARED, DEVICE_NAME, &my_dev_id)) {
        printk(KERN_ALERT "cannot register IRQ 1");
        return -EIO;
    }
    memset(clicktimes, 0, BUFSIZE*sizeof(time_t));
    return 0;
}
static void __exit keystats_exit(void) {
    unregister_chrdev(device_major_number, DEVICE_NAME);
    tasklet_kill(&tasklet);
    free_irq(KEYBOARD_IRQ, &my_dev_id);
    printk(KERN_INFO "Keystats module unregistered!\n");
}

module_init(keystats_init);
module_exit(keystats_exit);
