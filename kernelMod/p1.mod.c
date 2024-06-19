#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9711567, "module_layout" },
	{ 0xcdbc2c7b, "single_release" },
	{ 0x15b72dea, "seq_lseek" },
	{ 0xe286ae67, "seq_read" },
	{ 0x354a05bf, "remove_proc_entry" },
	{ 0x92997ed8, "_printk" },
	{ 0xf513290a, "proc_create" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x944e6dcb, "seq_printf" },
	{ 0xf1e98c74, "avenrun" },
	{ 0x40c7247c, "si_meminfo" },
	{ 0x6398c850, "init_task" },
	{ 0xe4754df4, "single_open" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9A52DAC052F03662849345D");
