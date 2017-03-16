#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "device_manager_interface.h"
#include "device_manager_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t DM_Initialize(void);
int32_t DM_Deinitialize(void);
int32_t DM_RegisterIface(const dm_iface_t *iface);
int32_t DM_UnregisterIface(const char *iface_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEVICE_MANAGER_H */
