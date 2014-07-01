#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd7cce10d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x8ea80fca, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x5193b802, __VMLINUX_SYMBOL_STR(vfs_fsync) },
	{ 0xa2d8f0b3, __VMLINUX_SYMBOL_STR(vfs_write) },
	{ 0xc783e527, __VMLINUX_SYMBOL_STR(vfs_read) },
	{ 0xf193cd2, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0xe3f9697c, __VMLINUX_SYMBOL_STR(filp_open) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9A28A65ACAD7CBB125BB52D");
