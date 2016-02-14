#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>  /* Needed for linked list implementation */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

struct node {
	struct list_head list;
	int pid;		// process id
	int ppid;		// parent process id
	char *name;	// process name
	int priority;	// priority of process
	long state;	// state of process (-1: unrunnable, 0: runnable, >0 stopped
};
struct node taskList;


int init_module1(void) {
	struct task_struct *task = NULL;
	struct node *temp;
	
	INIT_LIST_HEAD(&taskList.list);
	
	for_each_process(task) {
		temp = (struct node *)kmalloc(sizeof(struct node *),GFP_KERNEL);
		temp->pid = task->pid;
		temp->pid = task->parent->pid;
		strcpy(temp->name, task->comm);
		temp->priority = task->prio;
		temp->state = task->state;
		
		list_add_tail(&(temp->list), &(taskList.list));
	}
	
	printk(KERN_INFO "Listing all the process information :\n");
	
	list_for_each_entry(temp, &taskList.list, list) {
		printk(KERN_INFO "pid = %d || pname = %s \n", temp->pid, temp->name);
	}
	
	return 0;
}
	
void cleanup_module(void) {
	struct list_head *pos, *q;
	struct node *temp;
	list_for_each_safe(pos, q, &taskList.list) {
		temp = list_entry(pos, struct tsnode_list, list);
		list_del(pos);
		kfree(temp);
	}
	printk(KERN_INFO "It's done!!!!");
}
	

