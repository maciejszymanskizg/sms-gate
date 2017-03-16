#ifndef DEVICE_MANAGER_INTERFACE_H
#define DEVICE_MANAGER_INTERFACE_H

#include "device_manager_types.h"

#define DM_IFACE_NAME_MAX_LEN	32

typedef int32_t (*initialize_f)(void);
typedef int32_t (*deinitialize_f)(void);
typedef int32_t (*enumerate_f)(uint32_t *items_count, dm_item_t *items);
typedef int32_t (*register_callback_f)(add_remove_callback_f callback, void *user_data);

typedef struct {
	const char          iface_name[DM_IFACE_NAME_MAX_LEN];
	dm_iface_type_t     iface_type;
	initialize_f        iface_initialize;
	deinitialize_f      iface_deinitialize;
	enumerate_f         iface_enumerate;
	register_callback_f iface_register_callback;
} dm_iface_t;

#endif /* DEVICE_MANAGER_INTERFACE_H */
