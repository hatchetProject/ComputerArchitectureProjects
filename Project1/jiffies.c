#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
unsigned long jiffies_init;

static struct file_operations proc_ops = {
  .owner = THIS_MODULE,
  .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc init(void)
{
  /* creates the /proc/hello entry */
  proc_create (PROC_NAME, 0666, NULL, &proc_ops);
  jiffies_init = jiffies;
  return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{
  /* removes the /proc/hello entry */
  remove_proc_entry (PROC_NAME, NULL);
}

/* This function is called each time /proc/hello is read */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
  int rv = 0;
  char buffer[BUFFER_SIZE];
  static int completed = 0;
  if (completed) {
    completed = 0;
    return 0;
  }
  completed = 1;
  rv = sprintf(buffer, "Jiffies: %lu", jiffies - jiffies_init);
  /* copies kernel space buffer to user space usr buf */
  copy_to_user (usr_buf, buffer, rv);
  return rv;
  }

  module init(proc_init);
  module exit(proc_exit);

MODULE LICENSE("GPL");
MODULE DESCRIPTION("Jiffies Module");
MODULE AUTHOR("WHX");
