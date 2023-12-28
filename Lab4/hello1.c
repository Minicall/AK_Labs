// SPDX-License-Identifier: GPL-2-Clause
#include <hello1.h>

MODULE_AUTHOR("IO-12 Vlad");
MODULE_DESCRIPTION("Lab4");
MODULE_LICENSE("GPL");

struct timing_info {
    struct list_head node;
    ktime_t start_time;
    ktime_t end_time;
};

static struct list_head main_node = LIST_HEAD_INIT(main_node);

int print_hello(uint sprob)
{
    struct timing_info *element, *tmp_element;
    uint counter;

    if (sprob <= 0) {
        pr_err("Error: Invalid 'sprob' (must be positive)\n");
    } else if (sprob > 0 && sprob < 5) {
        pr_info("Note: 'sprob' is less than 5\n");
    } else if (sprob >= 5 && sprob <= 10) {
        pr_warn("Warning: 'sprob' is in the range 5-10\n");
    } else { 
        pr_err("Error: 'sprob' exceeds 10\n");
        return -EINVAL;
    }

    for (counter = 0; counter < sprob; counter++) {
        element = kmalloc(sizeof(struct timing_info), GFP_KERNEL);
        if (ZERO_OR_NULL_PTR(element))
            goto error_cleanup;
        element->start_time = ktime_get();
        pr_info("Hello!\n");
        element->end_time = ktime_get();
        list_add_tail(&element->node, &main_node);
    }
    return 0;

error_cleanup:
    list_for_each_entry_safe(element, tmp_element, &main_node, node) {
        list_del(&element->node);
        kfree(element);
    }
    pr_err("Error: Insufficient memory\n");
    return -ENOMEM;
}
EXPORT_SYMBOL(print_hello);

static int __init module_begin(void)
{
    pr_info("hello1 module starting\n");
    return 0;
}

static void __exit module_finish(void)
{
    struct timing_info *element, *tmp_element;

    list_for_each_entry_safe(element, tmp_element, &main_node, node) {
        pr_info("Execution Time: %lld", element->end_time - element->start_time);
        list_del(&element->node);
        kfree(element);
    }
    BUG_ON(!list_empty(&main_node));
    pr_info("hello1 module successfully unloaded\n");
}

module_init(module_begin);
module_exit(module_finish);

