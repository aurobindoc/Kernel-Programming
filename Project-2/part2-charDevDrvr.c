#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

#ifndef VM_RESERVED
#define  VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

char *buffer;

static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)	{
    struct page *page;
	page = virt_to_page(buffer);
    get_page(page);
    vmf->page = page;
    return 0;
}

struct vm_operations_struct mmap_vm_ops = {
    .fault =    mmap_fault,    
};

static int myMmap(struct file *myFile, struct vm_area_struct *myVma)    { 
	myVma->vm_ops = &mmap_vm_ops;
    myVma->vm_flags |= VM_RESERVED;    
    return 0;
}

static const struct file_operations myFileOps = {
	.owner      = THIS_MODULE,
    .mmap       = myMmap,
};
		

int __init initialise(void)	{
	if(register_chrdev(279, "myCharDev", &myFileOps)>=0)    
        printk(KERN_INFO "Device successfully registered!!!\n");
    else {
        printk(KERN_INFO "Device registration failed!!!\n");
		return 0;        
	}
    buffer = get_zeroed_page(GFP_KERNEL);
    memcpy((char *)buffer, "Hi, I am Kernel!", 16);
	printk("%s\n", buffer);
	return 0;
}

void __exit terminate(void)	{
	int i;
	printk("%s\n", buffer);
    unregister_chrdev(279, "myCharDev");
    printk(KERN_INFO "Removed device!!!\nGood Bye!!!!\n");
}

module_init(initialise);
module_exit(terminate);
