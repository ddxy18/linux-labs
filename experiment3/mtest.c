#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define MAX_CMD_LEN 64

/*Print all vma of the current process*/
static void mtest_list_vma(void) {
  struct vm_area_struct *vma = NULL;

  printk(KERN_INFO "exec listvma");

  for (vma = current->mm->mmap; vma != NULL; vma = vma->vm_next) {
    printk(KERN_INFO "%lx %lx %ld", vma->vm_start, vma->vm_end, vma->vm_flags);
  };
}

/*Find va->pa translation */
static void mtest_find_page(unsigned long addr) {
  struct page *pages[1];

  printk(KERN_INFO "exec findpage %lx", addr);

  if (get_user_pages(addr, 1, 0, pages, NULL) < 1) {
    printk(KERN_WARNING "translation not found");
    return;
  }
  printk(KERN_INFO "%lx -> %llx", addr, page_to_phys(pages[0]));
}

/*Write val to the specified address */
static void mtest_write_val(unsigned long addr, unsigned long val) {
  unsigned long tmp_val = 0;

  printk(KERN_INFO "exec writeval %lx %ld", addr, val);

  if (get_user(tmp_val, (long unsigned int *)addr) == -EFAULT ||
      put_user(val, (long unsigned int *)addr) == -EFAULT) {
    return;
  }
  printk(KERN_INFO "write %ld to %lx", val, addr);
}

static ssize_t mtest_proc_write(struct file *file, const char __user *buffer,
                                size_t count, loff_t *data) {
  char user_data[MAX_CMD_LEN];
  char command[MAX_CMD_LEN];
  unsigned long arg1 = 0;
  unsigned long arg2 = 0;

  if (count >= MAX_CMD_LEN || copy_from_user(user_data, buffer, count)) {
    return -EFAULT;
  }

  user_data[count] = '\0';
  sscanf(user_data, "%s %lx %ld", command, &arg1, &arg2);

  if (strcmp(command, "listvma") == 0) {
    mtest_list_vma();
  } else if (strcmp(command, "findpage") == 0) {
    mtest_find_page(arg1);
  } else if (strcmp(command, "writeval") == 0) {
    mtest_write_val(arg1, arg2);
  }

  return count;
}

static ssize_t mtest_proc_read(struct file *file, char __user *buffer,
                               size_t count, loff_t *data) {
  return 0;
}

static struct file_operations proc_mtest_operations = {
    .read = mtest_proc_read, .write = mtest_proc_write};
static struct proc_dir_entry *mtest_proc_entry;

static int __init mtest_init(void) {
  mtest_proc_entry = proc_create("mtest", 0244, NULL, &proc_mtest_operations);
  if (!mtest_proc_entry) {
    return -EFAULT;
  }
  return 0;
}

static void __exit mtest_exit(void) { proc_remove(mtest_proc_entry); }

module_init(mtest_init);
module_exit(mtest_exit);
