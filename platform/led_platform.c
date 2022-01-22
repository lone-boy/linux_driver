#include "linux/module.h"
#include "linux/of_gpio.h"
#include "linux/platform_device.h"
#include "linux/leds.h"


struct myled_data{
    struct led_classdev cdev;       //led device
    int gpio;
};

/* get struct addr from first variable */
static inline struct myled_data* cdev_to_led_data(struct led_classdev* led_cdev)
{
    return container_of(led_cdev,struct myled_data,cdev);
}

static int myled_init(struct platform_device* pdev)
{
    struct myled_data* led_data = platform_get_drvdata(pdev);
    struct device* dev = &pdev->dev;
    int ret;

    /* get gpio from dts */
    led_data->gpio = of_get_named_gpio(dev->of_node,"led-gpio",0);
    if(!gpio_is_valid(led_data->gpio)){
        dev_err(dev,"Failed to get gpio from dts");
        return -EINVAL;
    }

    /* request gpio */
    ret = devm_gpio_request(dev,led_data->gpio,"Ps led 0 gpio");
    if(ret){
        dev_err(dev,"Failed to request gpio");
        return ret;
    }

    gpio_direction_output(led_data->gpio,1);

    return 0;
}

static void myled_brightness_set(struct led_classdev* led_cdev,enum led_brightness value){
    struct myled_data* led_data = cdev_to_led_data(led_cdev);
    int level;

    if(LED_OFF == value)
        level = 1;
    else
        level = 0;

    gpio_set_value(led_data->gpio,level);
}

static int myled_brightness_set_blocking(struct led_classdev* led_cdev,enum led_brightness value){
    myled_brightness_set(led_cdev,value);
    return 0;
}

static int myled_probe(struct platform_device* pdev){
    struct myled_data* led_data;
    struct led_classdev* led_cdev;
    int ret;

    dev_info(&pdev->dev,"led device and driver atched seccessfully!\n");
    led_data = devm_kzalloc(&pdev->dev,sizeof(struct myled_data),GFP_KERNEL);
    if(!led_data){
        return -ENOMEM;
    }

    platform_set_drvdata(pdev,led_data);

    /* init led */
    ret = myled_init(pdev);
    if(ret)
        return ret;

    /* init led_cdev variables */
    led_cdev = &led_data->cdev;
    led_cdev->name = "mizar led";
    led_cdev->brightness = LED_OFF;
    led_cdev->max_brightness = LED_FULL;
    led_cdev->brightness_set = myled_brightness_set;
    led_cdev->brightness_set_blocking = myled_brightness_set_blocking;
    //register led device
    return led_classdev_register(&pdev->dev,led_cdev);
}

static int myled_remove(struct platform_device* pdev){
    struct myled_data *led_data = platform_get_drvdata(pdev);
    led_classdev_unregister(&led_data->cdev);
    dev_info(&pdev->dev,"led driver has been removed!\n");
    return 0;
}

/* compatible */
static const struct of_device_id led_of_match[] = {
        {.compatible = "mizar,led"},
        {}
};

static struct platform_driver myled_driver = {
        .driver = {
                .name   = "led",
                .of_match_table = led_of_match,
        },
        .probe  =   myled_probe,
        .remove = myled_remove,
};


module_platform_driver(myled_driver);

MODULE_AUTHOR("microphase");
MODULE_DESCRIPTION("led driver based on led driverframe work");
MODULE_LICENSE("GPL");