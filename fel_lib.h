/*
 * Copyright (C) 2016 Bernhard Nortmann <bernhard.nortmann@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _SUNXI_TOOLS_FEL_LIB_H
#define _SUNXI_TOOLS_FEL_LIB_H

#include <stdbool.h>
#include <stdint.h>
#include "progress.h"
#include "soc_info.h"

/* USB identifiers for Allwinner device in FEL mode */
#define AW_USB_VENDOR_ID	0x1F3A
#define AW_USB_PRODUCT_ID	0xEFE8

typedef struct _felusb_handle felusb_handle; /* opaque data type */

/* More general FEL "device" handle, to be extended later */
typedef struct {
	felusb_handle *usb;
} feldev_handle;

/* FEL device management */

void feldev_init(void);
void feldev_done(feldev_handle *dev);

feldev_handle *feldev_open(int busnum, int devnum,
			   uint16_t vendor_id, uint16_t product_id);
void feldev_close(feldev_handle *dev);

/* FEL functions */

void aw_fel_get_version(feldev_handle *dev, struct aw_fel_version *buf);
void aw_fel_read(feldev_handle *dev, uint32_t offset, void *buf, size_t len);
void aw_fel_write(feldev_handle *dev, void *buf, uint32_t offset, size_t len);
void aw_fel_write_buffer(feldev_handle *dev, void *buf, uint32_t offset,
			 size_t len, bool progress);
void aw_fel_execute(feldev_handle *dev, uint32_t offset);

#endif /* _SUNXI_TOOLS_FEL_LIB_H */
