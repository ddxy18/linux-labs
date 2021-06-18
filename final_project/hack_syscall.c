#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

unsigned long *syscall_table = NULL;

int (*original_clone)(unsigned long clone_flags, unsigned long newsp,
                      int __user *parent_tidptr, unsigned long tls,
                      int __user *child_tidptr);

void set_addr_rw(long unsigned int addr) {
  unsigned int level = 0;
  pte_t *pte = lookup_address(addr, &level);
  if (pte->pte)
    pte->pte |= _PAGE_RW;
}

void set_addr_ro(long unsigned int addr) {
  unsigned int level;
  pte_t *pte = lookup_address(addr, &level);
  if (pte->pte)
    pte->pte &= ~_PAGE_RW;
}

int new_clone(unsigned long clone_flags, unsigned long newsp,
              int __user *parent_tidptr, unsigned long tls,
              int __user *child_tidptr) {
  printk(KERN_INFO "hello，I have hacked this syscall %s", current->comm);

  return original_clone(clone_flags, newsp, parent_tidptr, tls, child_tidptr);
}

static int __init hack_init(void) {
  printk(KERN_INFO "init syscall hack module");

  syscall_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  set_addr_rw((unsigned long)syscall_table);
  original_clone = (void *)syscall_table[__NR_clone];
  syscall_table[__NR_clone] = (long unsigned int)new_clone;
  set_addr_ro((unsigned long)syscall_table);

  return 0;
}

static void __exit hack_exit(void) {
  set_addr_rw((unsigned long)syscall_table);
  syscall_table[__NR_clone] = (long unsigned int)original_clone;
  set_addr_ro((unsigned long)syscall_table);

  printk(KERN_INFO "exit syscall hack module");
}

module_init(hack_init);
module_exit(hack_exit);