#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x6a91b2bc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xe1d45d3e, __VMLINUX_SYMBOL_STR(seq_release) },
	{ 0xbcf21a07, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xc9f4200a, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0xac34070d, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0x8991070f, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x32e8d68a, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x93361036, __VMLINUX_SYMBOL_STR(nf_register_hook) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x439a4a19, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf5769951, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xb7f55ecc, __VMLINUX_SYMBOL_STR(atomic64_add_return_cx8) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x753ffa62, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xcc4d1bfb, __VMLINUX_SYMBOL_STR(atomic64_read_cx8) },
	{ 0x4d8ee16e, __VMLINUX_SYMBOL_STR(seq_open) },
	{ 0x76ebea8, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0xf18242e1, __VMLINUX_SYMBOL_STR(atomic64_set_cx8) },
	{ 0x67f7403e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E673646318D04F988040FBC");
