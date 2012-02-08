#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


#define DRIVER_AUTHOR "ringo@ce-lab.net"
#define DRIVER_DESC "A sample driver"


static int h3_data __initdata = 3;

int my_init( void )
{
    printk( KERN_ALERT "Hello world 1. %d\n", h3_data );
    return 0;
}

void my_cleanup(void)
{
    printk( KERN_ALERT "hello world cleanup %d\n", h3_data );
}

module_init(my_init);
module_exit(my_cleanup);

MODULE_AUTHOR(DRIVER_AUTHOR);

MODULE_DESCRIPTION(DRIVER_DESC);

    
