#include <linux/module.h>
#include <linux/init.h>


int my_init(void) {
    printk("Hello World Linux Kernel\n");
    return 0;
}

void my_exit(void) {
    printk("Goodbye Linux Kernel\n");
}
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darwin");
MODULE_DESCRIPTION("A simple Hello World kernel module");