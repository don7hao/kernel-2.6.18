#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_
#ifdef __KERNEL__

#include <linux/mount.h>
#include <linux/sched.h>

struct namespace {
    //usage counter(how many processes share the namespace)
	atomic_t		count;
	//represents the root of the tree of mounted filesystems of this namespace
	struct vfsmount *	root;
	//the head of a doubly linked circular list collecting all mounted
	//filesystem that belong to the namespace.
	struct list_head	list;
	wait_queue_head_t poll;
	int event;
};

extern int copy_namespace(int, struct task_struct *);
extern void __put_namespace(struct namespace *namespace);
extern struct namespace *dup_namespace(struct task_struct *, struct fs_struct *);

static inline void put_namespace(struct namespace *namespace)
{
	if (atomic_dec_and_lock(&namespace->count, &vfsmount_lock))
		/* releases vfsmount_lock */
		__put_namespace(namespace);
}

static inline void exit_namespace(struct task_struct *p)
{
	struct namespace *namespace = p->namespace;
	if (namespace) {
		task_lock(p);
		p->namespace = NULL;
		task_unlock(p);
		put_namespace(namespace);
	}
}

static inline void get_namespace(struct namespace *namespace)
{
	atomic_inc(&namespace->count);
}

#endif
#endif
