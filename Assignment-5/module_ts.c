#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");


int pID = -1;
module_param(pID, int, 0);
MODULE_PARM_DESC(pID, "An integer");

struct task_struct * trackProcessInfo(void) {
	struct task_struct *tsk = NULL;
	for_each_process(tsk) {
		if(pID == tsk->pid) {
			printk(KERN_INFO "Process pid = %d || Process Name = %s\n",tsk->pid, tsk->comm);
			return tsk;
		}
	}
	return NULL;
}

void trackProcessHierarchy(struct task_struct *task) {
	while(task->parent->pid != task->pid) {
		printk(KERN_INFO "Process pid = %d || Process Name = %s\n",task->pid, task->comm);
		task = task->parent;
	}
}

int __init initiate(void) {
	printk(KERN_INFO "Started tracking Information about process %d\n", pID);
	struct task_struct *task = trackProcessInfo();
	printk(KERN_INFO "Started tracking hierarchy of process %d : \n", pID);
	if(task != NULL)	trackProcessHierarchy(task);
	else	printk(KERN_INFO "Sorry, process not found. Enter correct pid. \n");
	return 0;
}

void __exit terminate(void) {
	printk(KERN_INFO "It's Done !!! \n");
}

module_init(initiate);
module_exit(terminate);
