#pragma once

#define HAL_USE_SPI TRUE
#define SPI_USE_ASSERT_ON_ERROR FALSE // asserts useless without debugger
#define SPI_USE_MUTUAL_EXCLUSION FALSE
#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#define SPI_DIV 240 // 200 kHz

#include_next <halconf.h>
