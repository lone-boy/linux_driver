#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/io.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>

#define GPIOLED_CNT 1
#define GPIOLED_NAME "gpioled"


/* 定义led设备 */
struct gpioled_dev{
    dev_t devid;                                /* 设备号 */
    struct cdev cdev;                       /* cdev 设备 */
    struct class *class;                    /* 类，用于自动申请设备号的时候用到 */
    struct device *device;              /* 设备  device_create是用到 */
    int major;                                      /* 主设备号 */
    int minor;                                      /* 次设备号 */
    struct device_node *node;   /* 设备节点 解析dts文件用到 */
    int led_gpio;                               /* led用到的引脚 */
    int led1_gpio;
};

static struct gpioled_dev gpioled;

static int led_open(struct inode *inode,struct file *filp){
    filp->private_data = &gpioled;
    return 0;
}

static ssize_t led_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt){
    return 0;
}

static ssize_t led_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt){
    int ret;
    char kern_buf[2];

    ret = copy_from_user(kern_buf,buf,cnt);
    if(0 > ret){
        printk(KERN_ERR"kernel write failed!\r\n");
        return -EINVAL;
    }
    printk(KERN_ERR"The kernel recevice data %d%d!\r\n",kern_buf[0],kern_buf[1]);
    if(0 == kern_buf[0]){
        printk(KERN_ERR"write the value high!\r\n");
        if(1 == kern_buf[1]){
            gpio_set_value(gpioled.led_gpio,1);
        }
        else if(2 == kern_buf[1]){
            gpio_set_value(gpioled.led1_gpio,1);
        }
    }
    else if(1 == kern_buf[0]){
        printk(KERN_ERR"write the value low!\r\n");
        if(1 == kern_buf[1]){
            gpio_set_value(gpioled.led_gpio,0);
        }
        else if(2 == kern_buf[1]){
            gpio_set_value(gpioled.led1_gpio,0);
        }
    }
        

        return 0;
}

static int led_release(struct inode *inode,struct file *filp){
    return 0;
}


static struct file_operations gpioled_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release,
};

static int led_gpio_init(void){
    const char *str;
    int ret;

    /* 获取led设备节点 */
    gpioled.node = of_find_node_by_path("/led");
    if(NULL == gpioled.node)
    {
        printk(KERN_ERR"gpioled:Failed to get /led node \n");
        return -EINVAL;
    }


    /* 得到设备树节点后开始读取status属性 */
    ret = of_property_read_string(gpioled.node,"status",&str);
    if(0> ret){
        if(strcmp(str,"okay"))
            return -EINVAL;
    }

    /* 获取compatible属性 */
    ret = of_property_read_string(gpioled.node,"compatible",&str);
    if(0 > ret){
        printk(KERN_ERR"gpioled:Failed to get compatible property\n");
        return ret;
    }
    if(strcmp(str,"microphase,led")){
        printk(KERN_ERR"gpioled:compatible match failed!\n");
        return -EINVAL;
    }
    printk(KERN_INFO"gpioled:device matching successful!\r\n");

    /* 获取gpio编号 即该gpio用到的引脚 */
    gpioled.led_gpio = of_get_named_gpio(gpioled.node,"led-gpio",0);
    if(!gpio_is_valid(gpioled.led_gpio)){
        printk(KERN_ERR"gpioled:failed to get led-gpio\n");
        return -EINVAL;
    }
    printk(KERN_INFO"gpioled:led-gpio num = %d\r\n",gpioled.led_gpio);

    gpioled.led1_gpio = of_get_named_gpio(gpioled.node,"led-gpio",1);
    if(!gpio_is_valid(gpioled.led1_gpio)){
        printk(KERN_ERR"gpioled:failed to get led1-gpio\n");
        return -EINVAL;
    }
    printk(KERN_INFO"gpioled:led1-gpio num = %d\r\n",gpioled.led1_gpio);


    /* 向gpio子系统申请使用GPIO */
    ret = gpio_request(gpioled.led_gpio,"LED-GPIO");
    if(ret){
        printk(KERN_ERR"gpio led:Failed to request led-gpio\n");
        return ret;
    }
    ret = gpio_request(gpioled.led1_gpio,"LED-GPIO1");
    if(ret){
        printk(KERN_ERR"gpio led:Failed to request led1-gpio\n");
        return ret;
    }
    

    /* 申请到gpio后设置gpioled 的状态  设置默认为输出高电平 */
    gpio_direction_output(gpioled.led_gpio,1);
    gpio_direction_output(gpioled.led1_gpio,1);

    /* 初始化led默认状态 */
    ret = of_property_read_string(gpioled.node,"default-state",&str);
    if(!ret){
        if(!strcmp(str,"on")){
            gpio_set_value(gpioled.led_gpio,0);
            gpio_set_value(gpioled.led1_gpio,0);
        }
            
        else{
            gpio_set_value(gpioled.led_gpio,1);
            gpio_set_value(gpioled.led1_gpio,1);
        }
            
    }
    else{
        gpio_set_value(gpioled.led_gpio,1);
        gpio_set_value(gpioled.led1_gpio,1);
    }
    return 1;
}

static int __init led_init(void)
{
   int ret;

   led_gpio_init();
    /* 创建字符设备驱动 */
    //创建设备号
    if(gpioled.major){
        gpioled.devid = MKDEV(gpioled.major,0);
        ret = register_chrdev_region(gpioled.devid,GPIOLED_CNT,GPIOLED_NAME);
        if(ret)
            goto out1;
    }
    else{
        ret = alloc_chrdev_region(&gpioled.devid,0,GPIOLED_CNT,GPIOLED_NAME);
        if(ret)
            goto out1;

        gpioled.major = MAJOR(gpioled.devid);
        gpioled.minor = MINOR(gpioled.devid);
    }

    printk("gpioled:major = %d,minor = %d\r\n",gpioled.major,gpioled.minor);

    /* 初始化cdev */
    gpioled.cdev.owner = THIS_MODULE;
    cdev_init(&gpioled.cdev,&gpioled_fops);

    /* 添加一个cdev */
    ret = cdev_add(&gpioled.cdev,gpioled.devid,GPIOLED_CNT);
    if(ret)
        goto out2;

    /* 创建类 */
    gpioled.class = class_create(THIS_MODULE,GPIOLED_NAME);
    if(IS_ERR(gpioled.class)){
        ret = PTR_ERR(gpioled.class);
        goto out3;
    }

    /* 创建设备 */
    gpioled.device = device_create(gpioled.class,NULL,gpioled.devid,NULL,GPIOLED_NAME);
    if(IS_ERR(gpioled.device)){
        ret = PTR_ERR(gpioled.device);
        goto out4;
    }
    return 0;

out4:
    class_destroy(gpioled.class);
out3:
    cdev_del(&gpioled.cdev);
out2:
    unregister_chrdev_region(gpioled.devid,GPIOLED_CNT);
out1:
    gpio_free(gpioled.led_gpio);
    gpio_free(gpioled.led1_gpio);

    return ret;
}

static void __exit led_exit(void){
    /* 注销设备 */
    device_destroy(gpioled.class,gpioled.devid);
    /* 注销类 */
    class_destroy(gpioled.class);
    /* 删除cdev */
    cdev_del(&gpioled.cdev);
    /* 注销设备号 */
    unregister_chrdev_region(gpioled.devid,GPIOLED_CNT);
    /* 释放GPIO */
    gpio_free(gpioled.led_gpio);
    gpio_free(gpioled.led1_gpio);
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("DOMIC");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Microphase MIZAR GPIO driver");
