#pragma once

#define HAL_USE_SPI TRUE

#define SPI_USE_WAIT FALSE
#define SPI_USE_MUTUAL_EXCLUSION FALSE
#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD

#include_next <halconf.h>

