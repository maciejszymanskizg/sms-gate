#ifndef USB_IFACE_INTERNAL_H
#define USB_IFACE_INTERNAL_H

#define USB_IFACE_MAX_NODES (8)
#define USB_IFACE_DEVICE_NODE_MAX_PATH (32)

typedef struct {
	char device_path[USB_IFACE_DEVICE_NODE_MAX_PATH];
} usb_iface_node_t;

typedef struct {
	uint16_t pid;
	uint16_t vid;
	uint8_t nodes_count;
	usb_iface_node_t nodes[USB_IFACE_MAX_NODES];
	uint32_t notify_counter;
} usb_iface_device_t;

#endif /* USB_IFACE_INTERNAL_H */
