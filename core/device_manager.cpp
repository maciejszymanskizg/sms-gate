#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "device_manager.h"
#include <semaphore.h>
#include <map>

static std::map<uint32_t, dm_item_t *> dm_items;
static sem_t dm_sem;
static uint32_t dm_counter = 1;

static void DM_FreeItem(dm_item_t *item)
{
	if (item != NULL) {
		if (item->nodes != NULL) {
			free(item->nodes);
		}

		free(item);
	}
}

static void DM_AddItemToMap(dm_item_t *item)
{
	item->device_id = dm_counter;
	dm_items.insert(std::pair<uint32_t, dm_item_t *>(dm_counter, item));
	dm_counter++;
}

int32_t DM_Initialize(void)
{
	int32_t res = sem_init(&dm_sem, 0, 1);

	if (res != 0) {
		
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
