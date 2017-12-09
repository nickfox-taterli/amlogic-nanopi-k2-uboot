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
#include <asm/arch/secure_apb.h>
#include <asm/arch/eth_setup.h>
#include <phy.h>

#if defined(CONFIG_AML_ETHERNET)
struct eth_board_socket *eth_board_setup(char *name)
{
	struct eth_board_socket *new_board;

	new_board = (struct eth_board_socket *)
		malloc(sizeof(struct eth_board_socket));
	if (!new_board)
		return NULL;

	if (name != NULL) {
		new_board->name = (char *)malloc(strlen(name));
		strncpy(new_board->name, name, strlen(name));
	} else {
		new_board->name = "gxb";
	}

	new_board->eth_pinmux_setup = NULL;
	new_board->eth_clock_configure = NULL;
	new_board->eth_hw_reset = NULL;

	return new_board;
}

void setup_net_chip(void)
{
	eth_aml_reg0_t reg;

	/* Setup ethernet clk need calibrate to configre */
	setbits_le32(P_PERIPHS_PIN_MUX_6, 0x3fff);

	reg.d32 = 0;
	reg.b.phy_intf_sel = 1;
	reg.b.data_endian = 0;
	reg.b.desc_endian = 0;
	reg.b.rx_clk_rmii_invert = 0;
	reg.b.rgmii_tx_clk_src = 0;
	reg.b.rgmii_tx_clk_phase = 1;
	reg.b.rgmii_tx_clk_ratio = 4;
	reg.b.phy_ref_clk_enable = 1;
	reg.b.clk_rmii_i_invert = 0;
	reg.b.clk_en = 1;
	reg.b.adj_enable = 0;
	reg.b.adj_setup = 0;
	reg.b.adj_delay = 0;
	reg.b.adj_skew = 0;
	reg.b.cali_start = 0;
	reg.b.cali_rise = 0;
	reg.b.cali_sel = 0;
	reg.b.rgmii_rx_reuse = 0;
	reg.b.eth_urgent = 0;

	/* RGMII mode */
	setbits_le32(P_PREG_ETH_REG0, reg.d32);

	setbits_le32(HHI_GCLK_MPEG1, (1 << 3));

	/* Power on memory */
	clrbits_le32(HHI_MEM_PD_REG0, (1 << 3) | (1 << 2));

	/* Hardware reset ethernet phy : GPIOZ_14 */
#define GPIO_HW_RST		(1 << 14)
	clrbits_le32(PREG_PAD_GPIO3_EN_N, GPIO_HW_RST);
	clrbits_le32(PREG_PAD_GPIO3_O, GPIO_HW_RST);
	udelay(50000);
	setbits_le32(PREG_PAD_GPIO3_O, GPIO_HW_RST);
}
#endif

extern int aml_eth_init(bd_t *bis);
extern int designware_initialize(ulong base_addr, u32 interface);

int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_AML_ETHERNET)
	setup_net_chip();
#endif
	udelay(1000);

#if defined(CONFIG_AML_ETHERNET)
	aml_eth_init(bis);
#elif defined(CONFIG_DESIGNWARE_ETH)
	designware_initialize(ETH_BASE, PHY_INTERFACE_MODE_RMII);
#endif

	return 0;
}

