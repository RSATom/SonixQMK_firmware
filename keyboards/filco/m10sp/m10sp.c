#include <hal.h>
#include <quantum.h>
#include <spi_master.h>

/* halfs data exchange SPI protocol

LEDS --> Right Half (SEND_LEDS_MODE_PIN = LOW)
00 00 00 00 00 A0 - template

XX XX FF FF FF XX - 1 layer
XX XX 00 00 FF XX - 2 layer
XX XX 00 FF 00 XX - 3 layer
XX XX FF 00 FF XX - 4 layer

XX 08 XX XX XX XX - none
XX 09 XX XX XX XX - Caps Lock on
XX 0A XX XX XX XX - Scroll Lock on
XX 0C XX XX XX XX - Win Lock on

Left Half <-- Keys (SEND_LEDS_MODE_PIN = HIGH)
00 02 00 00 00 00 00 00 00 00 00 00 - 7
00 08 00 00 00 00 00 00 00 00 00 00 - u
00 20 00 00 00 00 00 00 00 00 00 00 - j
00 80 00 00 00 00 00 00 00 00 00 00 - m

00 00 02 00 00 00 00 00 00 00 00 00 - 8
00 00 08 00 00 00 00 00 00 00 00 00 - i
00 00 20 00 00 00 00 00 00 00 00 00 - k
00 00 80 00 00 00 00 00 00 00 00 00 - ,

00 00 00 02 00 00 00 00 00 00 00 00 - 9
00 00 00 08 00 00 00 00 00 00 00 00 - o
00 00 00 20 00 00 00 00 00 00 00 00 - l
00 00 00 80 00 00 00 00 00 00 00 00 - .

00 00 00 00 02 00 00 00 00 00 00 00 - 0
00 00 00 00 08 00 00 00 00 00 00 00 - p
00 00 00 00 20 00 00 00 00 00 00 00 - ;
00 00 00 00 80 00 00 00 00 00 00 00 - /

00 00 00 00 00 02 00 00 00 00 00 00 - -
00 00 00 00 00 08 00 00 00 00 00 00 - [
00 00 00 00 00 20 00 00 00 00 00 00 - '
00 00 00 00 00 80 00 00 00 00 00 00 - Shift

00 00 00 00 00 00 02 00 00 00 00 00 - =
00 00 00 00 00 00 08 00 00 00 00 00 - ]
00 00 00 00 00 00 20 00 00 00 00 00 - Enter

00 00 00 00 00 00 00 02 00 00 00 00 - Backspace
00 00 00 00 00 00 00 04 00 00 00 00 - y
00 00 00 00 00 00 00 08 00 00 00 00 - \
00 00 00 00 00 00 00 10 00 00 00 00 - h
00 00 00 00 00 00 00 40 00 00 00 00 - n

00 00 00 00 00 00 00 00 00 01 00 00 - m6
00 00 00 00 00 00 00 00 00 02 00 00 - m7
00 00 00 00 00 00 00 00 00 04 00 00 - m8
00 00 00 00 00 00 00 00 00 10 00 00 - m9
00 00 00 00 00 00 00 00 00 20 00 00 - Ctl
00 00 00 00 00 00 00 00 00 40 00 00 - m10

00 00 00 00 00 00 00 00 00 00 02 00 - Fn
00 00 00 00 00 00 00 00 00 00 10 00 - Spacebar
00 00 00 00 00 00 00 00 00 00 40 00 - Alt
00 00 00 00 00 00 00 00 00 00 80 00 - Win
*/

#define WAIT_BOOT_KEY

#define SLAVE_MATRIX_ROWS 8
#define SLAVE_MATRIX_RAW_COLS 12
#define SLAVE_MATRIX_COLS_OFFSET 1
#define SLAVE_MATRIX_COLS 10

#define LOCK_LEDS_OFFSET 1
#define NO_LOCK_LEDS 0x08
#define CAPS_LOCK_LED_ON 0x09
#define SCROLL_LOCK_LED_ON 0x0A
#define WIN_LOCK_LED_ON 0x0C

#define LAYER_LED_RED_OFFSET 2
#define LAYER_LED_GREEN_OFFSET 3
#define LAYER_LED_BLUE_OFFSET 4
#define LAYER_LED_COLOR_HIGH_LEVEL 0xFF
#define LAYER_LED_COLOR_MEDIUM_LEVEL 0x80
#define LAYER_LED_COLOR_LOW_LEVEL 0x01

#define LEDS_TEMPLATE { 0x00, NO_LOCK_LEDS, 0x00, 0x00, 0x00, 0xA0 }

#define LEDS_SEND_INTERVAL 100 // milliseconds


static void spiTransportInit(void) {
    spi_init();
}

static bool spiTransportStart(void) {
    return spi_start(
        SPI_SS_PIN,
        false,
        0,
        SPI_DIV);
}

static void spiTransportStop(void) {
    spi_stop();
}

static systime_t lastLedsSendTime = 0;
static bool rightHalfReady = false;
static bool sleeping = false;
static bool sendLedsState(led_t led_state) {
    bool success = false;

    gpio_write_pin_low(SEND_LEDS_MODE_PIN);

    if(spiTransportStart()) {
        uint8_t leds[] = LEDS_TEMPLATE;
        uint8_t dipSwitches[sizeof(leds) / sizeof(leds[0])] = {};

        if(!sleeping) {
            leds[LAYER_LED_GREEN_OFFSET] |= LAYER_LED_COLOR_LOW_LEVEL;

            if(led_state.caps_lock) {
                leds[LOCK_LEDS_OFFSET] |= CAPS_LOCK_LED_ON;
            }

            if(led_state.scroll_lock) {
                leds[LOCK_LEDS_OFFSET] |= SCROLL_LOCK_LED_ON;
            }
        }

        spiExchange(
            &SPI_DRIVER,
            sizeof(leds) / sizeof(leds[0]),
            leds,
            dipSwitches);
        rightHalfReady =
            dipSwitches[0] == 0x0 && dipSwitches[1] == 0x55 &&
            dipSwitches[4] == 0x0 && dipSwitches[5] == 0x0;

        lastLedsSendTime = chVTGetSystemTimeX();

        success = true;
    }

    spiTransportStop();

    gpio_write_pin_high(SEND_LEDS_MODE_PIN);

    return success;
}

static bool receiveKeysState(matrix_row_t slave_matrix[]) {
    bool success = false;

    uint8_t buttonsBuf[SLAVE_MATRIX_RAW_COLS] = {};

    if(spiTransportStart()) {
        spiReceive(&SPI_DRIVER, SLAVE_MATRIX_RAW_COLS, buttonsBuf);
        success = true;
    }

    spiTransportStop();

    if(success && !buttonsBuf[0] && !buttonsBuf[SLAVE_MATRIX_RAW_COLS - 1]) {
        for(uint_fast8_t col = 0; col < SLAVE_MATRIX_COLS; ++col) {
            uint8_t colPressedKeys = buttonsBuf[SLAVE_MATRIX_COLS_OFFSET + col];
            for(uint_fast8_t row = 0; row < SLAVE_MATRIX_ROWS; ++row) {
                if(colPressedKeys & (1 << row)) {
                    slave_matrix[row] |= 1 << col;
                }
            }
        }
    }

    return success;
}

void keyboard_pre_init_kb(void) {
    gpio_set_pin_output(SEND_LEDS_MODE_PIN);
    gpio_set_pin_output(SPI_SS_PIN);
    gpio_set_pin_output(SPI_SCK_PIN);
    gpio_set_pin_output(SPI_MOSI_PIN);
    gpio_set_pin_output(SPI_MISO_PIN);

    gpio_write_pin_high(SEND_LEDS_MODE_PIN);
    gpio_write_pin_high(SPI_SS_PIN);
    gpio_write_pin_low(SPI_SCK_PIN);
    gpio_write_pin_low(SPI_MOSI_PIN);
    gpio_write_pin_low(SPI_MISO_PIN);
}

void suspend_power_down_kb(void) {
    if(sleeping)
        return;

    sleeping = true;

    sendLedsState(host_keyboard_led_state());
}

void suspend_wakeup_init_kb(void) {
    if(!sleeping)
        return;

    sleeping = false;

    sendLedsState(host_keyboard_led_state());
}

bool is_keyboard_master(void) { return true; }

void transport_slave_init(void) {}
void transport_slave(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {}

void transport_master_init(void) {
    spiTransportInit();
}

bool led_update_kb(led_t led_state) {
    return sendLedsState(led_state);
}

bool transport_master(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
#ifdef WAIT_BOOT_KEY
    static unsigned counter = 0;
    if(counter < 3) { //give chance to switch to bootloader
        ++counter;

        chThdSleepMilliseconds(50);

        return false;
    }
#endif

    if(TIME_I2MS(chVTTimeElapsedSinceX(lastLedsSendTime)) > LEDS_SEND_INTERVAL) {
        sendLedsState(host_keyboard_led_state());
    }

    if(rightHalfReady) {
        return receiveKeysState(slave_matrix);
    }

    return false;
}
