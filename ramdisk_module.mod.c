#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) =
{
    .name = KBUILD_MODNAME,
    .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
    .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
        __attribute_used__
__attribute__((section("__versions"))) =
{
    { 0x89e24b9c, "struct_module" },
    { 0xd6ee688f, "vmalloc" },
    { 0x5d57df57, "remove_proc_entry" },
    { 0x2fd1d81c, "vfree" },
    { 0xda4008e6, "cond_resched" },
    { 0x1b7d4074, "printk" },
    { 0x2da418b5, "copy_to_user" },
    { 0x625acc81, "__down_failed_interruptible" },
    { 0xd5028665, "create_proc_entry" },
    { 0xeac28d78, "proc_root" },
    { 0x19cacd0, "init_waitqueue_head" },
    { 0x60a4461c, "__up_wakeup" },
    { 0xf2a644fb, "copy_from_user" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
    "depends=";


MODULE_INFO(srcversion, "BA33A1DCBB1D7A0C01D222E");
