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
	{ 0xc257719d, __VMLINUX_SYMBOL_STR(netlink_kernel_release) },
	{ 0xbedae1a6, __VMLINUX_SYMBOL_STR(__netlink_kernel_create) },
	{ 0xc1015e3a, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x1b588732, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
	{ 0x542937b4, __VMLINUX_SYMBOL_STR(netlink_unicast) },
	{ 0xcee14c1d, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x8ea80fca, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "FC5CDBCE4F73C73F7F7B965");
