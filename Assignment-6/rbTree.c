#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>		/* Needed for linked list implementation */
#include <linux/kfifo.h>	/* Needed for Queue Implementation */	
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/rbtree.h>	

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

int pID = -1;
module_param(pID, int, 0);
MODULE_PARM_DESC(pID, "An integer");


int init_module(void) {
	struct task_struct *task;
	struct vm_area_struct *vma = NULL;
	
	struct rb_root *root;
	struct rb_node *node;
	
	if(pID == -1)	return -1;
	
	task = pid_task(find_vpid(pID), PIDTYPE_PID);
	vma = task->mm->mmap;
	root = &task->mm->mm_rb;
	unsigned long start, end;
	for(node = rb_first(root); node; node = rb_next(node))	{
		start = rb_entry(node, struct vm_area_struct, vm_rb)->vm_start;
		end = rb_entry(node, struct vm_area_struct, vm_rb)->vm_end;
		
		printk(KERN_INFO "start : %lx || end : %lx\n", start, end);
	}
	return 0;
}

void cleanup_module(void)	{
	printk(KERN_INFO "It's done !!!! \n");
}
