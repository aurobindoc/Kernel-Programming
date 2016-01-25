#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

extern void (*pCreate)(struct task_struct *);
extern void (*pTerminate)(struct task_struct *);
static int numProcessCreated=0;
static int numProcessTerm=0;

struct node {
	int pid;
	int ppid;
	unsigned int policy;
	struct node *next;
};

struct node *headPC = NULL;
struct node *headPT = NULL;

void createProcess(struct task_struct *ts) {
	struct node *temp, *tmp;
	if(headPC==NULL) {
		 temp = kmalloc(sizeof(struct node), GFP_KERNEL);
		 temp->pid = ts->pid;
		 temp->ppid = ts->parent->pid;
		 temp->policy = ts->policy;
		 headPC = temp;
		 headPC->next = NULL;		
	}
	else {
		temp = headPC;
		while(temp->next != NULL) {
			temp = temp->next;
		}
		tmp = kmalloc(sizeof(struct node), GFP_KERNEL);
		tmp->pid = ts->pid;
		tmp->ppid = ts->parent->pid;
		tmp->policy = ts->policy;
		temp->next = tmp;
		tmp->next = NULL;
	}
	numProcessCreated++;
}

void terminateProcess(struct task_struct *ts) {
	struct node *temp, *tmp;
	if(headPT==NULL) {
		 temp = kmalloc(sizeof(struct node), GFP_KERNEL);
		 temp->pid = ts->pid;
		 temp->ppid = ts->parent->pid;
		 temp->policy = ts->policy;
		 headPT = temp;
		 headPT->next = NULL;		
	}
	else {
		temp = headPT;
		while(temp->next != NULL) {
			temp = temp->next;
		}
		tmp = kmalloc(sizeof(struct node), GFP_KERNEL);
		tmp->pid = ts->pid;
		tmp->ppid = ts->parent->pid;
		tmp->policy = ts->policy;
		temp->next = tmp;
		tmp->next = NULL;
	}
	numProcessTerm++;
}

int init_module(void) {
	pCreate = createProcess;
	pTerminate = terminateProcess;
	printk(KERN_INFO "Started logging creation & termination of processes.\n");
	return 0;
}

void cleanup_module(void) {
	pCreate = NULL;
	pTerminate = NULL;
	
	struct node *temp;
	temp = headPC;
	printk(KERN_INFO "List of process created : \n");
	while(temp != NULL) {
		printk(KERN_INFO "Child pid = %d || Parent pid = %d.\n",temp->pid, temp->ppid);
		temp = temp->next;
	}
	
	temp = headPT;
	printk(KERN_INFO "List of process terminated : \n");
	while(temp != NULL) {
		printk(KERN_INFO "Child pid = %d || Parent pid = %d.\n",temp->pid, temp->ppid);
		temp = temp->next;
	}
	printk(KERN_INFO "Process Creation Info logged. Number of process created are %d and Number of process terminated are %d.\n",numProcessCreated, numProcessTerm);
}
