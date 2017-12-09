/*
 * Copyright (C) Guangzhou FriendlyARM Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 */

#include <common.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <part.h>

#define MAX_PARTITION_TABLE		20
#define MMC_BLOCK_SIZE			512
#define bytes_to_lba(n)			((n) >> 9)
#define lba_to_bytes(x)			((x) << 9)
#define lba_to_kb(x)			((x) >> 1)

extern int disk_part_table_info(block_dev_desc_t *dev_desc,
		long ex_sector, long relative,
		long (*parts)[3], int *part_num);
extern int disk_part_table_make(block_dev_desc_t *dev_desc,
		long capacity_bl,
		long (*parts)[2], int part_num);

/* Pre-defined partition table */
static disk_partition_t parts_android[] = {
	{
		.start	= bytes_to_lba(0x200),
		.size	= bytes_to_lba(768*1024),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "bootloader",
		.type	= "raw",
#if 0
	}, {
		.start	= bytes_to_lba(CONFIG_ENV_OFFSET),
		.size	= bytes_to_lba(CONFIG_ENV_SIZE),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "env",
		.type	= "raw",
	}, {
		.start	= bytes_to_lba(0x00100000),
		.size	= bytes_to_lba(0x00700000),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "reserved",
		.type	= "raw",
#endif
	}, {
		.start	= bytes_to_lba(0x00800000),
		.size	= bytes_to_lba(0x04800000),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "boot",
		.type	= "ext4",
	}, {
		.start	= bytes_to_lba(0x05000000),
		.size	= bytes_to_lba(0x30000000),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "system",
		.type	= "ext4",
	}, {
		.start	= bytes_to_lba(0x35000000),
		.size	= bytes_to_lba(0x1d000000),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "cache",
		.type	= "ext4",
	}, {
		.start	= bytes_to_lba(0x52000000),
		.size	= 0,
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "userdata",
		.type	= "ext4",
	}
};

static disk_partition_t parts_generic[] = {
	{
		.start	= bytes_to_lba(0x200),
		.size	= bytes_to_lba(768*1024),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "bootloader",
		.type	= "raw",
	}, {
		.start	= bytes_to_lba(0x00800000),
		.size	= bytes_to_lba(0x04800000),
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "boot",
		.type	= "ext4",
	}, {
		.start	= bytes_to_lba(0x05000000),
		.size	= 0,
		.blksz	= MMC_BLOCK_SIZE,
		.name	= "rootfs",
		.type	= "ext4",
	}
};

/* Current partition info. */
static long parts_disk[MAX_PARTITION_TABLE][3] = {{0,},};
static disk_partition_t *parts = NULL;
static int parts_nr;

static int disk_load_current(block_dev_desc_t *dev_desc)
{
	int ret;

	parts_nr = 0;
	ret = disk_part_table_info(dev_desc, 0, 0, parts_disk, &parts_nr);
	if (ret < 0) {
		printf("Load current partition info failed, %d\n", ret);
		return -1;
	}

#if 0
	printf("Current partition of mmc.%d:\n", dev_desc->dev);
	for (i = 0 ; i < parts_nr; i++) {
		printf(" MBR.%d  start :      0x%010lx %10ld KB %s\n", i,
				lba_to_bytes(parts_disk[i][0]), lba_to_kb(parts_disk[i][1]),
				parts_disk[i][2]==0x05?"Ext":"");
	}
#endif

	return 0;
}

static int disk_update_partition(block_dev_desc_t *dev_desc,
		disk_partition_t *parts, int nr)
{
	long parts_mbr[MAX_PARTITION_TABLE][2] = {{0,},};
	int create_ptable = 0;
	int i, j;

	for (i = 0, j = 0; i < nr; i++) {
		if (!strcmp((char *)parts[i].type, "raw"))
			continue;

		parts_mbr[j][0] = parts[i].start;
		parts_mbr[j][1] = parts[i].size;
		j++;
	}

	disk_load_current(dev_desc);

	if (j != parts_nr) {
		create_ptable = 1;
	} else {
		for (i = 0; i < j; i++) {
			if (parts_mbr[i][0] != parts_disk[i][0] ||
				parts_mbr[i][1] != parts_disk[i][1]) {
				create_ptable = 1;
				break;
			}
		}
	}

	if (create_ptable)
		disk_part_table_make(dev_desc, dev_desc->lba, parts_mbr, j);

	return create_ptable;
}

void board_init_partition(char *osname)
{
	block_dev_desc_t *dev_desc;
	int i, nr, ret;

	dev_desc = get_dev("mmc", CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (!dev_desc || dev_desc->type == DEV_TYPE_UNKNOWN) {
		error("invalid mmc device\n");
		return;
	}

	if (!strcasecmp(osname, "android")) {
		parts = parts_android;
		nr = ARRAY_SIZE(parts_android);
	} else {
		parts = parts_generic;
		nr = ARRAY_SIZE(parts_generic);
	}

	/* Initialize size of last partition */
	if (parts[nr-1].size <= 0) {
		parts[nr-1].size = dev_desc->lba - parts[nr-1].start;
	}

	ret = disk_update_partition(dev_desc, parts, nr);
	printf("Fastboot partitions for %s: %s\n", osname, ret ? "*" : "");

	parts_nr = nr;
	for (i = 0; i < parts_nr; i++) {
		printf(" mmc.%d: %-10s - 0x%010lx %10ld KB\n",
			dev_desc->dev, parts[i].name,
			lba_to_bytes(parts[i].start), lba_to_kb(parts[i].size));
	}
}

int board_get_part_info(block_dev_desc_t *dev_desc, const char *name,
		disk_partition_t *info)
{
	int i;

	for (i = 0; i < parts_nr; i++) {
		if (!strcmp((char *)parts[i].name, name)) {
			*info = parts[i];
			return 0;
		}
	}

	return -1;
}

