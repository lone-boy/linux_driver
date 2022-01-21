#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ide.h>


#define CHARDEVBASE_MAJOR 200
#define CHARDEVBASE_NAEM    "chrdevbase"

static char read_buf[100],write_buf[100];
static char kernel_data[] = {"kernel data!"};


static int chrdevbase_open(struct inode *inode,struct file *filp)
{
  printk("chrdevbase open!\r\n");
  return 0;
}

static int chrdevbase_release(struct inode *inode,struct file *filp)
{
  printk("chrdevbase release!\r\n");
  return 0;
}

static ssize_t chrdevbase_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
  int ret = 0;
  memcpy(read_buf,kernel_data,sizeof(kernel_data));
  ret = copy_to_user(buf,read_buf,cnt);
  if(0 == ret)
  {
    printk("kernel send data ok!\r\n");
  }
  else
  {
    printk("kernel senddata failed!\r\n");
  }
  return 0;
}

static ssize_t chrdevbase_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt)
{
  int ret = 0;
  ret = copy_from_user(write_buf,buf,cnt);
  if(0 == ret)
  {
    printk("kernel recevice data:%s\r\n",write_buf);
  }
  else
  {
    printk("kernel recevice data failed!\r\n");
  }
  return 0;
}

static struct file_operations chrdevbase_fops = 
{
  .owner = THIS_MODULE,
  .open   = chrdevbase_open,
  .write = chrdevbase_write,
  .release   = chrdevbase_release,
  .read    = chrdevbase_read,
};

/* 字符设备驱动注册 */
static int __init chrdevbase_init(void)
{
  int ret = 0;
  ret = register_chrdev(CHARDEVBASE_MAJOR,CHARDEVBASE_NAEM,&chrdevbase_fops);

  if(0 > ret){
    printk("chrdevbase driver register failed!\r\n");
  }
  printk("chrdevbase_init()\r\n");
  return 0;
}

static void __exit chrdevbase_exit(void)
{
  unregister_chrdev(CHARDEVBASE_MAJOR,CHARDEVBASE_NAEM);
  printk("chrdevbase_exit()\r\n");
}

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DOMIC");