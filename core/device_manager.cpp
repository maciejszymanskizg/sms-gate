/**************************************************************************************
 *
 * Strictly internal and proprietary.
 *
 * Definition:  Device Manager module
 *
 * Author:      Maciej Szymanski (maciej.szymanski.zg@gmail.com)
 * Contributor:
 *
 * Copyright: Maciej Szymanski
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "device_manager.h"
#include "debug.h"
#include <semaphore.h>
#include <list>
#include <map>

static std::list<dm_item_t *> dm_items;
static std::map<uint32_t, dm_iface_t *> dm_ifaces;
static sem_t dm_sem;
static uint32_t dm_iface_id = 1;

static void DM_FreeItem(dm_item_t *item)
{
	if (item != NULL) {
		uint32_t i = 0;

		for (i = 0; i < item->nodes_count; i++) {
			free(item->nodes[i]);
		}

		free(item);
	}
}

static void DM_Callback(dm_item_t *item, dm_iface_action_t action, void *user_data)
{
	if (item != NULL) {
		DBG_Info("%s: action %s, iface 0x%x, device [%s]\n", __func__,
				(action == DM_IFACE_ACTION_ADD ? "ADD" : "REMOVE"), item->iface_id,
				item->device_name);

		sem_wait(&dm_sem);

		std::list<dm_item_t *>::iterator it = dm_items.end();

		for (it = dm_items.begin(); it != dm_items.end(); it++) {
			if ((item->iface_id == (*it)->iface_id) &&
					(item->device_id == (*it)->device_id))
				break;
		}

		if (action == DM_IFACE_ACTION_ADD) {
			if (it != dm_items.end())
				DBG_Warning("%s: Item %p already found in items list.\n", __func__, item);
			else {
				DBG_Info("%s: Add new item %p to list.\n", __func__, item);
				dm_items.push_back(item);
			}
		}
		else {
			if (it == dm_items.end())
				DBG_Warning("%s: Item %p not found in items list.\n", __func__, item);
			else {
				DBG_Info("%s: Remove existing item %p from list.\n", __func__, item);
				dm_item_t *list_item = *it;
				dm_items.erase(it);
				DM_FreeItem(item);
				DM_FreeItem(list_item);
			}
		}

		sem_post(&dm_sem);
	}
}

int32_t DM_Initialize(void)
{
	int32_t res = sem_init(&dm_sem, 0, 1);

	if (res != 0) {
		DBG_Error("%s: Could not initialize semaphore (errno %d)\n", __func__, errno);
	}

	return res;
}

int32_t DM_Deinitialize(void)
{
	dm_item_t *item = NULL;
	dm_iface_t *iface = NULL;

	sem_wait(&dm_sem);

	while (dm_items.size() > 0) {
		item = *(dm_items.begin());
		dm_items.erase(dm_items.begin());
		DM_FreeItem(item);
	}

	while (dm_ifaces.size() > 0) {
		iface = dm_ifaces.begin()->second;
		iface->iface_deinitialize();
		dm_ifaces.erase(dm_ifaces.begin());
		free(iface);
	}

	sem_post(&dm_sem);
	sem_destroy(&dm_sem);

	return 0;
}

int32_t DM_RegisterIface(const dm_iface_t *iface)
{
	int32_t res = 0;

	if (iface != NULL) {
		std::map<uint32_t, dm_iface_t *>::const_iterator it;
		sem_wait(&dm_sem);

		for (it = dm_ifaces.begin(); it != dm_ifaces.end(); it++) {
			if (strcmp(it->second->iface_name, iface->iface_name) == 0) {
				/* found interface with same name already registered */
				DBG_Error("%s: Interface with name [%s] already found.\n", __func__, iface->iface_name);
				res = -1;
				break;
			}
		}

		if (res != -1) {
			/* at first enumerate available items */
			if (iface->iface_initialize(dm_iface_id) == 0) {
				if (iface->iface_register_callback(DM_Callback, NULL) == 0) {
					dm_items_t items;

					if (iface->iface_enumerate(&items) == 0) {
						dm_iface_t *dm_iface = (dm_iface_t *) malloc(sizeof(dm_iface_t));

						if (dm_iface != NULL) {
							uint32_t i = 0;

							DBG_Info("%s: Got initial %u items from iface [%s].\n", __func__,
									items.items_count, iface->iface_name);
							for (i = 0; i < items.items_count; i++) {
								dm_item_t *item = items.items[i];
								dm_items.push_back(item);
							}

							memcpy(dm_iface, iface, sizeof(dm_iface_t));
							DBG_Info("%s: Registering interface %p with name [%s] as id %u.\n", __func__,
									dm_iface, dm_iface->iface_name, dm_iface_id);
							dm_ifaces.insert(std::make_pair<uint32_t, dm_iface_t *>(dm_iface_id, dm_iface));
							dm_iface_id++;
						}
						else {
							DBG_Error("%s: Memory allocation error.\n", __func__);
							iface->iface_deinitialize();
							uint32_t i = 0;

							for (i = 0; i < items.items_count; i++) {
								dm_item_t *item = items.items[i];
								DM_FreeItem(item);
							}
						}
					}
					else {
						DBG_Error("%s: Enumeration error on iface [%s].\n", __func__, iface->iface_name);
						iface->iface_deinitialize();
						res = -1;
					}
				}
				else {
					DBG_Error("%s: Callback registration error on iface [%s].\n", __func__, iface->iface_name);
					iface->iface_deinitialize();
					res = -1;
				}
			}
			else {
				DBG_Error("%s: Iface initialization error on [%s].\n", __func__, iface->iface_name);
				res = -1;
			}
		}

		sem_post(&dm_sem);
	}

	return res;
}

int32_t DM_UnregisterIface(const char *iface_name)
{
	int32_t res = -1;
	std::map<uint32_t, dm_iface_t *>::iterator it;

	sem_wait(&dm_sem);

	/* free memory of the registered interfaces */
	for (it = dm_ifaces.begin(); it != dm_ifaces.end(); it++) {
		if (strcmp(it->second->iface_name, iface_name) == 0) {
			dm_iface_t *iface = it->second;
			dm_ifaces.erase(it);
			free(iface);
			res = 0;
			break;
		}
	}

	sem_post(&dm_sem);
	return res;
}
