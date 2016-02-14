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
	char name[100];	// process name
	int priority;	// priority of process
	long state;	// state of process (-1: unrunnable, 0: runnable, >0 stopped
};
struct node taskList;


int init_module(void) {
	struct task_struct *task = NULL;
	struct node *temp;
	char st[20];
	INIT_LIST_HEAD(&taskList.list);
	
	for_each_process(task) {
		temp = (struct node *)kmalloc(sizeof(struct node ),GFP_KERNEL);
		temp->pid = task->pid;
		temp->ppid = task->parent->pid;
		strcpy(temp->name, task->comm);
		temp->priority = task->prio;
		temp->state = task->state;
		
		switch(task->state)	{
			case -1 : strcpy(st,"Unrunnable");
			break;
			case 0 :  strcpy(st,"Runnable\n");
			break;
			default : strcpy(st,"Stopped\n");
			break;
		}
		printk(KERN_INFO "pid = %d || ppid = %d || name = %s || priority = %d || state = %ld : %s", task->pid, task->parent->pid, task->comm, task->prio, task->state, st);
		
		list_add_tail(&(temp->list), &(taskList.list));
	}
	
	printk(KERN_INFO "Listing all the process information :\n");
	list_for_each_entry(temp, &taskList.list, list) {
		switch(temp->state)	{
			case -1 : strcpy(st,"Unrunnable\n");
			break;
			case 0 :  strcpy(st,"Runnable\n");
			break;
			default : strcpy(st,"Stopped\n");
			break;
		}
		printk(KERN_INFO "pid = %d || ppid = %d || name = %s || priority = %d || state = %ld : %s", temp->pid, temp->ppid, temp->name, temp->priority, temp->state, st);
		
	}
	
	return 0;
}
	
void cleanup_module(void) {
	struct list_head *pos, *q;
	struct node *temp;
	list_for_each_safe(pos, q, &taskList.list) {
		temp = list_entry(pos, struct node, list);
		list_del(pos);
		kfree(temp);
	}
	printk(KERN_INFO "It's done!!!!");
}
	

