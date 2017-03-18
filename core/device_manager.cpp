#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "device_manager.h"
#include "debug.h"
#include <semaphore.h>
#include <list>
#include <map>

static std::map<uint32_t, dm_item_t *> dm_items;
static std::list<dm_iface_t *> dm_ifaces;
static sem_t dm_sem;
static uint32_t dm_counter = 1;

static void DM_AddItemToMap(dm_item_t *item)
{
	item->device_id = dm_counter;
	dm_items.insert(std::pair<uint32_t, dm_item_t *>(dm_counter, item));
	dm_counter++;
}

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

int32_t DM_Initialize(void)
{
	int32_t res = sem_init(&dm_sem, 0, 1);

	if (res != 0) {
		/* TODO */
	}

	return res;
}

int32_t DM_Deinitialize(void)
{
	dm_item_t *item = NULL;

	while (dm_items.size() > 0) {
		item = dm_items.begin()->second;
		dm_items.erase(dm_items.begin());
		DM_FreeItem(item);
	}
	return 0;
}

int32_t DM_RegisterIface(const dm_iface_t *iface)
{
	int32_t res = -1;
	std::list<dm_iface_t *>::const_iterator it;

	if (iface != NULL) {

		sem_wait(&dm_sem);
		for (it = dm_ifaces.begin(); it != dm_ifaces.end(); it++) {
			if ((*it)->iface_unique_id == iface->iface_unique_id) {
				/* found same unitque id already in the list */
				DBG_Error("%s: Unique id %u already found.\n", __func__, iface->iface_unique_id);
				break;
			}
		}

		if (res != -1) {
			/* at first enumerate available items */
			if (iface->iface_initialize() == 0) {
				dm_items_t items;
				uint32_t i = 0;

				if (iface->iface_enumerate(&items) == 0) {
					dm_iface_t *dm_iface = (dm_iface_t *) malloc(sizeof(dm_iface_t *));

					if (dm_iface != NULL) {
						for (i = 0; i < items.items_count; i++) {
							dm_item_t *item = items.items[i];
							DM_AddItemToMap(item);
						}

						memcpy(dm_iface, iface, sizeof(dm_iface_t));
						dm_ifaces.push_back(dm_iface);
						res = 0;
					}
					else {
						DBG_Error("%s: Memory allocation error.\n", __func__);
					}
				}
				else {
					DBG_Error("%s: Enumeration error on iface %u\n", __func__, iface->iface_unique_id);
				}
			}
			else {
				DBG_Error("%s: Iface initialization error on %u\n", __func__, iface->iface_unique_id);
			}
		}

		sem_post(&dm_sem);
	}

	return res;
}

int32_t DM_UnregisterIface(uint32_t iface_unique_id)
{
	int32_t res = -1;
	std::list<dm_iface_t *>::iterator it;

	/*at first remove all registered items */

	/* at the end free memory of the reistered interfaces */
	for (it = dm_ifaces.begin(); it != dm_ifaces.end(); it++) {
		if (iface_unique_id == (*it)->iface_unique_id) {
			dm_iface_t *iface = *it;
			dm_ifaces.erase(it);
			free(iface);
			res = 0;
			break;
		}
	}

	return res;
}
