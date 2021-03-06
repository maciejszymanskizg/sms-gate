/**************************************************************************************
 *
 * Strictly internal and proprietary.
 *
 * Definition:  SMS Gate main code
 *
 * Author:      Maciej Szymanski (maciej.szymanski.zg@gmail.com)
 * Contributor:
 *
 * Copyright: Maciej Szymanski
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "usb_iface.h"
#include "device_manager.h"
#include "debug.h"

#define SMS_GATE_VERSION_MAJOR 0
#define SMS_GATE_VERSION_MINOR 1

static void SG_HandleSignal(int signum)
{
	DM_Deinitialize();
	exit(EXIT_SUCCESS);
}

static void SG_RegisterInterfaces(void)
{
	DBG_Info("Registering interfaces...\n");
	DM_RegisterIface(&usb_iface);
}

int main(int argc, char *argv[])
{
	/* Register signal handlers */
	signal(SIGTERM, SG_HandleSignal);
	signal(SIGINT, SG_HandleSignal);

	DBG_Info("SMS Gate version %u.%u, compilation %s %s\n", SMS_GATE_VERSION_MAJOR,
			SMS_GATE_VERSION_MINOR, __DATE__, __TIME__);

	if (DM_Initialize() == 0) {
		SG_RegisterInterfaces();

		DBG_Info("Entering endless loop...\n");
		/* Enter endless loop */
		while (1) {
			sleep(1);
		}
	}
	else {
		DBG_Error("Could not initialize device manager.\n");
	}
	return 0;
}
