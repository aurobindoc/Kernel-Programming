#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

static unsigned long va;
module_param(va, ulong, 0);
MODULE_PARM_DESC(va, "Virtual Address");

struct node {
	char buffer[256];
};

int __init initialise(void)	{
	char *var = (char *) va;
	printk(KERN_INFO "%s\n",var);
	return 0;
}

void __exit terminate(void) {
    printk(KERN_INFO "Great!!!\n");
}

module_init(initialise);
module_exit(terminate);
