#include "quantum.h"
#include "print.h"

#include "spi_master.h"

void keyboard_pre_init_kb() {
}

void matrix_init_kb(void) {
}

void matrix_post_init_kb(void) {
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

    chThdSleepMilliseconds(1);

    // --->

    // XX XX FF FF FF XX - 1 layer
    // XX XX 00 00 FF XX - 2 layer
    // XX XX 00 FF 00 XX - 3 layer
    // XX XX FF 00 FF XX - 4 layer

    // XX 08 XX XX XX XX - none
    // XX 09 XX XX XX XX - Caps Lock on
    // XX 0A XX XX XX XX - Scroll Lock on
    // XX 0C XX XX XX XX - Win Lock on


    // <---

    // 00 02 00 00 00 00 00 00 00 00 00 00 - 7
    // 00 08 00 00 00 00 00 00 00 00 00 00 - u
    // 00 20 00 00 00 00 00 00 00 00 00 00 - j
    // 00 80 00 00 00 00 00 00 00 00 00 00 - m

    // 00 00 02 00 00 00 00 00 00 00 00 00 - 8
    // 00 00 08 00 00 00 00 00 00 00 00 00 - i
    // 00 00 20 00 00 00 00 00 00 00 00 00 - k
    // 00 00 80 00 00 00 00 00 00 00 00 00 - ,

    // 00 00 00 02 00 00 00 00 00 00 00 00 - 9
    // 00 00 00 08 00 00 00 00 00 00 00 00 - o
    // 00 00 00 20 00 00 00 00 00 00 00 00 - l
    // 00 00 00 80 00 00 00 00 00 00 00 00 - .

    // 00 00 00 00 02 00 00 00 00 00 00 00 - 0
    // 00 00 00 00 08 00 00 00 00 00 00 00 - p
    // 00 00 00 00 20 00 00 00 00 00 00 00 - ;
    // 00 00 00 00 80 00 00 00 00 00 00 00 - /

    // 00 00 00 00 00 02 00 00 00 00 00 00 - -
    // 00 00 00 00 00 08 00 00 00 00 00 00 - [
    // 00 00 00 00 00 20 00 00 00 00 00 00 - '
    // 00 00 00 00 00 80 00 00 00 00 00 00 - Shift

    // 00 00 00 00 00 00 02 00 00 00 00 00 - =
    // 00 00 00 00 00 00 08 00 00 00 00 00 - ]
    // 00 00 00 00 00 00 20 00 00 00 00 00 - Enter

    // 00 00 00 00 00 00 00 02 00 00 00 00 - Backspace
    // 00 00 00 00 00 00 00 04 00 00 00 00 - y
    // 00 00 00 00 00 00 00 08 00 00 00 00 - \
    // 00 00 00 00 00 00 00 10 00 00 00 00 - h
    // 00 00 00 00 00 00 00 40 00 00 00 00 - n

    // 00 00 00 00 00 00 00 00 00 01 00 00 - m6
    // 00 00 00 00 00 00 00 00 00 02 00 00 - m7
    // 00 00 00 00 00 00 00 00 00 04 00 00 - m8
    // 00 00 00 00 00 00 00 00 00 10 00 00 - m9
    // 00 00 00 00 00 00 00 00 00 20 00 00 - Ctl
    // 00 00 00 00 00 00 00 00 00 40 00 00 - m10

    // 00 00 00 00 00 00 00 00 00 00 02 00 - Fn
    // 00 00 00 00 00 00 00 00 00 00 10 00 - Spacebar
    // 00 00 00 00 00 00 00 00 00 00 40 00 - Alt
    // 00 00 00 00 00 00 00 00 00 00 80 00 - Win

    if(spi_start(SPI_SS_PIN, false, 0, 254)) {
        uint8_t data[] = {
            0x00,
            0x08,
            0x00,
            0x00,
            0xFF,
            0xA0
        };
        spi_transmit(data, sizeof(data));

        spi_stop();
    } else {
        println("spi start failed");
    }

    gpio_write_pin_high(SPI_MASTER_PIN);

    return true;
}

