#ifndef DEVICE_MANAGER_TYPES_H
#define DEVICE_MANAGER_TYPES_H

#include <stdint.h>

#define DM_DEVICE_NAME_MAX_LEN      64
#define DM_DEVICE_NODE_NAME_MAX_LEN 32

typedef enum {
	DM_IFACE_TYPE_USB,
} dm_iface_type_t;


typedef enum {
	DM_IFACE_ACTION_ADD,
	DM_IFACE_ACTION_REMOVE,
} dm_iface_action_t;

typedef struct {
	char node_name[DM_DEVICE_NODE_NAME_MAX_LEN];
} dm_item_node_t;

typedef struct {
	uint32_t       device_id;
	char           device_name[DM_DEVICE_NAME_MAX_LEN];
	uint8_t        nodes_count;
	dm_item_node_t *nodes[];
} dm_item_t;

typedef struct {
	uint32_t items_count;
	dm_item_t *items[];
} dm_items_t;

typedef int32_t (*add_remove_callback_f)(dm_item_t *item, dm_iface_action_t action, void *user_data);

#endif /* DEVICE_MANAGER_TYPES_H */
