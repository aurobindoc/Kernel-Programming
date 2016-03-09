#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kfifo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

#define file1 "procEntry"
#define file2 "rwCount"

char *data1, *data2;
struct proc_dir_entry *pfs, *pfsCount;
int rc=0, wc=0, flag=0;
DECLARE_KFIFO(myfifo, unsigned char, PAGE_SIZE);

static ssize_t pfsRead(struct file *file, char __user *buffer, size_t count, loff_t *pffset)	{
	int ret;
	unsigned int copied;
	
	if(!strcmp(PDE_DATA(file_inode(file)),"procEntry"))	{
		ret = kfifo_to_user(&myfifo, buffer, 32, &copied);
		rc+=1;
		return ret?ret:copied;
	}
	else if(!strcmp(PDE_DATA(file_inode(file)),"rwCount"))	{
		if(flag>0)	{
			flag=0;
			ret=0;
		}
		else	{
			ret = sprintf(buffer, "Number of Reads : %d\nNumber of Writes : %d\n", rc, wc);
			flag = 1;
		}
		return ret;
	}
	else	{
		printk(KERN_ALERT "Invalid Data\n");
		return 0;
	}
	return 0;
}

static ssize_t pfsWrite(struct file *file, const char *buffer, size_t count, loff_t *pffset)	{
	int ret;
	unsigned int copied;
	ret = kfifo_from_user(&myfifo, buffer, count, &copied);

	wc+=1;
	return ret?ret:copied;
}

static const struct file_operations pfs_fops = {
	.owner = THIS_MODULE,
	.read = pfsRead,
	.write = pfsWrite,
	.llseek = noop_llseek,
};
		
static const struct file_operations pfsCount_fops = {
	.owner = THIS_MODULE,
	.read = pfsRead,
	.llseek = noop_llseek,
};


int __init initialise(void)	{
	data1 = "procEntry";
	data2 = "rwCount";
	pfs = proc_create_data(file1, 0, NULL, &pfs_fops, data1);
	pfsCount = proc_create_data(file2, 0, NULL, &pfsCount_fops, data2);
	INIT_KFIFO(myfifo);

	if(pfs == NULL)	{
		remove_proc_entry(file1,NULL);
		kfifo_free(&myfifo);
		printk(KERN_ALERT "Error : Couldn't initialise /proc/%s\n", file1);
		return -ENOMEM;
	}
	if(pfsCount == NULL)	{
		remove_proc_entry(file2,NULL);
		printk(KERN_ALERT "Error : Couldn't initialise /proc/%s\n", file2);
		return -ENOMEM;
	}
	return 0;
}

void __exit terminate(void)	{
	remove_proc_entry(file1,NULL);
	remove_proc_entry(file2,NULL);
	printk(KERN_ALERT "Good Bye!!!!\n");
}

module_init(initialise);
module_exit(terminate);
