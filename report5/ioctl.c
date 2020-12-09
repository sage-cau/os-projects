static struct file_operations vd_fops = {
    .read = my_read,
    .write = my_write,
    .unlocked_ioctl = my_ioctl, // from kernel 2.6.36
    .open = my_open,
    .release = my_release
};
static int debug = 0;

#define MY_IOCTL_NUMBER100
#define MY_IOCTL_READ_IOR(MY_IOCTL_NUMBER, 0, int)
#define MY_IOCTL_WRITE_IOW(MY_IOCTL_NUMBER, 1, int)

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
