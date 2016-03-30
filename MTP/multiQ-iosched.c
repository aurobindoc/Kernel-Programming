#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

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


static int noop_dispatch(struct request_queue *q, int force)	{
	struct mpq_data *md = q->elevator->elevator_data;
	struct request *rq;
	
	rq = list_first_entry_or_null(&nd->queue[0], struct request, queuelist);
	if(rq)	{
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		return 1;
	}
	else {
		rq = list_first_entry_or_null(&nd->queue[1], struct request, queuelist);
		if(rq)	{
			list_del_init(&rq->queuelist);
			elv_dispatch_sort(q, rq);
			return 1;
		}
		else {
			rq = list_first_entry_or_null(&nd->queue[2], struct request, queuelist);
			if(rq)	{
				list_del_init(&rq->queuelist);
				elv_dispatch_sort(q, rq);
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
	
	switch(task->pid)	{
		case 1: list_add_tail(&rq->queuelist, &nd->queue[0]);
				break;
		case 2: list_add_tail(&rq->queuelist, &nd->queue[1]);
				break;
		default: list_add_tail(&rq->queuelist, &nd->queue[2]);
				break;
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
	struct mpq_data *md;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	md = kmalloc_node(sizeof(*md), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;
	
	int i=0;
	while(i<numQ)	{
		INIT_LIST_HEAD(&nd->queue[i]);	
	}

	nd->wait_timer = w_wait;

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void mpq_exit_queue(struct elevator_queue *e)
{
	struct mpq_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
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

static int __init mpq_init(void)
{
	return elv_register(&elevator_mpq);
}

static void __exit mpq_exit(void)
{
	elv_unregister(&elevator_mpq);
}

module_init(mpq_init);
module_exit(mpq_exit);
