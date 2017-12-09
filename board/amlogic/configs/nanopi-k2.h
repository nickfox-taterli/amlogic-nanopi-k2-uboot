/*
 * board/amlogic/configs/nanopi-k2.h
 *
 * Copyright (C) Guangzhou FriendlyARM Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __NANOPI_K2_H__
#define __NANOPI_K2_H__             1

#define CONFIG_MACH_NANOPI_K2       1

#ifndef __SUSPEND_FIRMWARE__
#include <asm/arch/cpu.h>
#endif	/* for compile problem of A53 and m3 */

#define CONFIG_SYS_GENERIC_BOARD    1
#ifndef __SUSPEND_FIRMWARE__
#ifndef CONFIG_AML_MESON
#warning "include warning"
#endif
#endif	/* for compile problem of A53 and m3 */

/* CPU */
/* clock range: 600-1800, should be multiple of 24 */
#define CONFIG_CPU_CLK              1536

/* SMP definitinos */
#define CPU_RELEASE_ADDR            secondary_boot_func

/* DDR */
#define CONFIG_DDR_SIZE             1024
/* clock range: 384-1200, should be multiple of 24 */
#define CONFIG_DDR_CLK              800
#define CONFIG_DDR_TYPE             CONFIG_DDR_TYPE_DDR3

/* DDR channel setting, please refer hardware design.
 *    CONFIG_DDR0_RANK0_ONLY   : one channel
 *    CONFIG_DDR0_RANK01_SAME  : one channel use two rank with same setting
 *    CONFIG_DDR0_RANK01_DIFF  : one channel use two rank with diff setting
 *    CONFIG_DDR01_SHARE_AC    : two channels  */
#define CONFIG_DDR_CHANNEL_SET      CONFIG_DDR0_RANK01_SAME
#define CONFIG_DDR_FULL_TEST        0	/* 1 for ddr full test */
#define CONFIG_NR_DRAM_BANKS        1

/* DDR power saving */
#define CONFIG_DDR_ZQ_POWER_DOWN
#define CONFIG_DDR_POWER_DOWN_PHY_VREF

/* DDR detection */
#define CONFIG_DDR_SIZE_AUTO_DETECT 1	/* 0: disable, 1: enable */

/* DDR functions */
#define CONFIG_CMD_DDR_D2PLL        0	/* d2pll   cmd */
#define CONFIG_CMD_DDR_TEST         0	/* ddrtest cmd */

/* Platform power init config */
#define CONFIG_PLATFORM_POWER_INIT
#define CONFIG_VCCK_INIT_VOLTAGE    1100
#define CONFIG_VDDEE_INIT_VOLTAGE   1000 // voltage for power up
#define CONFIG_VDDEE_SLEEP_VOLTAGE   850 // voltage for suspend

/* configs for CEC */
#define CONFIG_CEC_OSD_NAME         "NanoPi-K2"
#define CONFIG_CEC_WAKEUP

/* Serial config */
#define CONFIG_CONS_INDEX           2
#define CONFIG_BAUDRATE             115200
#define CONFIG_AML_MESON_SERIAL     1
#define CONFIG_SERIAL_MULTI         1

#define CONFIG_BOOTDELAY            3	/* seconds */
#define CONFIG_ABORTBOOT_WITH_ENTERKEY

/* IR remote wake up for bl30 */
//#define CONFIG_IR_REMOTE
//#define CONFIG_AML_IRDETECT_EARLY

/* See arch/arm/cpu/armv8/SOC/firmware/scp_task/scp_remote.c */
#define CONFIG_IR_REMOTE_POWER_UP_KEY_CNT   4
#define CONFIG_IR_REMOTE_USE_PROTOCOL       0 // 0:nec  1:duokan  2:Toshiba 3:rca 4:rcmm
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL1  0xE51AFB04 // tv ir --- power
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL2  0xBB44FB04 // tv ir --- ch+
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL3  0xF20DFE01 // tv ir --- ch-
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL4  0xBA45BD02
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL5  0x3ac5bd02

/* args/envs */
#define CONFIG_SYS_MAXARGS          64

#define CONFIG_EXTRA_ENV_SETTINGS \
	"firstboot=1\0" \
	"display_width=1920\0" \
	"display_height=1080\0" \
	"display_bpp=24\0" \
	"display_color_index=24\0" \
	"display_color_fg=0xffff\0" \
	"display_color_bg=0\0" \
	"display_layer=osd0\0" \
	"fb_addr=0x3d800000\0" \
	"fb_width=1920\0" \
	"fb_height=1080\0" \
	"fdt_high=0x20000000\0" \
	"dtb_mem_addr=0x1000000\0" \
	"dtb_name=nanopi-k2.dtb\0" \
	"hdmimode=1080p60hz\0" \
	"cvbsmode=576cvbs\0" \
	"serial#=fe2017a905b20003\0" \
	"initrd_high=0x40000000\0" \
	"initrd_start=0x39000000\0" \
	"initrd_size=0x200000\0" \
	"initrd_name=ramdisk.img\0" \
	"loadaddr=0x1080000\0" \
	"bloader=ext4load mmc 0:1\0" \
	"loadkernel=" \
		"${bloader} ${loadaddr} Image\0" \
	"loaddtb=" \
		"${bloader} ${dtb_mem_addr} ${dtb_name}; fdt addr ${dtb_mem_addr}\0" \
	"loadinitrd=" \
		"${bloader} ${initrd_start} ${initrd_name}; setenv initrd_size 0x${filesize}\0" \
	"loadbootimg=" \
		"${bloader} 0x20000000 boot.img\0" \
	"consoleargs=" \
		"console=ttyS0,115200 no_console_suspend " \
		"earlyprintk=aml-uart,0xc81004c0\0" \
	"droidargs=" \
		"androidboot.console=ttyS0 androidboot.hardware=nanopi-k2\0" \
	"rootargs=" \
		"root=/dev/mmcblk0p2 rootfstype=ext4 rootwait init=/sbin/init\0" \
	"init_bootargs=" \
		"setenv bootargs ${consoleargs} ${rootargs} " \
		  "hdmimode=${hdmimode} hdmitx=cecf logo=osd1,loaded,${fb_addr},${hdmimode} " \
		  "initrd=${initrd_start},${initrd_size}\0" \
	"init_display=" \
		"osd open; osd clear; ${bloader} 0x20000000 logo.bmp; bmp display 0x20000000\0" \
	"bootcmd=" \
		"run loadkernel; run loadinitrd; " \
		"run init_bootargs; " \
		"booti ${loadaddr} - ${dtb_mem_addr}\0"

#define CONFIG_PREBOOT  \
	"run loaddtb"
#define CONFIG_BOOTCOMMAND

/* Image support */
#define CONFIG_FIT                  1
#define CONFIG_OF_LIBFDT            1
#define CONFIG_ANDROID_BOOT_IMAGE   1
#define CONFIG_ANDROID_IMG          1
#define CONFIG_SYS_BOOTM_LEN        (64 << 20)	/* Increase max gunzip size */
#define CONFIG_SUPPORT_RAW_INITRD   1

/* Commands */
#define CONFIG_CMD_CACHE            1
#define CONFIG_CMD_BOOTI            1
#define CONFIG_CMD_EFUSE            1
#define CONFIG_CMD_I2C              1
#define CONFIG_CMD_MEMORY           1
#define CONFIG_CMD_FAT              1
#define CONFIG_CMD_EXT4             1
#define CONFIG_CMD_GPIO             1
#define CONFIG_CMD_RUN              1
#define CONFIG_CMD_REBOOT           1
#define CONFIG_CMD_ECHO             1
#define CONFIG_CMD_JTAG             1
#define CONFIG_CMD_AUTOSCRIPT       1
#define CONFIG_CMD_MISC             1
#define CONFIG_CMD_SAVEENV          1
#define CONFIG_CMD_BDI              1
#define CONFIG_CMD_BMP              1
#define CONFIG_CMD_BOOTD            1
#define CONFIG_CMD_CPU_TEMP         1
#define CONFIG_CMD_ITEST            1
//#define CONFIG_CMD_MEMTEST          1
//#define CONFIG_CMD_NET              1
#define CONFIG_USBDOWNLOAD_GADGET   1
#define CONFIG_CMD_FASTBOOT         1
#define CONFIG_CMD_USB              1
#define CONFIG_CMD_PART             1
#define CONFIG_PARTITION_UUIDS      1

/* File system */
#define CONFIG_PARTITIONS           1
#define CONFIG_DOS_PARTITION        1
#define CONFIG_MMC                  1
#define CONFIG_FS_FAT               1
#define CONFIG_FS_EXT4              1
#define CONFIG_LZO                  1

/* Storage: emmc/nand/sd */
#define CONFIG_SYS_NO_FLASH         1
#undef  CONFIG_AML_NAND
#define CONFIG_AML_SD_EMMC          1
//#define CONFIG_STORE_COMPATIBLE     1

#if defined(CONFIG_AML_SD_EMMC)
	#define CONFIG_GENERIC_MMC      1
	#define CONFIG_CMD_MMC          1
	#define CONFIG_EMMC_DDR52_EN    1
	#define CONFIG_EMMC_DDR52_CLK   52000000
#endif

/* ENV */
#define CONFIG_ENV_OVERWRITE        1
#undef  CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_IS_IN_MMC        1

#define CONFIG_SYS_MMC_ENV_DEV      0	/* 0: SD, 1: eMMC */
#define CONFIG_ENV_SIZE             ( 64 * 1024)
#define CONFIG_ENV_OFFSET           (800 * 1024)

#if defined(CONFIG_STORE_COMPATIBLE) && \
   (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC))
#error "ENV in amlnand/mmc already be compatible"
#endif

/* VPU */
#define CONFIG_AML_VPU              1

#if defined(CONFIG_AML_VPU)
#define CONFIG_VPU_PRESET           1
#endif

/* DISPLAY & HDMITX */
#define CONFIG_AML_HDMITX20         1
#define CONFIG_AML_CANVAS           1
#define CONFIG_AML_VOUT             1
#define CONFIG_AML_OSD              1
#define CONFIG_OSD_SCALE_ENABLE     1

#if defined(CONFIG_AML_VOUT)
#define CONFIG_AML_CVBS             1
#endif

/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC			1 */
#if defined(CONFIG_CMD_USB)
	#define CONFIG_M8_USBPORT_BASE_A    0xC9000000
	#define CONFIG_M8_USBPORT_BASE_B    0xC9100000
	#define CONFIG_USB_STORAGE      1
	#define CONFIG_USB_DWC_OTG_HCD  1
	#define CONFIG_USB_DWC_OTG_294  1
#endif	/* CONFIG_CMD_USB */

/* Gadget */
#define CONFIG_USB_GADGET           1
#if defined(CONFIG_USB_GADGET)
	#define CONFIG_USB_GADGET_S3C_UDC_OTG   1
	#define CONFIG_USB_GADGET_DUALSPEED     1
	#define CONFIG_USB_GADGET_VBUS_DRAW     2
	#define CONFIG_SYS_CACHELINE_SIZE       64
	#define CONFIG_USB_MAX_PACKET_SIZE      (0x0200)
#endif

#if defined(CONFIG_USBDOWNLOAD_GADGET)
	#define CONFIG_G_DNL_VENDOR_NUM     0x18d1
	#define CONFIG_G_DNL_PRODUCT_NUM    0x0002
	#define CONFIG_G_DNL_MANUFACTURER   "FriendlyElec Technology Co., Ltd."
#endif

/* Android fastboot */
#if defined(CONFIG_CMD_FASTBOOT)
	#define CONFIG_FASTBOOT_BUF_ADDR    (0x18000000)
	#define CONFIG_FASTBOOT_BUF_SIZE    (0x20000000)
	#define CONFIG_FASTBOOT_FLASH       1
	#if defined(CONFIG_FASTBOOT_FLASH)
		#define CONFIG_FASTBOOT_FLASH_MMC_DEV   0
	#endif
#endif

/* Facotry usb/sdcard burning config */
#if defined(CONFIG_STORE_COMPATIBLE)
#define CONFIG_AML_V2_FACTORY_BURN              1  // support facotry usb burning
#endif
#define CONFIG_AML_FACTORY_BURN_LOCAL_UPGRADE   1  // support factory sdcard burning
#define CONFIG_POWER_KEY_NOT_SUPPORTED_FOR_BURN 1  // There isn't power-key for factory sdcard burning
#define CONFIG_SD_BURNING_SUPPORT_UI            1  // Displaying upgrading progress bar when sdcard/udisk burning

/* Security */
#define CONFIG_AML_SECURE_UBOOT     1
#define CONFIG_AML_SECURITY_KEY     1

#if defined(CONFIG_STORE_COMPATIBLE)
#define CONFIG_INSTABOOT            1
#define CONFIG_SECURE_STORAGE       1
#define CONFIG_UNIFY_KEY_MANAGE     1
#endif

/* Net */
#if defined(CONFIG_CMD_NET)
//	#define CONFIG_DESIGNWARE_ETH   1
//	#define CONFIG_PHYLIB           1
	#define CONFIG_AML_ETHERNET     1
	#define CONFIG_NET_MULTI        1
	#define CONFIG_CMD_PING         1
	#define CONFIG_CMD_DHCP         1
	#define CONFIG_CMD_RARP         1
	#define CONFIG_HOSTNAME         nanopi-k2-u1
	#define CONFIG_ETHADDR          00:16:0F:A0:10:22
	#define CONFIG_IPADDR           192.168.102.39	/* Our ip address */
	#define CONFIG_GATEWAYIP        192.168.102.1	/* Gateway ip address */
	#define CONFIG_SERVERIP         192.168.102.30	/* Tftp server ip address */
	#define CONFIG_NETMASK          255.255.255.0
#endif	/* CONFIG_CMD_NET */

/* Other devices */
#define CONFIG_EFUSE                1
#define CONFIG_SYS_I2C_AML          1
#define CONFIG_SYS_I2C_SPEED        400000

/* Cache Definitions */
//#define CONFIG_SYS_DCACHE_OFF
//#define CONFIG_SYS_ICACHE_OFF

/* Other functions */
#define CONFIG_NEED_BL301           1
#define CONFIG_NEED_BL32            1
//#define CONFIG_AML_UBOOT_AUTO_TEST  1
//#define CONFIG_CMD_RSVMEM           1
#define CONFIG_FIP_IMG_SUPPORT      1
#define CONFIG_SYS_LONGHELP         1
#define CONFIG_SYS_MEM_TOP_HIDE     0x08000000 // hide 128MB for kernel reserve

/* Secure boot */
#if defined(CONFIG_AML_SECURE_UBOOT)
//for GXBB SRAM size limitation just disable NAND
//as the socket board default has no NAND
//#undef CONFIG_AML_NAND

//unify build for generate encrypted bootloader "u-boot.bin.encrypt"
#define CONFIG_AML_CRYPTO_UBOOT     1

//unify build for generate encrypted kernel image
//SRC : "board/amlogic/gxb_skt_v1/boot.img"
//DST : "fip/boot.img.encrypt"
//#define CONFIG_AML_CRYPTO_IMG       1

#endif	/* CONFIG_AML_SECURE_UBOOT */

/* Board customer ID */
//#define CONFIG_CUSTOMER_ID        (0x6472616F624C4D41)

#if defined(CONFIG_CUSTOMER_ID)
	#undef  CONFIG_AML_CUSTOMER_ID
	#define CONFIG_AML_CUSTOMER_ID  CONFIG_CUSTOMER_ID
#endif

#endif /* __NANOPI_K2_H__ */

