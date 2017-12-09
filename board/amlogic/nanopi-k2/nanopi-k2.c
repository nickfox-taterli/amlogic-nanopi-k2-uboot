/*
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

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <environment.h>
#include <fdt_support.h>
#include <libfdt.h>

#ifdef CONFIG_SYS_I2C_AML
#include <aml_i2c.h>
#include <asm/arch/secure_apb.h>
#endif
#ifdef CONFIG_AML_VPU
#include <vpu.h>
#endif
#ifdef CONFIG_AML_V2_FACTORY_BURN
#include <amlogic/aml_v2_burning.h>
#endif
#ifdef CONFIG_AML_HDMITX20
#include <amlogic/hdmi.h>
#endif

//#define AML_DEBUG		1
#if defined(AML_DEBUG)
#define aml_dbg(arg...)	printf(## arg)
#else
#define aml_dbg(arg...)
#endif

DECLARE_GLOBAL_DATA_PTR;

int serial_set_pin_port(unsigned long port_base)
{
	//UART in "Always On Module"
	//GPIOAO_0==tx,GPIOAO_1==rx
	//setbits_le32(P_AO_RTI_PIN_MUX_REG,3<<11);
	return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

/*
 * secondary_boot_func
 * - this function should be written in ASM, here, it is only
 * - for compiling pass
 */
void secondary_boot_func(void)
{
}

int get_boot_device(void)
{
	return readl(AO_SEC_GP_CFG0) & 0xf;
}

#if CONFIG_AML_SD_EMMC
#include <mmc.h>
#include <asm/arch/sd_emmc.h>

static int sd_emmc_init(unsigned port)
{
	switch (port) {
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//todo add card detect
			//setbits_le32(P_PREG_PAD_GPIO5_EN_N,1<<29);//CARD_6
			break;
		case SDIO_PORT_C:
			//enable pull up
			//clrbits_le32(P_PAD_PULL_UP_REG3, 0xff<<0);
			break;
		default:
			break;
	}

	return cpu_sd_emmc_init(port);
}

extern unsigned sd_debug_board_1bit_flag;
static int sd_emmc_detect(unsigned port)
{
	int ret;

	switch (port) {
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			setbits_le32(P_PREG_PAD_GPIO5_EN_N, 1<<29); //CARD_6
			ret = readl(P_PREG_PAD_GPIO5_I) & (1<<29) ? 0 : 1;
			if (!ret)
				aml_dbg("PORT_B: No card detect");

			if ((readl(P_PERIPHS_PIN_MUX_8) & (3<<9))) {
				//if uart pinmux set, debug board in
				if (!(readl(P_PREG_PAD_GPIO2_I)&(1<<24))) {
					printf("sdio debug board detected, sd card with 1bit mode\n");
					sd_debug_board_1bit_flag = 1;
				}
				else {
					printf("sdio debug board detected, no sd card in\n");
					sd_debug_board_1bit_flag = 0;
					return 1;
				}
			}
			break;
		default:
			break;
	}

	return 0;
}

static void sd_emmc_pwr_prepare(unsigned port)
{
	cpu_sd_emmc_pwr_prepare(port);
}

static void sd_emmc_pwr_on(unsigned port)
{
	switch (port) {
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//clrbits_le32(P_PREG_PAD_GPIO5_O,(1<<31)); //CARD_8
			//clrbits_le32(P_PREG_PAD_GPIO5_EN_N,(1<<31));
			/// @todo NOT FINISH
			break;
		case SDIO_PORT_C:
			break;
		default:
			break;
	}
	return;
}

static void sd_emmc_pwr_off(unsigned port)
{
	/// @todo NOT FINISH
	switch (port)
	{
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			//setbits_le32(P_PREG_PAD_GPIO5_O,(1<<31)); //CARD_8
			//clrbits_le32(P_PREG_PAD_GPIO5_EN_N,(1<<31));
			break;
		case SDIO_PORT_C:
			break;
				default:
			break;
	}
	return;
}

static void board_mmc_register(unsigned port)
{
	struct aml_card_sd_info *aml_priv = cpu_sd_emmc_get(port);
	if (aml_priv == NULL)
		return;

	aml_priv->sd_emmc_init=sd_emmc_init;
	aml_priv->sd_emmc_detect=sd_emmc_detect;
	aml_priv->sd_emmc_pwr_off=sd_emmc_pwr_off;
	aml_priv->sd_emmc_pwr_on=sd_emmc_pwr_on;
	aml_priv->sd_emmc_pwr_prepare=sd_emmc_pwr_prepare;

	aml_priv->desc_buf = malloc(NEWSD_MAX_DESC_MUN *
			(sizeof(struct sd_emmc_desc_info)));
	if (NULL == aml_priv->desc_buf)
		printf("desc_buf Dma alloc Fail!\n");
	else {
		aml_dbg("aml_priv->desc_buf = 0x%p\n", aml_priv->desc_buf);
	}

	sd_emmc_register(aml_priv);
}

int board_mmc_init(bd_t *bis)
{
#ifdef CONFIG_VLSI_EMULATOR
	//board_mmc_register(SDIO_PORT_A);
#else
	//board_mmc_register(SDIO_PORT_B);
#endif
	board_mmc_register(SDIO_PORT_B);
	board_mmc_register(SDIO_PORT_C);
	//board_mmc_register(SDIO_PORT_B1);
	return 0;
}

#ifdef CONFIG_SYS_I2C_AML
#if 0
static void board_i2c_set_pinmux(void) {
	/******************************************************/
	/*           | I2C_Master_AO     | I2C_Slave        | */
	/*----------------------------------------------------*/
	/*           | I2C_SCK           | I2C_SCK_SLAVE    | */
	/* GPIOAO_4  | [AO_PIN_MUX: 6]   | [AO_PIN_MUX: 2]  | */
	/*----------------------------------------------------*/
	/*           | I2C_SDA           | I2C_SDA_SLAVE    | */
	/* GPIOAO_5  | [AO_PIN_MUX: 5]   | [AO_PIN_MUX: 1]  | */
	/******************************************************/

	//disable all other pins which share with I2C_SDA_AO & I2C_SCK_AO
	clrbits_le32(P_AO_RTI_PIN_MUX_REG, ((1<<2)|(1<<24)|(1<<1)|(1<<23)));
	//enable I2C MASTER AO pins
	setbits_le32(P_AO_RTI_PIN_MUX_REG,
			(MESON_I2C_MASTER_AO_GPIOAO_4_BIT | MESON_I2C_MASTER_AO_GPIOAO_5_BIT));

	udelay(10);
};
#endif

struct aml_i2c_platform g_aml_i2c_plat = {
	.wait_count         = 1000000,
	.wait_ack_interval  = 5,
	.wait_read_interval = 5,
	.wait_xfer_interval = 5,
	.master_no          = AML_I2C_MASTER_AO,
	.use_pio            = 0,
	.master_i2c_speed   = AML_I2C_SPPED_400K,
	.master_ao_pinmux = {
		.scl_reg    = (unsigned long)MESON_I2C_MASTER_AO_GPIOAO_4_REG,
		.scl_bit    = MESON_I2C_MASTER_AO_GPIOAO_4_BIT,
		.sda_reg    = (unsigned long)MESON_I2C_MASTER_AO_GPIOAO_5_REG,
		.sda_bit    = MESON_I2C_MASTER_AO_GPIOAO_5_BIT,
	}
};

#if 0
static void board_i2c_init(void)
{
	//set I2C pinmux with PCB board layout
	board_i2c_set_pinmux();

	//Amlogic I2C controller initialized
	//note: it must be call before any I2C operation
	aml_i2c_init();

	udelay(10);
}
#endif
#endif
#endif /* CONFIG_AML_SD_EMMC */

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
	/* add board early init function here */
	return 0;
}
#endif

#ifdef CONFIG_USB_DWC_OTG_HCD
#include <asm/arch/usb.h>

static int usb_charging_detect_call_back(char bc_mode)
{
	switch (bc_mode) {
		case BC_MODE_DCP:
		case BC_MODE_CDP:
			//Pull up chargging current > 500mA
			break;

		case BC_MODE_UNKNOWN:
		case BC_MODE_SDP:
		default:
			//Limit chargging current <= 500mA
			//Or detet dec-charger
			break;
	}
	return 0;
}

//note: try with some M3 pll but only following can work
//USB_PHY_CLOCK_SEL_M3_XTAL      @  1 (24MHz)
//USB_PHY_CLOCK_SEL_M3_XTAL_DIV2 @  0 (12MHz)
//USB_PHY_CLOCK_SEL_M3_DDR_PLL   @ 27 (336MHz); @Rev2663 M3 SKT board DDR is 336MHz
//                                 43 (528MHz); M3 SKT board DDR not stable for 528MHz

struct amlogic_usb_config usb_config_a = {
	.clk_selecter	= USB_PHY_CLK_SEL_XTAL,
	.pll_divider	= 1,	// (clock/12 - 1)
	.base_addr		= CONFIG_M8_USBPORT_BASE_A,
	.id_mode		= USB_ID_MODE_SW_HOST,
	.set_vbus_power	= NULL,
};

struct amlogic_usb_config usb_config_b = {
	.clk_selecter	= USB_PHY_CLK_SEL_XTAL,
	.pll_divider	= 1,
	.base_addr		= CONFIG_M8_USBPORT_BASE_B,
	.id_mode		= USB_ID_MODE_SW_HOST,
	.set_vbus_power	= NULL,
};

struct amlogic_usb_config usb_config_h = {
	.clk_selecter	= USB_PHY_CLK_SEL_XTAL,
	.pll_divider	= 1,
	.base_addr		= CONFIG_M8_USBPORT_BASE_A,
	.id_mode		= USB_ID_MODE_HARDWARE,
	.set_vbus_power	= NULL,
	.battery_charging_det_cb	= usb_charging_detect_call_back,
};

struct amlogic_usb_config *usb_get_config(int port)
{
	if (port == 0)
		return &usb_config_a;
	else if (port == 1)
		return &usb_config_b;

	return NULL;
}
#endif /* CONFIG_USB_DWC_OTG_HCD */

#ifdef CONFIG_AML_HDMITX20
static void hdmi_tx_set_hdmi_5v(void)
{
	/*Power on VCC_5V for HDMI_5V*/
	clrbits_le32(P_AO_GPIO_O_EN_N, ((1<<2)|(1<<18)));
}
#endif

int board_init(void)
{
	//Please keep CONFIG_AML_V2_FACTORY_BURN at first place of board_init
#ifdef CONFIG_AML_V2_FACTORY_BURN
	aml_try_factory_usb_burning(0, gd->bd);
#endif

	/* for LED */
	//clear pinmux
	clrbits_le32(AO_RTI_PIN_MUX_REG, ((1<<3)|(1<<4)));
	clrbits_le32(AO_RTI_PIN_MUX_REG2, ((1<<1)|(1<<31)));
	//set output mode
	clrbits_le32(P_AO_GPIO_O_EN_N, (1<<13));
	//set output 1
	setbits_le32(P_AO_GPIO_O_EN_N, (1<<29));

	/* Power on GPIOAO_2 for VCC_5V */
	clrbits_le32(P_AO_GPIO_O_EN_N, ((1<<2)|(1<<18)));

#ifdef CONFIG_USB_DWC_OTG_HCD
	amlogic_usb_init(&usb_config_a, BOARD_USB_MODE_SLAVE);
	amlogic_usb_init(&usb_config_b, BOARD_USB_MODE_HOST);
	amlogic_usb_init(&usb_config_h, BOARD_USB_MODE_CHARGER);
#endif

#ifdef CONFIG_AML_VPU
	vpu_probe();
#endif

#ifndef CONFIG_AML_IRDETECT_EARLY
#ifdef CONFIG_AML_HDMITX20
	hdmi_tx_set_hdmi_5v();
	hdmi_tx_init();
#endif
#endif

#ifdef CONFIG_AML_NAND
	extern int amlnf_init(unsigned char flag);
	amlnf_init(0);
#endif

	return 0;
}

#ifdef CONFIG_AML_IRDETECT_EARLY
#ifdef CONFIG_AML_HDMITX20
static int do_hdmi_init(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	hdmi_tx_set_hdmi_5v();
	hdmi_tx_init();
	return 0;
}

U_BOOT_CMD(hdmi_init, CONFIG_SYS_MAXARGS, 0, do_hdmi_init,
		"HDMI_INIT sub-system",
		"hdmit init\n")
#endif
#endif

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void){
#ifdef CONFIG_STORE_COMPATIBLE
	int ret;
#endif

	//update env before anyone using it
	run_command("get_rebootmode; echo reboot_mode=${reboot_mode}; "\
				"if test ${reboot_mode} = factory_reset; then "\
				"defenv_reserv aml_dt;setenv upgrade_step 2;save; fi;", 0);
	run_command("if itest ${upgrade_step} == 1; then "\
				"defenv_reserv; setenv upgrade_step 2; saveenv; fi;", 0);

#ifndef CONFIG_AML_IRDETECT_EARLY
	/* after */
	run_command("cvbs init;hdmitx hpd", 0);
	run_command("vout output $outputmode", 0);
#endif

	/*add board late init function here*/
#ifdef CONFIG_STORE_COMPATIBLE
	ret = run_command("store dtb read $dtb_mem_addr", 1);
	if (ret) {
		printf("%s(): [store dtb read $dtb_mem_addr] fail\n", __func__);
		#ifdef CONFIG_DTB_MEM_ADDR
		char cmd[64];
		printf("load dtb to %x\n", CONFIG_DTB_MEM_ADDR);
		sprintf(cmd, "store dtb read %x", CONFIG_DTB_MEM_ADDR);
		ret = run_command(cmd, 1);
		if (ret) {
			printf("%s(): %s fail\n", __func__, cmd);
		}
		#endif
	}
#endif

#ifdef CONFIG_AML_V2_FACTORY_BURN
	aml_try_factory_sdcard_burning(0, gd->bd);
#endif

	return 0;
}
#endif

phys_size_t get_effective_memsize(void)
{
	// >>16 -> MB, <<20 -> real size, so >>16<<20 = <<4
#if defined(CONFIG_SYS_MEM_TOP_HIDE)
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4) - CONFIG_SYS_MEM_TOP_HIDE;
#else
	return (((readl(AO_SEC_GP_CFG0)) & 0xFFFF0000) << 4);
#endif
}
