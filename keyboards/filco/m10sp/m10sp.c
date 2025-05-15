#include <hal.h>

#include "quantum.h"
#include "print.h"


static bool spiTransportInitialized = false;
static bool spiTransportStarted = false;

static void spiTransportInit(void) {
    if(spiTransportInitialized) return;

    spiInit();

    spiTransportInitialized = true;

    spiStop(&SPI_DRIVER);
    spiTransportStarted = false;
}


static SPIConfig spiConfig;

static bool spiTransportStart(void) {
    if(spiTransportStarted) return false;

    spiConfig.clkdiv = 119; // 200 kHz
    spiConfig.ctrl0 = /*SPI_TXFIFO_LENGTH(7) | SPI_RXFIFO_LENGTH(7) |*/ SPI_DATA_LENGTH(8);
    spiConfig.ctrl1 = SPI_CPOL_LOW | SPI_CPHA_FALLING;

    spiStart(&SPI_DRIVER, &spiConfig);

    spiSelect(&SPI_DRIVER);

    spiTransportStarted = true;

    return spiTransportStarted;
}

static void spiTransportStop(void) {
    if(!spiTransportStarted) return;

    spiUnselect(&SPI_DRIVER);
    spiStop(&SPI_DRIVER);

    spiTransportStarted = false;
}

static void sendLedState(void) {
    if(spiTransportStart()) {
        // println("SPI Transport started");

        uint8_t txBuf[] = { 0xAA };
#if 0
        uint8_t rxBuf[sizeof(txBuf)];
        spiExchange(&SPI_DRIVER, sizeof(txBuf), &txBuf, &rxBuf);
#else
        spiSend(&SPI_DRIVER, sizeof(txBuf), &txBuf);
#endif

        spiTransportStop();
    } else {
        //println("SPI Transport start failed");
    }
}

void keyboard_pre_init_kb(void) {
    gpio_set_pin_output(SPI_MASTER_PIN);
    gpio_set_pin_output(SPI_SS_PIN);
    gpio_set_pin_output(SPI_SCK_PIN);
    gpio_set_pin_output(SPI_MOSI_PIN);
    gpio_set_pin_output(SPI_MISO_PIN);

    gpio_write_pin_high(SPI_MASTER_PIN);
    gpio_write_pin_high(SPI_SS_PIN);
    gpio_write_pin_low(SPI_SCK_PIN);
    gpio_write_pin_low(SPI_MOSI_PIN);
    gpio_write_pin_low(SPI_MISO_PIN);
}

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}

bool is_keyboard_master(void) { return true; }

void transport_slave_init(void) {}
void transport_slave(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {}

void transport_master_init(void) {
    spiTransportInit();
}

static int counter = 0;
bool transport_master(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    //chThdSleepMilliseconds(1);

    if(counter > 20) return false;
    ++counter;

    gpio_write_pin_low(SPI_MASTER_PIN);

    sendLedState();

    gpio_write_pin_high(SPI_MASTER_PIN);

    return false;
}
