/*
 * Simple kernel gaming control
 *
 * Copyright (C) 2019
 * Diep Quynh Nguyen <remilia.1505@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/gaming_control.h>

#define GAME_LIST_LENGTH 1024
#define GAMING_CONTROL_VERSION "0.2"

#define TASK_STARTED 1
#define TASK_KILLED 0

int gaming_mode;

static ssize_t version_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", GAMING_CONTROL_VERSION);
}

static struct kobj_attribute version_attribute = 
	__ATTR(version, 0444, version_show, NULL);

static struct attribute *gaming_control_attributes[] = {
	&version_attribute.attr,
	NULL
};

static struct attribute_group gaming_control_control_group = {
	.attrs = gaming_control_attributes,
};

static struct kobject *gaming_control_kobj;

static int gaming_control_init(void)
{
	int sysfs_result;

	gaming_control_kobj = kobject_create_and_add("gaming_control", kernel_kobj);
	if (!gaming_control_kobj) {
		pr_err("%s gaming_control kobject create failed!\n", __FUNCTION__);
		return -ENOMEM;
        }

	sysfs_result = sysfs_create_group(gaming_control_kobj,
			&gaming_control_control_group);

        if (sysfs_result) {
		pr_info("%s gaming_control sysfs create failed!\n", __FUNCTION__);
		kobject_put(gaming_control_kobj);
	}

	return sysfs_result;
}


static void gaming_control_exit(void)
{
	if (gaming_control_kobj != NULL)
		kobject_put(gaming_control_kobj);
}

module_init(gaming_control_init);
module_exit(gaming_control_exit);
