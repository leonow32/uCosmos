#ifndef UCOSMOS_H
#define UCOSMOS_H

#include "../_config/uCosmos_config.h"
#include "console.h"
#include "log.h"
#include "task.h"

#define UCOSMOS_VERSION_MAJOR 5
#define UCOSMOS_VERSION_MINOR 0
#define UCOSMOS_VERSION_PATCH 1

void uCosmos_init(void);				// Call this function at the beginning of main()
void uCosmos_scheduler(void);			// Call this function at the end of main()

#endif
