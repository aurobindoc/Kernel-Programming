#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

MODULE_AUTHOR("Aurobindo Mondal");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multilevel Priority Queue IO scheduler");

struct mpq_data	{
	struct list_head queue;
};


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

	INIT_LIST_HEAD(&nd->queue);

	nd->wait_timer = w_wait;

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void mpq_exit_queue(struct elevator_queue *e)
{
	
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
