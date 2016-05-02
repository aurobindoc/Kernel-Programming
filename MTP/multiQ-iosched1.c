#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

MODULE_AUTHOR("Aurobindo Mondal");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multilevel Priority Queue IO scheduler");

#define numQ 3

struct mpq_data	{
	struct list_head queue[numQ];
};

static void mpq_merged_requests(struct request_queue *q, struct request *rq, struct request *next)
{
	list_del_init(&next->queuelist);
}


static int mpq_dispatch(struct request_queue *q, int force)	{
	struct mpq_data *nd = q->elevator->elevator_data;
	struct request *rq;
	
	rq = list_first_entry_or_null(&nd->queue[0], struct request, queuelist);
	if(rq)	{
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		printk(KERN_ALERT "Dispatched from Queue 1\n");
		return 1;
	}
	else {
		rq = list_first_entry_or_null(&nd->queue[1], struct request, queuelist);
		if(rq)	{
			list_del_init(&rq->queuelist);
			elv_dispatch_sort(q, rq);
			printk(KERN_ALERT "Dispatched from Queue 2\n");
			return 1;
		}
		else {
			rq = list_first_entry_or_null(&nd->queue[2], struct request, queuelist);
			if(rq)	{
				list_del_init(&rq->queuelist);
				elv_dispatch_sort(q, rq);
				printk(KERN_ALERT "Dispatched from Queue 3\n");
				return 1;
			}
		}
	}
	return 0;
}


static void mpq_add_request(struct request_queue *q, struct request *rq)
{
	struct task_struct *task = current;
	struct mpq_data *nd = q->elevator->elevator_data;
	
	if(task->pid == -4)	{
		list_add_tail(&rq->queuelist, &nd->queue[0]);
	}
	else if(task->pid == -5)	{
		list_add_tail(&rq->queuelist, &nd->queue[1]);
	}
	else {
		list_add_tail(&rq->queuelist, &nd->queue[2]);
	}
}

static struct request *mpq_former_request(struct request_queue *q, struct request *rq)
{
	struct mpq_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue[0] || rq->queuelist.prev == &nd->queue[1] || rq->queuelist.prev == &nd->queue[2])
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *mpq_latter_request(struct request_queue *q, struct request *rq)
{
	struct mpq_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue[0] || rq->queuelist.next == &nd->queue[1] || rq->queuelist.next == &nd->queue[2])
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}


static int mpq_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct mpq_data *nd;
	struct elevator_queue *eq;
	int i=0;
	
	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;
	
	while(i<numQ)	{
		INIT_LIST_HEAD(&nd->queue[i]);	
        i++;
	}

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void mpq_exit_queue(struct elevator_queue *e)
{
	struct mpq_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue[0]));
	BUG_ON(!list_empty(&nd->queue[1]));
	BUG_ON(!list_empty(&nd->queue[2]));
	kfree(nd);
}

static struct elevator_type elevator_mpq = {
	.ops = {
		.elevator_merge_req_fn		= mpq_merged_requests,
		.elevator_dispatch_fn		= mpq_dispatch,
		.elevator_add_req_fn		= mpq_add_request,
		.elevator_former_req_fn		= mpq_former_request,
		.elevator_latter_req_fn		= mpq_latter_request,
		.elevator_init_fn		= mpq_init_queue,
		.elevator_exit_fn		= mpq_exit_queue,
	},
	.elevator_name = "mpq",
	.elevator_owner = THIS_MODULE,
};

/************************** Handling the sysfs entry *********************************/
/*
 * The 2 processid having the priority of 1 and 2 are writen in 2 files
 * q1 and q2 in /sys/kernel/multiQ. 
 */
 
static struct kobject *mpqObj;
static int q1=-1;
static int q2=-1;
 
 
// Queue 1

static ssize_t q1_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%d\n", q1);
}

static ssize_t q1_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	sscanf(buf, "%du", &q1);
	return count;
}

static struct kobj_attribute q1_attribute = __ATTR(q1, 0664, q1_show, q1_store);


// Queue 2 

static ssize_t q2_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%d\n", q2);
}

static ssize_t q2_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	sscanf(buf, "%du", &q2);
	return count;
}

static struct kobj_attribute q2_attribute = __ATTR(q2, 0664, q2_show, q2_store);

static struct attribute *attrs[] = {
	&q1_attribute.attr,
	&q2_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int __init mpq_init(void)
{
	int ret;
	mpqObj = kobject_create_and_add("multiQ", kernel_kobj);

	if (!mpqObj)
		return -ENOMEM;

	ret = sysfs_create_group(mpqObj, &attr_group);
	if (ret)	kobject_put(mpqObj); //register kobject

	return elv_register(&elevator_mpq);
}

static void __exit mpq_exit(void)
{
	kobject_put(mpqObj); // deregister kobject
	elv_unregister(&elevator_mpq);
	printk(KERN_INFO "GoodBye!!!!\n");
}

module_init(mpq_init);
module_exit(mpq_exit);
