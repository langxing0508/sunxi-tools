/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * display information about sunxi boot headers
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef unsigned char u8;
typedef unsigned char __u8;
typedef unsigned int u32;
typedef unsigned int __u32;
typedef signed int __s32;

/* boot_file_head copied from mksunxiboot */
/* boot head definition from sun4i boot code */
typedef struct boot_file_head
{
	u32  jump_instruction;   // one intruction jumping to real code
	u8   magic[8];           // ="eGON.BT0" or "eGON.BT1",  not C-style string.
	u32  check_sum;          // generated by PC
	u32  length;             // generated by PC
	u32  pub_head_size;      // the size of boot_file_head_t
	u8   pub_head_vsn[4];    // the version of boot_file_head_t
	u8   file_head_vsn[4];   // the version of boot0_file_head_t or boot1_file_head_t
	u8   Boot_vsn[4];        // Boot version
	u8   eGON_vsn[4];        // eGON version
	u8   platform[8];        // platform information
} boot_file_head_t;

typedef struct _boot_dram_para_t {
    __u32 dram_baseaddr;
    __u32 dram_clk;
    __u32 dram_type;
    __u32 dram_rank_num;
    __u32 dram_chip_density;
    __u32 dram_io_width;
    __u32 dram_bus_width;
    __u32 dram_cas;
    __u32 dram_zq;
    __u32 dram_odt_en;
    __u32 dram_size;
    __u32 dram_tpr0;
    __u32 dram_tpr1;
    __u32 dram_tpr2;
    __u32 dram_tpr3;
    __u32 dram_tpr4;
    __u32 dram_tpr5;
    __u32 dram_emr1;
    __u32 dram_emr2;
    __u32 dram_emr3;
} boot_dram_para_t;

typedef struct _normal_gpio_cfg {
    __u8 port;
    __u8 port_num;
    __u8 mul_sel;
    __u8 pull;
    __u8 drv_level;
    __u8 data;
    __u8 reserved[2];
} normal_gpio_cfg;

typedef struct _boot0_private_head_t {
    __u32 prvt_head_size;
    char prvt_head_vsn[4];
    boot_dram_para_t dram_para;
    __s32 uart_port;
    normal_gpio_cfg uart_ctrl[2];
    __s32 enable_jtag;
    normal_gpio_cfg jtag_gpio[5];
    normal_gpio_cfg storage_gpio[32];
    char storage_data[256];
} boot0_private_head_t;

typedef struct _boot0_file_head_t {
    boot_file_head_t boot_head;
    boot0_private_head_t prvt_head;
} boot0_file_head_t;

typedef struct _boot_core_para_t {
    __u32 user_set_clock;
    __u32 user_set_core_vol;
    __u32 vol_threshold;
} boot_core_para_t;

typedef struct _boot1_private_head_t {
    __u32 prvt_head_size;
    __u8 prvt_head_vsn[4];
    __s32 uart_port;
    normal_gpio_cfg uart_ctrl[2];
    boot_dram_para_t dram_para;
    char script_buf[32768];
    boot_core_para_t core_para;
    __s32 twi_port;
    normal_gpio_cfg twi_ctrl[2];
    __s32 debug_enable;
    __s32 hold_key_min;
    __s32 hold_key_max;
    __u32 work_mode;
    __u32 storage_type;
    normal_gpio_cfg storage_gpio[32];
    char storage_data[256];
} boot1_private_head_t;

typedef struct _boot1_file_head_t {
    boot_file_head_t boot_head;
    boot1_private_head_t prvt_head;
} boot1_file_head_t;

/* Not sure where this is, but should be in STORAGE DATA somewhere */
typedef struct _boot_sdcard_info_t {
    __s32 card_ctrl_num;
    __s32 boot_offset;
    __s32 card_no[4];
    __s32 speed_mode[4];
    __s32 line_sel[4];
    __s32 line_count[4];
} boot_sdcard_info_t;

#define BOOT0_MAGIC                     "eGON.BT0"
#define BOOT1_MAGIC                     "eGON.BT1"

void fail(char *msg) {
	perror(msg);
	exit(1);
}

void print_boot_file_head(boot_file_head_t *hdr)
{
	printf("Magic     : %.8s\n", hdr->magic);
	printf("Length    : %u\n", hdr->length);
	printf("HSize     : %u\n", hdr->pub_head_size);
	printf("HEAD ver  : %.4s\n", hdr->pub_head_vsn);
	printf("FILE ver  : %.4s\n", hdr->file_head_vsn);
	printf("BOOT ver  : %.4s\n", hdr->Boot_vsn);
	printf("eGON ver  : %.4s\n", hdr->eGON_vsn);
	printf("platform  : %.8s\n", hdr->platform);
}

void print_boot_dram_para(boot_dram_para_t *dram)
{
	printf("DRAM base : %p\n", (void *)(long)dram->dram_baseaddr);
	printf("DRAM clk  : %d\n", dram->dram_clk);
	printf("DRAM type : %d\n", dram->dram_type);
	printf("DRAM rank : %d\n", dram->dram_rank_num);
	printf("DRAM den  : %d\n", dram->dram_chip_density);
	printf("DRAM iow  : %d\n", dram->dram_io_width);
	printf("DRAM busw : %d\n", dram->dram_bus_width);
	printf("DRAM cas  : %d\n", dram->dram_cas);
	printf("DRAM zq   : %d\n", dram->dram_zq);
	printf("DRAM odt  : 0x%x\n", dram->dram_odt_en);
	printf("DRAM size : %d\n", dram->dram_size);
	printf("DRAM tpr0 : 0x%x\n", dram->dram_tpr0);
	printf("DRAM tpr1 : 0x%x\n", dram->dram_tpr1);
	printf("DRAM tpr2 : 0x%x\n", dram->dram_tpr2);
	printf("DRAM tpr3 : 0x%x\n", dram->dram_tpr3);
	printf("DRAM tpr4 : 0x%x\n", dram->dram_tpr4);
	printf("DRAM tpr5 : 0x%x\n", dram->dram_tpr5);
	printf("DRAM emr1 : 0x%x\n", dram->dram_emr1);
	printf("DRAM emr2 : 0x%x\n", dram->dram_emr2);
	printf("DRAM emr3 : 0x%x\n", dram->dram_emr3);
}

void print_normal_gpio_cfg(normal_gpio_cfg *gpio, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (gpio[i].port)
			printf(" GPIO %d   : port=%c%d, sel=%d, pull=%d, drv=%d, data=%d, reserved=%02x,%02x\n", i, 'A'+gpio[i].port-1, gpio[i].port_num, gpio[i].mul_sel, gpio[i].pull, gpio[i].drv_level, gpio[i].data, gpio[i].reserved[0], gpio[i].reserved[1]);
	}
}
void print_boot0_private_head(boot0_private_head_t *hdr)
{
	printf("FHSize    : %u\n", hdr->prvt_head_size);
	printf("FILE ver  : %.4s\n", hdr->prvt_head_vsn);
	print_boot_dram_para(&hdr->dram_para);
	printf("UART port : %d\n", hdr->uart_port);
	print_normal_gpio_cfg(hdr->uart_ctrl, 2);
	printf("JTAG en   : %d\n", hdr->enable_jtag);
	print_normal_gpio_cfg(hdr->jtag_gpio, 5);
	printf("STORAGE   :\n");
	print_normal_gpio_cfg(hdr->storage_gpio, 2);
	int i;
	for (i = 0; i < 256; i++) {
		if (i % 16 == 0) {
			if (i) {
				printf("\n");
			}
			printf(" DATA %02x  :", i);
		}
		printf(" %02x", (u8)hdr->storage_data[i]);
	}
	printf("\n");
}

void print_script(void *script)
{
}

void print_core_para(boot_core_para_t *core)
{
	printf("Set Clock : %d\n", core->user_set_clock);
	printf("Set Core Vol: %d\n", core->user_set_core_vol);
	printf("Vol Threshold: %d\n", core->vol_threshold);
}

void print_boot1_private_head(boot1_private_head_t *hdr)
{
	printf("FHSize    : %u\n", hdr->prvt_head_size);
	printf("FILE ver  : %.4s\n", hdr->prvt_head_vsn);
	printf("UART port : %d\n", hdr->uart_port);
	print_normal_gpio_cfg(hdr->uart_ctrl, 2);
	print_boot_dram_para(&hdr->dram_para);
	print_script(&hdr->script_buf);
	print_core_para(&hdr->core_para);
	printf("TWI port  : %d\n", hdr->twi_port);
	print_normal_gpio_cfg(hdr->twi_ctrl, 2);
	printf("Debug     : %d\n", hdr->debug_enable);
	printf("Hold key min : %d\n", hdr->hold_key_min);
	printf("Hold key max : %d\n", hdr->hold_key_max);
	printf("Work mode : %d\n", hdr->work_mode);
	printf("Storage   : %d\n", hdr->storage_type);
	int i;
	for (i = 0; i < 256; i++) {
		if (i % 16 == 0) {
			if (i) {
				printf("\n");
			}
			printf(" DATA %02x  :", i);
		}
		printf(" %02x", (u8)hdr->storage_data[i]);
	}
	printf("\n");
}

void print_boot0_file_head(boot0_file_head_t *hdr)
{
	print_boot_file_head(&hdr->boot_head);
	if (strncmp((char *)hdr->boot_head.file_head_vsn, "1230", 4) == 0)
		print_boot0_private_head(&hdr->prvt_head);
	else
		printf("Unknown boot0 header version\n");
}

void print_boot1_file_head(boot1_file_head_t *hdr)
{
	print_boot_file_head(&hdr->boot_head);
	if (strncmp((char *)hdr->boot_head.file_head_vsn, "1230", 4) == 0)
		print_boot1_private_head(&hdr->prvt_head);
	else
		printf("Unknown boot0 header version\n");
}

int main(int argc, char * argv[])
{
	FILE *in = stdin;
	if (argc > 1) {
		in = fopen(argv[1], "rb");
		if (!in)
			fail("open input: ");
	}
	union {
		boot_file_head_t boot;
		boot0_file_head_t boot0;
		boot1_file_head_t boot1;
	} hdr;
	int len;

	len = fread(&hdr, 1, sizeof(hdr), in);
	if (len < (int)sizeof(boot_file_head_t))
		fail("Failed to read header:");
	if (strncmp((char *)hdr.boot.magic, BOOT0_MAGIC, strlen(BOOT0_MAGIC)) == 0) {
		print_boot0_file_head(&hdr.boot0);
	} else if (strncmp((char *)hdr.boot.magic, BOOT1_MAGIC, strlen(BOOT1_MAGIC)) == 0) {
		print_boot1_file_head(&hdr.boot1);
	} else {
		fail("Invalid magic\n");
	}

	return 0;
}
