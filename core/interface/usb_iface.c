#include <stdio.h>
#include <stdlib.h>
#include "device_manager.h"
#include "usb_iface.h"

#define USB_IFACE_NAME "USB Interface"

static uint32_t usb_iface_id = 0;

int32_t USB_IFACE_Initialize(uint32_t id)
{
	int32_t res = 0;
	usb_iface_id = id;

	return res;
}

int32_t USB_IFACE_Deinitialize(void)
{
	int32_t res = 0;

	return res;
}

int32_t USB_IFACE_Enumerate(dm_items_t *items)
{
	int32_t res = -1;

	if (items != NULL) {
		items->items_count = 0;
		res = 0;
	}

	return res;
}

int32_t USB_IFACE_RegisterCallback(add_remove_callback_f callback, void *user_data)
{
	int32_t res = 0;

	return res;
}

dm_iface_t usb_iface = {
	.iface_name = USB_IFACE_NAME,
	.iface_type = DM_IFACE_TYPE_USB,
	.iface_initialize = USB_IFACE_Initialize,
	.iface_deinitialize = USB_IFACE_Deinitialize,
	.iface_enumerate = USB_IFACE_Enumerate,
	.iface_register_callback = USB_IFACE_RegisterCallback,
};
