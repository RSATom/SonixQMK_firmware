#include "quantum.h"
#include "print.h"

#include "spi_master.h"

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    //debug_enable=true;
    //debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;
}

bool is_keyboard_master(void) { return true; }

void transport_slave_init(void) {}
void transport_slave(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {}

void transport_master_init(void) {
    gpio_set_pin_output(SPI_MASTER_PIN);
    gpio_write_pin_high(SPI_MASTER_PIN);

    spi_init();
}

bool transport_master(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    gpio_write_pin_low(SPI_MASTER_PIN);

    println("transport_master");

    if(spi_start(SPI_SS_PIN, false, 0, 119)) {
        println("spi started");

        spi_write(0xA0);
        spi_stop();
    } else {
        println("spi start failed");
    }

    gpio_write_pin_high(SPI_MASTER_PIN);

    return true;
}
