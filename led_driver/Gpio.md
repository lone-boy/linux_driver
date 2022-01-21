# Linux gpio驱动的编写
## 回顾
上个工作日已经回忆了字符设备驱动编写的整个流程。在Linux中，字符设备驱动编写的主要函数为以下内容。

```c
#include  <linux/ide.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
 static struct file_operations char_fops{
     .wrtie = chr_write,
     .read = chr_read,
     .open = chr_open,
     .release = chr_open,
 };

static int __init xxx_init(void){
    return 0;
}

static void __exit xxx_exit(void);

module_init(xxx_init);
module_exit(xxx_exit);
 /* 该结构体就是Linux内核驱动的操作函数集合。 具体实现该函数即可。*/
```
字符设备驱动编写较为简单，接下来学习gpio驱动编写来驱动板子上的IO。

## 地址映射
关于地址映射我们首先说一下MMU这个东西，MMU全程叫做Memory Manage Unit。内存管理单元，主要完成以下功能：
1. 完成虚拟空间到物理空间的映射
2. 内存保存，设置储存器的访问权限，设置虚拟储存空间的缓冲特性。

对于32位的处理器来说，虚拟地址范围是 2^32=4GB,而我们搭配的DDR是1GB。因此需要将1GB映射到4GB。
这里完成物理内存和虚拟内存之间的转换需要用到两个函数：ioremap和iounmap。主要定义在arch/arm/include/asm/io.h中。

```c
#define ioremap(cookie,size) __arm_ioremap((cookie), (size), MT_DEVICE)

 void __iomem * __arm_ioremap(phys_addr_t phys_addr, size_t size, unsigned int mtype)
 {
    return arch_ioremap_caller(phys_addr, size, mtype, __builtin_return_address(0));
 }

define APER_CLK_CTRL 0xF800012C
static void __iomem *aper_clk_ctrl_addr;
aper_clk_ctrl_addr = ioremap(APER_CLK_CTRL, 4);
iounmap(aper_clk_ctrl_addr);
```

## I/O内存访问函数
这里I/O函数是指输入/输出的意思，arm体系下理解成I/O内存。使用ioremp函数将寄存器的物理地址映射到虚拟地址以后，就可以直接通过指针来访问这些地址，对于Linux内核来说，推荐使用一组操作函数来对映射后的内存进行读写操作。

1. 读操作函数
```c
u8    readb(const volatile void __iomem *addr);
u16 readw(const volatile void __iomem *addr);
u32 readl(const volatile void __iomem *addr);
```
三个函数分别对应8bit,16bit,32bit读操作，参数addr就是要读取的内存地址，返回值就是数据。
2. 写操作函数
```c
void readb(u8 value,volatile void __iomem *addr);
void readw(u16 value,volatile void __iomem *addr);
void readl(u32 value,volatile void __iomem *addr);

```
这里使用这种方式写gpio驱动就不在详细写。
重点在使用新字符设备进行linux gpio开发。

## 新字符设备驱动实验
字符设备开发重点是使用register_chrdev函数注册字符设备，当不在使用设备的时候就是用unregister_chrdev函数注销。驱动模块加载成功后需要手动使用mknod命令创建设备节点。现在使用新字符设备驱动能够在加载模块的时候自动创建设备节点。
### 新字符设备注册方法
Linux中使用cdev结构体表示一个字符设备，cdev 结构体在include/linux/cdev.h 文件中定义如下：
```c
struct cdev{
    struct kobject kobj;
    strcut module *owner;
    const struct file_operations *ops;
    struct list_head list;
    dev_t dev;
    unsigned int count;
}

struct cdev test_cdev;//这个变量就表示一个字符设备
```
定义好字符设备后就要使用cdev_init函数对其进行初始化，cdev_init函数原型如下
`void cdev_init(struct cdev *cdev,const struct file_operations *fops);`

初始化后使用cdev_add函数用于向Linux系统添加字符设备
`int cdev_add(struct cdev *p,dev_t dev,unsigned count);`

卸载驱动的时候一定要使用cdev_del函数从linux内核中删除相应的字符设备
`void cdev_del(struct cdev *p);`

### 自动创建设备节点
#### 创建和删除类
自动创建设备节点工作是在驱动程序的入口函数中完成，一般在cdev_add函数后面添加自动创建设备节点相关代码。首先要创建一个class类，定义在文件include/linux/device.h里面。
使用class_create 和 class_destroy函数来进行创建和删除。

#### 创建设备
使用device_create 和 device_destroy函数来创建删除设备所处的类。

## 设备树
设备树的相关知识可以百度，这里直接记录干货。
### 查找节点的of函数
设备都是以节点的形式“挂”到设备树上的，因此要想获取这个设备的属性信息，必须先获取到这个设备的节点。Linux内核使用device_node结构体来描述一个节点，此结构体定义在文件include/linux/of.h中。与查找节点有关的函数有5个
```c
struct device_node *of_find_node_by_name(strcut device_node *from,const char *name);
/*
from:开始查找的节点，如果为NULL表示从根节点开始查找整个设备树。
。
*/
struct device_node *of_find_node_by_type(struct device_node *from, const char *type)；
//type:要查找的节点对应的type字符串，也就是device_type属性值
struct device_node *of_find_compatible_node(struct device_node *from,const char *type,const char*compatible)

struct device_node *of_find_matching_node_and_match(struct device_node
*from,const struct of_device_id *matches,const struct of_device_id **match)
/* 通过of_device_id匹配表来查找指定的节点 */

inline struct device_node *of_find_node_by_path(const char *path)
/*通过节点路径来查找指定的节点 */
```

### 提取属性值的OF函数


## gpio子系统简介
前面介绍的gpio寄存器方式都是配置LED灯的GPIO寄存器，驱动开发方式和逻辑基本没有什么区别。内核维护者在内核中设计了一些统一管控系统资源的体系，这些体系让内核能够对系统资源在各个驱动之间的使用统一协调和分配，保证整个内核的稳定健康运行。每个驱动模块如果要使用某个GPIO的时候，就要先调用特殊的接口去申请，申请到后才能使用，使用完后要释放。譬如中断号也是一种资源。
因此，gpio子系统其实就是内核中管控GPIO资源的一套软件设计体系。
