#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>  /* Needed for linked list implementation */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

/* Defining my custom structure 'struct node' */
 
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
	struct task_struct *task = NULL, *ts = NULL;
	struct list_head *pos, *q;
	struct node *temp;
	char st[20];
	int count = 1;
	INIT_LIST_HEAD(&taskList.list);
	
	/* Building my linked list from process list */
	printk(KERN_ALERT "\nPrinting output from kernel process list :\n");
	for_each_process(task) {
		temp = (struct node *)kmalloc(sizeof(struct node),GFP_KERNEL);
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
		printk(KERN_ALERT "( %x ): pid = %d || ppid = %d || name = %s || priority = %d || state = %ld : %s", task, task->pid, task->parent->pid, task->comm, task->prio, task->state, st);
		
		list_add_tail(&(temp->list), &(taskList.list));
	}
	
	printk(KERN_INFO "\nPrinting output from Custom linked list :\n");
	list_for_each_entry(temp, &taskList.list, list) {
		switch(temp->state)	{
			case -1 : strcpy(st,"Unrunnable\n");
			break;
			case 0 :  strcpy(st,"Runnable\n");
			break;
			default : strcpy(st,"Stopped\n");
			break;
		}
		printk(KERN_INFO "( %x ): pid = %d || ppid = %d || name = %s || priority = %d || state = %ld : %s", temp, temp->pid, temp->ppid, temp->name, temp->priority, temp->state, st);
	}
		
	ts = next_task(&init_task);
	printk(KERN_ALERT "\nHead ( %x ) of kernel process list : pid = %d || name = %s \n", ts, ts->pid, ts->comm);
	
	temp = list_first_entry(&taskList.list, struct node, list);
	printk(KERN_ALERT "\nHead ( %x ) of custom linked list : pid = %d || name = %s \n", temp, temp->pid, temp->name);
	
	
	
	printk(KERN_ALERT "\nRemoving alternate entries from my linked list :\n");
	
	list_for_each_safe(pos, q, &taskList.list) {
		if(count%2 == 0) {
			temp = list_entry(pos, struct node, list);
			list_del(pos);
			kfree(temp);
		}
		count++;
	}
	
	list_for_each_entry(temp, &taskList.list, list) {
		switch(temp->state)	{
			case -1 : strcpy(st,"Unrunnable\n");
			break;
			case 0 :  strcpy(st,"Runnable\n");
			break;
			default : strcpy(st,"Stopped\n");
			break;
		}
		printk(KERN_ALERT "pid = %d || ppid = %d || name = %s || priority = %d || state = %ld : %s", temp->pid, temp->ppid, temp->name, temp->priority, temp->state, st);
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
	

