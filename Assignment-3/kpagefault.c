#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

extern void (*pFault)(struct task_struct *, unsigned long);
static int numFault = 0;


int pID = 3;
module_param(pID, int, 0);
MODULE_PARM_DESC(pID, "An integer");

void trackPageFault(struct task_struct *ts, unsigned long add) {
	if(pID == ts->pid) {
		printk(KERN_INFO "Process pid = %d || Address = %lx.\n",ts->pid, add);
		numFault++;
	}
}

int init_module(void) {
	pFault = trackPageFault;
	printk(KERN_INFO "Started tracking Page Fault Information.\n");
	return 0;
}

void cleanup_module(void) {
	pFault = NULL;
	printk(KERN_INFO "Number of Page faults due to unmapped memory address for process %d are %d.\n", pID, numFault);
}

