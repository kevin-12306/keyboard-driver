// File: keyboard_4x4_drv.c
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/uaccess.h>

#define KEYBOARD_MAJOR 105
#define ROW_PORT 0x08000000 // Example address for rows
#define COL_PORT 0x08000004 // Example address for columns

static long ioremap_row_addr, ioremap_col_addr;

const char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

ssize_t KEYBOARD_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops) {
    int row, col;
    unsigned char outbyte = 0xF0;
    unsigned char ret_key = 0xFF;
    int flag = 0;

    for (row = 0; row < 4; row++) {
        outbyte = ~(1 << row);
        outb(outbyte, ioremap_row_addr);
        unsigned char inbyte = inb(ioremap_col_addr);

        for (col = 0; col < 4; col++) {
            if (!(inbyte & (1 << col))) {
                ret_key = key_map[row][col];
                flag = 1;
                break;
            }
        }
        if (flag) break;
    }

    if (copy_to_user(buf, &ret_key, 1)) {
        return -EFAULT;
    }

    return 1;
}

struct file_operations KEYBOARD_ops = {
    .read = KEYBOARD_read,
};

static int __init KEYBOARD_init(void) {
    int ret;
    printk(KERN_INFO "Loading keyboard_4x4_drv...\n");

    ret = register_chrdev(KEYBOARD_MAJOR, "keypad_4x4", &KEYBOARD_ops);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register chrdev\n");
        return ret;
    }

    ioremap_row_addr = ioremap(ROW_PORT, 0xF);
    ioremap_col_addr = ioremap(COL_PORT, 0xF);
    if (!ioremap_row_addr || !ioremap_col_addr) {
        unregister_chrdev(KEYBOARD_MAJOR, "keypad_4x4");
        return -ENOMEM;
    }

    printk(KERN_INFO "Keyboard driver loaded successfully! Major: %d\n", KEYBOARD_MAJOR);
    return 0;
}

static void __exit KEYBOARD_exit(void) {
    iounmap((void *)ioremap_row_addr);
    iounmap((void *)ioremap_col_addr);
    unregister_chrdev(KEYBOARD_MAJOR, "keypad_4x4");
    printk(KERN_INFO "Keyboard driver unloaded successfully\n");
}

MODULE_LICENSE("GPL");
module_init(KEYBOARD_init);
module_exit(KEYBOARD_exit);