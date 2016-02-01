#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/fs.h>
#include <linux/mm.h>
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
			return tsk;
		}
	}
	return NULL;
}

void showVMArea(struct task_struct *task) {
	struct vm_area_struct *vm = task->mm->mmap;
	char *fileName;
	while(vm != NULL)	{
		vm_flags_t flags = vm->vm_flags;
		unsigned long size = (vm->vm_end - vm->vm_start)/1024;
		if(vm->vm_file != NULL) fileName = vm->vm_file->f_path.dentry->d_iname;
		else fileName = " ";	// may be [ anon ] or [ stack ]
		char r = flags & VM_READ ? 'r' : '-';
		char w = flags & VM_WRITE ? 'w' : '-';
		char x = flags & VM_EXEC ? 'x' : '-';
		char s = flags & VM_MAYSHARE ? 's' : '-';
		printk(KERN_INFO "%08lx\t%lu KB\t%c%c%c%c-\t%s\n",vm->vm_start, size,r,w,x,s,fileName);
		vm = vm->vm_next;
	}
}

int __init initiate(void) {
	printk(KERN_INFO "Started tracking Information about process %d\n", pID);
	struct task_struct *task = trackProcessInfo();
	printk(KERN_INFO "\nShowing VM Area of process %d : \n", pID);
	if(task != NULL)	showVMArea(task);
	else	printk(KERN_INFO "Error : Sorry !!! process not found. Enter correct pid. \n");
	return 0;
}

void __exit terminate(void) {
	printk(KERN_INFO "It's Done !!! \n");
}

module_init(initiate);
module_exit(terminate);
