#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kfifo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

static int myOpen(struct inode *myInode, struct file *myFile) {
    printk(KERN_ALERT "Opened File\n");
    return 0;
}

static int myClose(struct inode *myInode, struct file *myFile)   {
    printk(KERN_ALERT "Closed File\n");
    return 0;
}

static ssize_t myRead(struct file *myFile, char __user *buffer, size_t length, loff_t *offset)	{
    return 0,
}

static ssize_t myWrite(struct file *myFile, const char *buffer, size_t length, loff_t *offset)	{
    return 0;
}

static int myMmap(struct file *myFile, struct vm_area_struct *myVma)    { 
    return 0;
}

static const struct file_operations myFileOps = {
	.owner      = THIS_MODULE,
	.open       = myOpen,
    .read       = myRead,
	.write      = myWrite,
    .mmap       = myMmap,
    .release    = myClose,
};
		

int __init initialise(void)	{
	if(register_chrdev(279, "myDevice", &myFileOps)>=0)    
        printk(KERN_ALERT "Device successfully registered!!!");
    else
        printk(KERN_ALERT "Device registration failed!!!");
    
    return 0;
}

void __exit terminate(void)	{
    unregister_chrdev(279, "myDevice");
    printk(KERN_ALERT "Removed device!!!\nGood Bye!!!!\n");
}

module_init(initialise);
module_exit(terminate);
