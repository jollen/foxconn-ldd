/*
 * Copyright (C) 1999 - 2004 Jollen Chen <jollen@jollen.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Linux device driver training - 
 * lession 1 (a simple VMA to IO memory mapping driver template)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
//#include <linux/tqueue.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#ifdef CONFIG_SMP
#define __SMP__
#endif

#define	CDATA_MAJOR 121 
#define BUFSIZE     1024

#define LCD_WIDTH   (240)
#define LCD_HEIGHT  (320)
#define LCD_BPP     (4)
#define LCD_SIZE    (LCD_WIDTH*LCD_HEIGHT*LCD_BPP)

struct cdata_t {
    unsigned long *iomem;
    int          index;
};

static int cdata_open(struct inode *inode, struct file *filp)
{
    int i;
    struct cdata_t *cdata;
    unsigned long *iomem;

    cdata = (struct cdata_t *)kmalloc(sizeof(struct cdata_t), GFP_KERNEL);

    iomem = (unsigned long *)ioremap(0x33f00000, 320*240*4);


    for (i = 0; i < 4096; i++) {
        writel(0x00ff0000, iomem++);
    }

    cdata->iomem = iomem;
    filp->private_data = (void *)cdata;

	return 0;
}

static ssize_t cdata_write(struct file *filp, const char *buf, 
				size_t count, loff_t *off)
{
    struct cdata_t *cdata = (struct cdata_t *)filp->private_data;
    unsigned long *iomem;
    int index;

    index = cdata->index;

	return 0;
}

static int cdata_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "cdata: in cdata_release()\n");
	return 0;
}

static int cdata_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long start, end, size;

    start = vma->vm_start;
    end = vma->vm_end;
    size = end - start;

    remap_page_range(start, 0x33f00000, size, PAGE_SHARED);
}

struct file_operations cdata_fops = {	
	open:		cdata_open,
	release:	cdata_release,
	write:		cdata_write,
    mmap:       cdata_mmap,
};

int my_init_module(void)
{
    int i;

	if (register_chrdev(CDATA_MAJOR, "cdata", &cdata_fops)) {
	    printk(KERN_ALERT "cdata module: can't registered.\n");
    }

	return 0;
}

void my_cleanup_module(void)
{
	unregister_chrdev(CDATA_MAJOR, "cdata");
	printk(KERN_ALERT "cdata module: unregisterd.\n");
}

module_init(my_init_module);
module_exit(my_cleanup_module);

MODULE_LICENSE("GPL");
