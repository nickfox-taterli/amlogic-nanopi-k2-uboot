/*
 * Copyright (C) Guangzhou FriendlyARM Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
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
#include <asm/arch/usb.h>

#include <usb.h>
#include <usb/s3c_udc.h>

/* Port - A */
#define USB_OTG_PORT        0

#define DWC_REG_BASE        0xC9000000
#define DWC_REG_GSNPSID     (DWC_REG_BASE + 0x040) /* Synopsys ID Register (RO) */

#define PHY_REG_BASE        0xC0000000
#define PHY_REG_CONFIG      (PHY_REG_BASE + 0x000)
#define PHY_REG_CTRL        (PHY_REG_BASE + 0x004)
#define PHY_REG_TUNE        (PHY_REG_BASE + 0x018)

#define P_RESET1_REG        0xC1104408

extern struct amlogic_usb_config *usb_get_config(int port);

static int dwc2_oth_phy_control(int enable)
{
	u32 val;

	if (enable) {
		writel((1 << 2), P_RESET1_REG);
		udelay(500);

		val = readl(PHY_REG_CTRL);

		/* Select 24MHz (101b) as reference clock and Reset */
		val |= (5 << 22) | (1 << 15);
		writel(val, PHY_REG_CTRL);
		udelay(500);

		val &= ~(1 << 15);
		writel(val, PHY_REG_CTRL);
		udelay(50000);

		val = readl(PHY_REG_CTRL);
		if (!(val & (1 << 8))) {
			printf("Error: USB phy clock not detected\n");
			return -1;
		}

		val = readl(DWC_REG_GSNPSID) & 0xfffff000;
		if (val != 0x4F542000 && val != 0x4F543000) {
			printf("Error: Unknown SNPSID 0x%08x\n", val);
			return -1;
		}

	} else {
		/* TODO: Disable PHY */
	}

	return 0;
}

struct s3c_plat_otg_data dwc2_otg_data = {
	.regs_otg		= DWC_REG_BASE,
	.phy_control	= dwc2_oth_phy_control,
};

void otg_phy_init(struct s3c_udc *dev)
{
	dev->pdata->phy_control(1);

	printf("USB PHY0 Enabled\n");
}

void otg_phy_off(struct s3c_udc *dev)
{
	dev->pdata->phy_control(0);

	printf("USB PHY0 Disabled\n");
}

int board_usb_init(int index, enum usb_init_type init)
{
	amlogic_usb_config_t *config;

	if (init == USB_INIT_DEVICE) {
		config = board_usb_start(BOARD_USB_MODE_SLAVE, USB_OTG_PORT);
		if (usb_get_config(USB_OTG_PORT) == config)
			return s3c_udc_probe(&dwc2_otg_data);
	}

	return -ENODEV;
}

int board_usb_cleanup(int index, enum usb_init_type init)
{
	if (init == USB_INIT_DEVICE)
		board_usb_stop(BOARD_USB_MODE_SLAVE, USB_OTG_PORT);

	return 0;
}
