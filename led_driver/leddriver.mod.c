#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfb080ece, "module_layout" },
	{ 0xc8afcc70, "device_destroy" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xd805aeab, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x6b2a60, "of_get_named_gpio_flags" },
	{ 0xfe990052, "gpio_free" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xd1f18866, "cdev_del" },
	{ 0x3e6badd6, "class_destroy" },
	{ 0xe9e59beb, "device_create" },
	{ 0x2d0d89b2, "__class_create" },
	{ 0xb9757c96, "cdev_add" },
	{ 0xfad496fc, "cdev_init" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x29a28e54, "of_property_read_string" },
	{ 0xcf388455, "of_find_node_opts_by_path" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x2d8cf763, "gpiod_set_raw_value" },
	{ 0x172e772a, "gpio_to_desc" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x7c32d0f0, "printk" },
	{ 0x5f754e5a, "memset" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

