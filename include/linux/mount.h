/*
 *
 * Definitions for mount interface. This describes the in the kernel build
 * linkedlist with mounted filesystems.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 * Version: $Id: mount.h,v 2.0 1996/11/17 16:48:14 mvw Exp mvw $
 *
 */
#ifndef _LINUX_MOUNT_H
#define _LINUX_MOUNT_H
#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <asm/atomic.h>

struct super_block;
struct vfsmount;
struct dentry;
struct namespace;

#define MNT_NOSUID	0x01
#define MNT_NODEV	0x02
#define MNT_NOEXEC	0x04
#define MNT_NOATIME	0x08
#define MNT_NODIRATIME	0x10

#define MNT_SHRINKABLE	0x100

#define MNT_SHARED	0x1000	/* if the vfsmount is a shared mount */
#define MNT_UNBINDABLE	0x2000	/* if the vfsmount is a unbindable mount */
#define MNT_PNODE_MASK	0x3000	/* propogation flag mask */

struct vfsmount {
    //pointers for the hash table list
	struct list_head mnt_hash;
	//pointers to the parent filesystem on which this filesystem is mounted
	struct vfsmount *mnt_parent;	/* fs we are mounted on */
	//pointers to the dentry of the mount point directory where the filesystem
	//is mounted
	struct dentry *mnt_mountpoint;	/* dentry of mountpoint */
	//pointers to the dentry of the root directory of this filesystem
	struct dentry *mnt_root;	/* root of the mounted tree */
	//pointers to the superblock object of this filesystem
	struct super_block *mnt_sb;	/* pointer to superblock */
	//head of a list including all filesystem descriptors mounted on directories
	//of this filesystem
	struct list_head mnt_mounts;	/* list of children, anchored here */
	//pointers for the mnt_mounts list of mounted filesystem descriptors
	struct list_head mnt_child;	/* and going through their mnt_child */
	//usage counter(increased to forbid filesystem unmounting)
	atomic_t mnt_count;
    /*
     * The mnt_flags field of the descriptor stores the value of several flags that
     * specify how some kinds of files in the mounted filesystem are handled.
     * MNT_NOSUID : Forbid setuid and setgid flags in the mounted filesystem
     * MNT_NODEV : Forbid access to device files in the mounted filesystem
     * MNT_NOEXEC : Disallow program execution in the mounted filesystem
     */
	int mnt_flags;
	//flag set to true if the filesystem is marked as expired
	//(the filesystem can be automatically umounted if the flag is set and no
	//one is using it)
	int mnt_expiry_mark;		/* true if marked for expiry */
	char *mnt_devname;		/* Name of device e.g. /dev/dsk/hda1 */
	//pointers for namespace's list of mounted filesystem descriptors
	struct list_head mnt_list;
	struct list_head mnt_expire;	/* link in fs-specific expiry list */
	struct list_head mnt_share;	/* circular list of shared mounts */
	struct list_head mnt_slave_list;/* list of slave mounts */
	struct list_head mnt_slave;	/* slave list entry */
	struct vfsmount *mnt_master;	/* slave is on master->mnt_slave_list */
	//pointer to the namespace of the process that a mounted the filesystem
	struct namespace *mnt_namespace; /* containing namespace */
	int mnt_pinned;
};

static inline struct vfsmount *mntget(struct vfsmount *mnt)
{
	if (mnt)
		atomic_inc(&mnt->mnt_count);
	return mnt;
}

extern void mntput_no_expire(struct vfsmount *mnt);
extern void mnt_pin(struct vfsmount *mnt);
extern void mnt_unpin(struct vfsmount *mnt);

static inline void mntput(struct vfsmount *mnt)
{
	if (mnt) {
		mnt->mnt_expiry_mark = 0;
		mntput_no_expire(mnt);
	}
}

extern void free_vfsmnt(struct vfsmount *mnt);
extern struct vfsmount *alloc_vfsmnt(const char *name);
extern struct vfsmount *do_kern_mount(const char *fstype, int flags,
				      const char *name, void *data);

struct file_system_type;
extern struct vfsmount *vfs_kern_mount(struct file_system_type *type,
				      int flags, const char *name,
				      void *data);

struct nameidata;

extern int do_add_mount(struct vfsmount *newmnt, struct nameidata *nd,
			int mnt_flags, struct list_head *fslist);

extern void mark_mounts_for_expiry(struct list_head *mounts);
extern void shrink_submounts(struct vfsmount *mountpoint, struct list_head *mounts);

extern spinlock_t vfsmount_lock;
extern dev_t name_to_dev_t(char *name);

#endif
#endif /* _LINUX_MOUNT_H */
