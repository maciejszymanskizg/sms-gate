/**************************************************************************************
 *
 * Strictly internal and proprietary.
 *
 * Definition:  USB interface module
 *
 * Author:      Maciej Szymanski (maciej.szymanski.zg@gmail.com)
 * Contributor:
 *
 * Copyright: Maciej Szymanski
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <libudev.h>
#include <libusb-1.0/libusb.h>
#include <vector>
#include "device_manager.h"
#include "usb_iface.h"
#include "usb_iface_internal.h"
#include "debug.h"

#define USB_IFACE_NAME "USB Interface"

#define USB_IFACE_UDEV_ADD_EVENT_STRING "add"
#define USB_IFACE_UDEV_REMOVE_EVENT_STRING "remove"

static uint32_t usb_iface_id = 0;
static uint32_t usb_thread_work_flag = 1;
static std::vector<usb_iface_device_t *> usb_iface_devices;
dm_iface_t usb_iface;

static void USB_IFACE_Constructor(void) __attribute__ ((constructor));

static bool USB_IFACE_FindDevice(uint16_t vid, uint16_t pid, std::vector<usb_iface_device_t *>::iterator &it)
{
	bool result = false;
	for (it = usb_iface_devices.begin(); it != usb_iface_devices.end(); it++) {
		if (((*it)->vid == vid) && ((*it)->pid == pid)) {
			result = true;
			break;
		}
	}

	return result;
}

static bool USB_IFACE_FindDeviceByNode(const char *device_path, std::vector<usb_iface_device_t *>::iterator &it)
{
	bool result = false;
	uint8_t i = 0;

	for (it = usb_iface_devices.begin(); it != usb_iface_devices.end(); it++) {
		for (i = 0; i < (*it)->nodes_count; i++) {
			if (strcmp((*it)->nodes[i].device_path, device_path) == 0) {
				result = true;
				break;
			}
		}
	}
	return result;
}

static void USB_IFACE_HandleAddEvent(struct udev_device *dev)
{

}

static void USB_IFACE_HandleRemoveEvent(struct udev_device *dev)
{

}

static void *USB_IFACE_MonitorThread(void *arg)
{
	struct udev_monitor *monitor = (struct udev_monitor *) arg;

	if (monitor != NULL) {
		struct udev_device *dev = NULL;
		fd_set fds;
		struct timeval tv;
		int ret;
		int fd;

		udev_monitor_filter_add_match_subsystem_devtype(monitor, "tty", NULL);
		udev_monitor_enable_receiving(monitor);

		while (usb_thread_work_flag) {
			fd = udev_monitor_get_fd(monitor);

			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			tv.tv_sec = 0;
			tv.tv_usec = 0;

			ret = select(fd + 1, &fds, NULL, NULL, &tv);

			if ((ret > 0) && (FD_ISSET(fd, &fds))) {
				dev = udev_monitor_receive_device(monitor);

				if (dev != NULL) {

					if (strcmp(udev_device_get_action(dev), USB_IFACE_UDEV_ADD_EVENT_STRING) == 0) {
						USB_IFACE_HandleAddEvent(dev);
					}
					else { /* remove */
						USB_IFACE_HandleRemoveEvent(dev);
					}

					udev_device_unref(dev);
				}
			}

		}

		udev_monitor_unref(monitor);
	}
	else {
		DBG_Error("%s: Monitor object NULL.\n", __func__);
	}
	return NULL;
}

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

static void USB_IFACE_Constructor(void) {
	usb_iface.iface_type = DM_IFACE_TYPE_USB;
	usb_iface.iface_initialize = USB_IFACE_Initialize;
	usb_iface.iface_deinitialize = USB_IFACE_Deinitialize;
	usb_iface.iface_enumerate = USB_IFACE_Enumerate;
	usb_iface.iface_register_callback = USB_IFACE_RegisterCallback;
	strcpy(usb_iface.iface_name, USB_IFACE_NAME);
}
