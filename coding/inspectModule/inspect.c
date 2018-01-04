#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL"); //GPL: the most recommended to be used.
MODULE_AUTHOR("Gustavo Walbon");
MODULE_DESCRIPTION("Module to inspect other chosen module");
MODULE_INFO(vermagic,"4.13.0-25-generic SMP mod_unload mprofile-kernel");


static unsigned long long ptr_DeviceOpen = NULL;
module_param(ptr_DeviceOpen, long, S_IRUSR);
MODULE_PARM_DESC(ptr_DeviceOpen, "Pointer to Device_open value");

static int __init inspect_init(void)
{
    printk(KERN_INFO "Starting inspecting\n");
    if (!ptr_DeviceOpen)
        return -1;

    int *ptrValue = ptr_DeviceOpen;
    printk(KERN_INFO "*ptrValue = %ll", ptrValue);

    int value = 1;
    memcpy(&value, ptrValue, sizeof(int));
    printk(KERN_INFO "Actual value of DeviceOpening: %d", value);
    value +=100;
    memcpy(ptrValue, &value, sizeof(int));
    //(int)(*ptrValue) = value;


    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit inspect_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(inspect_init);
module_exit(inspect_cleanup);
