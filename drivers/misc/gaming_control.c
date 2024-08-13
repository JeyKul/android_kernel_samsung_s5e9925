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
#include <linux/pm_qos.h>

#define GAME_LIST_LENGTH 1024
#define GAMING_CONTROL_VERSION "0.2"

#define TASK_STARTED 1
#define TASK_KILLED 0

char games_list[GAME_LIST_LENGTH] = {0};
int gaming_mode;

static int check_for_games(struct task_struct *tsk)
{
	char *cmdline;
	int ret;

	cmdline = kmalloc(GAME_LIST_LENGTH, GFP_KERNEL);
	if (!cmdline)
		return 0;

	ret = get_cmdline(tsk, cmdline, GAME_LIST_LENGTH);
	if (ret == 0) {
		kfree(cmdline);
		return 0;
	}

	/* Invalid Android process name. Bail out */
	if (strlen(cmdline) < 7) {
		kfree(cmdline);
		return 0;
	}

	/* tsk isn't a game. Bail out */
	if (strstr(games_list, cmdline) == NULL) {
		kfree(cmdline);
		return 0;
	}

	kfree(cmdline);

	return 1;
}

void game_option(struct task_struct *tsk, enum game_opts opts)
{
	int ret;

	ret = check_for_games(tsk);
	if (!ret)
		return;

	switch (opts) {
	case GAME_START:
		if (tsk->app_state == TASK_STARTED)
			return;

		tsk->app_state = TASK_STARTED;
		gaming_mode = 1;
		break;
	case GAME_RUNNING:
		gaming_mode = 1;
		break;
	case GAME_PAUSE:
		gaming_mode = 0;
		break;
	case GAME_KILLED:
		tsk->app_state = TASK_KILLED;
		gaming_mode = 0;
		break;
	default:
		break;
	}
}

/* Show added games list */
static ssize_t game_packages_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", games_list);
}


/* Store added games list */
static ssize_t game_packages_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	char games[GAME_LIST_LENGTH];

	sscanf(buf, "%s", games);
	sprintf(games_list, "%s", buf);

	return count;
}

static ssize_t version_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", GAMING_CONTROL_VERSION);
}

static struct kobj_attribute game_packages_attribute = 
	__ATTR(game_packages, 0644, game_packages_show, game_packages_store);

static struct kobj_attribute version_attribute = 
	__ATTR(version, 0444, version_show, NULL);

static struct kobj_attribute min_mif_freq_attribute =
	__ATTR(min_mif, 0644, min_mif_freq_show, min_mif_freq_store);

static struct kobj_attribute max_little_freq_attribute =
	__ATTR(little_freq_max, 0644, max_little_freq_show, max_little_freq_store);

static struct kobj_attribute min_big_freq_attribute =
	__ATTR(big_freq_min, 0644, min_big_freq_show, min_big_freq_store);

static struct kobj_attribute max_big_freq_attribute =
	__ATTR(big_freq_max, 0644, max_big_freq_show, max_big_freq_store);

static struct attribute *gaming_control_attributes[] = {
	&game_packages_attribute.attr,
	&version_attribute.attr,
	&min_mif_freq_attribute.attr,
	&max_little_freq_attribute.attr,
	&min_big_freq_attribute.attr,
	&max_big_freq_attribute.attr,
	NULL
};

static struct attribute_group gaming_control_control_group = {
	.attrs = gaming_control_attributes,
};

static struct kobject *gaming_control_kobj;

static int gaming_control_init(void)
{
	int sysfs_result;

	pm_qos_add_request(&gaming_control_min_mif_qos, PM_QOS_BUS_THROUGHPUT, PM_QOS_BUS_THROUGHPUT_DEFAULT_VALUE);
	pm_qos_add_request(&gaming_control_max_little_qos, PM_QOS_CLUSTER0_FREQ_MAX, PM_QOS_CLUSTER0_FREQ_MAX_DEFAULT_VALUE);
	pm_qos_add_request(&gaming_control_min_big_qos, PM_QOS_CLUSTER1_FREQ_MIN, PM_QOS_CLUSTER1_FREQ_MIN_DEFAULT_VALUE);
	pm_qos_add_request(&gaming_control_max_big_qos, PM_QOS_CLUSTER1_FREQ_MAX, PM_QOS_CLUSTER1_FREQ_MAX_DEFAULT_VALUE);
	
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
	pm_qos_remove_request(&gaming_control_min_mif_qos);
	pm_qos_remove_request(&gaming_control_max_little_qos);
	pm_qos_remove_request(&gaming_control_min_big_qos);
	pm_qos_remove_request(&gaming_control_max_big_qos);

	if (gaming_control_kobj != NULL)
		kobject_put(gaming_control_kobj);
}

module_init(gaming_control_init);
module_exit(gaming_control_exit);
