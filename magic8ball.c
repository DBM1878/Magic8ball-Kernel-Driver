#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("David Middour");
MODULE_LICENSE("GPL");

#define DEVICE_NAME "magic8ball"
static int doThing; //makes sure reading magic8ball doesnt get stuck in an infinite loop

static ssize_t magicball_read(struct file *file, char *buffer, size_t len, loff_t *offset) {
	//responses magic8ball can give
	//all uncle Iroh quotes from Avatar
	static const char *responses[] = {
	"It's time for you to look inward, and start asking yourself the big questions. Who are you? And what do you want?\n",
	"Follow your passion and life will reward you.\n",
	"Sometimes the best way to solve your own problems is to help someone else.\n",
	"In the darkest times, hope is something you give yourself.\n",
	"Sometimes, life is like this dark tunnel. You can't always see the light at the end of the tunnel, but if you just keep moving, you will come to a better place.\n",
	"Although it's always best to believe in one's self, a little help from others can be a great blessing.\n",
	"Is it your own destiny? Or is it a destiny someone else has tried to force on you?\n",
	"Pride is not the opposite of shame, but its source. True humility is the only antidote to shame.\n",
	"There is nothing wrong with letting people who love you, help you\n",
	"Be careful what you wish for. History is not always kind to its subjects.\n",
	"It is important to draw wisdom from many different places. If you take it from only one place, it becomes rigid and stale.\n",
	"You have light and peace inside of you. If you let it out, you can change the world around you.\n",
	"Perfection and power are overrated. It is very wise to choose happiness and love.\n",
	"You must never give in to despair.\n",
	"Failure is only the opportunity to begin again, only this time more wisely.\n",
	"Hope is something you give yourself. That is the meaning of inner strength.\n",
	"A man needs his rest.\n",
	"It is usually best to admit mistakes when they occur and to seek to restore honor.\n",
	"Follow your passion, and life will reward you.\n",
	"Now is not one of those times.\n"
	};
	unsigned int randOption;
	if(doThing == 0) {
		return 0;
	}
	doThing = 0;

	//get random number between 0-19
	get_random_bytes(&randOption, sizeof(randOption));
	randOption %= 20;

	//print respnse to console
	if(copy_to_user(buffer, responses[randOption], strlen(responses[randOption])+1) != 0) {
		printk(KERN_ERR);
		return -EFAULT;
	}
	return strlen(responses[randOption])+1;
}

static int magicball_open(struct inode *inode, struct file *file) {
	//set status of module to read only once
	doThing = 1;
	return 0;
}

static const struct file_operations magic_fops = {
	//defined file operations
    .read = magicball_read,
	.open = magicball_open,
};

static struct miscdevice magic_device = {
	// Miscellaneous device structure
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &magic_fops,
    .mode = 0444,
};

static int __init magicball_init(void) {
	// Module initialization and registration
    int ret = misc_register(&magic_device);
    if (ret) {
		printk(KERN_ERR "Failed to register misc device: %d\n", ret);
        return ret;
    }

	printk(KERN_INFO "Device registered successfully\n");
    return 0;
}

static void __exit magicball_exit(void) {
	// Module cleanup
    misc_deregister(&magic_device);
    printk(KERN_INFO"Misc device unregistered\n");
}

module_init(magicball_init);
module_exit(magicball_exit);
