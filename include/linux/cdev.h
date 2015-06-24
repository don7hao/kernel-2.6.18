#ifndef _LINUX_CDEV_H
#define _LINUX_CDEV_H
#ifdef __KERNEL__

/*
 * owner：该设备的驱动程序所属的内核模块，一般设置为THIS_MODULE；
 * ops：文件操作结构体指针，file_operations结构体中包含一系列对设备进行操作的函数接口；
 * dev：设备号。dev_t封装了unsigned int，该类型前12位为主设备号，后20位为次设备号；
 */
struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;
	unsigned int count;
};

void cdev_init(struct cdev *, const struct file_operations *);

struct cdev *cdev_alloc(void);

void cdev_put(struct cdev *p);

int cdev_add(struct cdev *, dev_t, unsigned);

void cdev_del(struct cdev *);

void cd_forget(struct inode *);

#endif
#endif
