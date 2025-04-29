#include "quantum.h"
#include "print.h"

#include "spi_master.h"

bool is_keyboard_master(void) {
    // println("is_keyboard_master");
    return true;
}

void transport_master_init(void) {
    println("transport_master_init");

    gpio_set_pin_output(SPI_MASTER_PIN);
    gpio_write_pin_high(SPI_MASTER_PIN);

    spi_init();
}

bool transport_master(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    gpio_write_pin_low(SPI_MASTER_PIN);

    chThdSleepMilliseconds(10);

    if(spi_start(SPI_SS_PIN, false, 0, 4096)) {
        println("spi started");

        spi_write(0x00);
        spi_write(0x08);
        spi_write(0x00);
        spi_write(0x00);
        spi_write(0xFF);
        spi_write(0xA0);

        chThdSleepMilliseconds(1);

        spi_stop();
    } else {
        println("spi start failed");
        chThdSleepMilliseconds(1);
    }

    gpio_write_pin_high(SPI_MASTER_PIN);

    println("transport_master");

    return true;
}
