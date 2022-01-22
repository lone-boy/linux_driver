# Linux中断简介

## 中断号
每个中断都有一个中断号，通过中断号即可区分不同的中断，有的资料也把中断号叫做中断线。在Linux内核中使用一个int变量表示中断号。

## request_irq函数

在linux内核中要想使用某个中断是需要申请的，request_irq函数用于申请中断，request_irq函数可能会导致睡眠，因此不能在中断上下文或者其他禁止睡眠的代码段中使用request_irq函数。request_irq函数会激活中断，所以不需要我们手动使能中断，函数原型如下:

```c
int request_irq(unsigned int	irq,
                				irq_handler_t handler,
                				unsigned long flags,
                				const char *name,
                				void			*dev)
    /*
   irq:要申请的中断号
   handler:中断处理函数
   flags:中断标志  include/linux/interrupt.h				可以将flags设置标志，可以通过“|”来实现多组组合。
   name:中断名字
   dev:如果将flags设置为IRQF_SHARED，dev用来区分不同的中断，一般情况会把dev设置为结构体。
   */
```

![](/home/jcc/图片/zynq/irq.png)

## free_irq函数

使用完中断后需要用free_irq函数释放掉相应的中断。如果中断不是共享的，那么free_irq函数会删除中断处理函数并且静止中断。

```c
void free_irq(unsigned int irq,void *dev)
    
    /*
   irq:要释放的中断
   dev:中断设置为共享的话，此参数用来区分具体的中断。共享中断只有在释放最后中断处理函数的时候才会被禁止掉。
   */
```

## 中断处理函数

使用request_irq函数申请中断的时候需要设置中断处理函数，中断处理函数格式如下

```c
irqreturn_t (*irq_handler_t)(int,void *);
/*
第一个参数就是中断号，第二个就是一个通用指针
*/
enum irqreturn{
    IRQ_NONE	=(0 << 0),
    IRQ_HANDLED = (1<<0),
    IRQ_WAKE_THREAD = (1<<1),
};
typedef enum irqreturn irqreturn_t;

```

一般中断服务函数返回值使用如下形式

return IRQ_RETVAL(IRQ_HANDLED)

## 中断使能与禁止函数

```c
void enable_irq(unsigned int irq);
void disable_irq(unsigned int irq);
```

enable_irq 和 disable_irq 用于使能和禁止指定的中断,irq 就是要禁止的中断号。
disable_irq 函数要等到当前正在执行的中断处理函数执行完才返回,因此使用者需要保证
不会产生新的中断,并且确保所有已经开始执行的中断处理程序已经全部退出。在这种情况
下,可以使用另外一个中断禁止函数:

```c
void disable_irq_nosync(unsigned int irq);
```

如果我们需要关闭当前处理器的整个中断系统，这个时候可以使用如下两个函数

```c
local_irq_enable();
local_irq_disable();
```

有的时候需要保留中断状态

```c
local_irq_save(flags);
local_irq_restore(flags);
```



