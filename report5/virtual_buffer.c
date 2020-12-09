#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // 2020
#include <linux/slab.h>
#define BUFF_SIZE 1024
#define MAJOR_NUMBER 240 // 2020

static char *buffer = NULL;
static struct file_operations vd_fops = {
    .read = my_read,
    .write = my_write,
    .unlocked_ioctl = my_ioctl, // from kernel 2.6.36
    .open = my_open,
    .release = my_release
};

static int debug = 0;

#define MY_IOCTL_NUMBER 100
#define MY_IOCTL_READ _IOR(MY_IOCTL_NUMBER, 0, int)
#define MY_IOCTL_WRITE _IOW(MY_IOCTL_NUMBER, 1, int)

static int my_open(struct inode *inode, struct file *filp)
{
    printk("[VB] opened\n");
    printk("[VB] inode = %p, flip = %p\n", indoe, flip);
    return 0;
}
static int my_release(struct inode *inode, struct file *filp)
{
    printk("[VB] released\n");
    printk("[VB] inode = %p, flip = %p\n", indoe, flip);
    return 0;
}
static ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    copy_from_user(buffer, buf, count);
    printk("[VB] write to buffer: %s\n", buffer);
    return count;
}
static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    printk("[VB] read from buffer: %s\n", buffer);
    copy_to_user(buf, buffer, count);
    return count;
}

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int size;
    if (_IOC_TYPE(cmd) != MY_IOCTL_NUMBER)
    {
        printk("[VB] ioctl cmd type error : %d\n", _IOC_TYPE(cmd));
        return -EINVAL;
    }

    size = _IOC_SIZE(cmd);
    switch (cmd)
    {
    case MY_IOCTL_WRITE:
        copy_from_user((void *)&debug, (const void *)arg, (unsigned long)size);
        printk("[VB] debug is set to %d\n", debug);
        break;
    case MY_IOCTL_READ:
        printk("[VB] current debug value is %d\n", debug);
        copy_to_user((void *)arg, (const void *)&debug, (unsigned long)size);
        break;
    default:
        printk("[VB] invalid ioctl command : %d\n", cmd);
        break;
    }
    return 0;
}


int __init my_init(void)
{
    if (register_chrdev(MAJOR_NUMBER, "virtual_buffer", &vd_fops) < 0)
        printk("[VB] initial fail\n");
    else
        printk("[VB] initialized\n");
    buffer = (char *)kmalloc(BUFF_SIZE, GFP_KERNEL);
    if (buffer != NULL)
        memset(buffer, 0, BUFF_SIZE);
    return 0;
}
void __exit my_exit(void)
{
    unregister_chrdev(MAJOR_NUMBER, "virtual_buffer");
    printk("[VB] exited\n");
    kfree(buffer);
}
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");

/*

$ cat /proc/devices   
# sudomknod/dev/virtual_bufferc 240 0    // 2020
# sudochmod0666 /dev/virtual_buffer
# sudoinsmodvirtual_buffer.ko
$ tail –f /var/log/syslog

*/