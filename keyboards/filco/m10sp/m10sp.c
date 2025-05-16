#include <hal.h>

#include "quantum.h"
#include "print.h"


#define USE_SPI_DRIVER 1

#define SPI_TEST_CNT 32

#define mskSPI_BUSY (0x1<<4) //Busy flag
#define mskSPI_RX_EMPTY (0x1<<2) //RX FIFO empty flag
#define mskSPI_TXFIFOTHF (0x1<<5) //TX FIFO threshold flag
#define mskSPI_RX_EMPTY (0x1<<2) //RX FIFO empty flag
#define	SPI_FRESET_RESET_FIFO 3 //Reset finite state machine and FIFO
#define __SPI0_FIFO_RESET (SN_SPI0->CTRL0_b.FRESET = SPI_FRESET_RESET_FIFO)

static void SPI_NBytesTxRx(SPIDriver* spip, uint8_t* txBuf, uint8_t* rxBuf, uint32_t N_Bytes)
{
	uint32_t  wSPI_Send_Pointer = 0;
	uint32_t  wSPI_Get_Pointer = 0;

	while(wSPI_Send_Pointer != N_Bytes)
	{
		SN_SPI0->DATA = txBuf[wSPI_Send_Pointer++];

		while (!(SN_SPI0->STAT & mskSPI_TXFIFOTHF)); //TX Half-Empty

		if(!(SN_SPI0->STAT & mskSPI_RX_EMPTY)) //Check having any data in RXFIFO
		{
			rxBuf[wSPI_Get_Pointer++] = SN_SPI0->DATA;
		}
	}

	__SPI0_FIFO_RESET;
}

static uint16_t hwSPI_Tx_Fifo[SPI_TEST_CNT] = {
    0xAA, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
};
static uint16_t hwSPI_Rx_Fifo[SPI_TEST_CNT] = { 0 };

static void SPI0_NBytesTxRx(uint32_t size)
{
	uint32_t offset = 0;

	while(offset < size) {
		SN_SPI0->DATA = hwSPI_Tx_Fifo[offset];

		while (!(SN_SPI0->STAT & mskSPI_TXFIFOTHF)) { };

        ++offset;
	}

	while(SN_SPI0->STAT & mskSPI_BUSY) {};

	__SPI0_FIFO_RESET;
}

#if USE_SPI_DRIVER
static bool spiTransportInitialized = false;
static bool spiTransportStarted = false;
static SPIConfig spiConfig;

static void spiTransportInit(void) {
    if(spiTransportInitialized) return;

    spiInit();

    spiTransportInitialized = true;

    spiStop(&SPI_DRIVER);
    spiTransportStarted = false;
}

static bool spiTransportStart(void) {
    if(spiTransportStarted) return false;

    spiConfig.clkdiv = 119; // 200 kHz
    spiConfig.ctrl0 = /*SPI_TXFIFO_LENGTH(7) | SPI_RXFIFO_LENGTH(7) |*/ SPI_DATA_LENGTH(8);
    spiConfig.ctrl1 = SPI_MLSB_MSB | SPI_CPOL_LOW | SPI_CPHA_FALLING;

    spiConfig.ssport = PAL_PORT(SPI_SS_PIN);
    spiConfig.sspad  = PAL_PAD(SPI_SS_PIN);
    gpio_set_pin_output(SPI_SS_PIN);

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
        println("SPI Transport started");

        uint8_t txBuf[] = { 0xAA };
        uint8_t rxBuf[sizeof(txBuf)] = { 0 };

        //spiSend(&SPI_DRIVER, sizeof(txBuf), &txBuf);
        //SPI_NBytesTxRx(&SPI_DRIVER, txBuf, rxBuf, sizeof(txBuf));
        SPI0_NBytesTxRx(2);

        spiTransportStop();
    }
}

#else

#define	SPI_SDODIS_EN 0 //Enable slave data output

#define	SPI_LOOPBACK_DIS 0 //Disable
#define	SPI_LOOPBACK_EN 1 //Data input from data output

/* SPI n Control register 0 <SPIn_CTRL0> (0x00) */
#define	SPI_SPIEN_DIS 0 //[0:0] SPI enable bit
#define	SPI_SPIEN_EN 1
#define mskSPI_SPIEN_DIS (SPI_SPIEN_DIS<<0)
#define	mskSPI_SPIEN_EN (SPI_SPIEN_EN<<0)

                        //[18:18]Auto-SEL disable bit. For SPI mode only.
#define	SPI_SELDIS_EN 0 //Enable Auto-SEL flow control
#define	SPI_SELDIS_DIS 1 //Disable Auto-SEL flow control
#define	mskSPI_SELDIS_EN (SPI_SELDIS_EN<<18)
#define	mskSPI_SELDIS_DIS (SPI_SELDIS_DIS<<18)

/* SPI n Control register 1 <SPIn_CTRL1> (0x04) */
                       //[0:0]MSB/LSB selection bit
//#define	SPI_MLSB_MSB 0 //MSB transmit first
//#define	SPI_MLSB_LSB 1 //LSB transmit first
#define	mskSPI_MLSB_MSB (SPI_MLSB_MSB<<0)
#define	mskSPI_MLSB_LSB (SPI_MLSB_LSB<<0)

                                //[1:1]Clock polarity selection bit
#define	SPI_CPOL_SCK_IDLE_LOW 0 //SCK idles at Low level
#define	SPI_CPOL_SCK_IDLE_HIGH 1 //SCK idles at High level
#define	mskSPI_CPOL_SCK_IDLE_LOW (SPI_CPOL_SCK_IDLE_LOW<<1)
#define	mskSPI_CPOL_SCK_IDLE_HIGH (SPI_CPOL_SCK_IDLE_HIGH<<1)

                                //[2:2]Clock phase for edge sampling
#define	SPI_CPHA_FALLING_EDGE 0 //Data changes at clock falling edge
#define	SPI_CPHA_RISING_EDGE 1 //Data changes at clock rising edge
#define	mskSPI_CPHA_FALLING_EDGE (SPI_CPHA_FALLING_EDGE<<2)
#define	mskSPI_CPHA_RISING_EDGE (SPI_CPHA_RISING_EDGE<<2)

#define SPI_DL_8 7
#define	SPI_DIV 240 //MCLK/n,MCLK=system clk  n = 2, 4, 6, 8, ...,512

#define SPI_MS_MASTER_MODE 0 //[3:3] Master/Slave selection bit

#define SPI_FRESET_RESET_FIFO 3 //Reset finite state machine and FIFO

#define __SPI0_FIFO_RESET (SN_SPI0->CTRL0_b.FRESET = SPI_FRESET_RESET_FIFO)
//#define __SPI0_CLR_SEL0 SN_GPIO0->MODE_b.MODE5=1; SN_GPIO0->DATA_b.DATA5=0;
//#define __SPI0_SET_SEL0 SN_GPIO0->MODE_b.MODE5=1; SN_GPIO0->DATA_b.DATA5=1;

#define mskSPI_BUSY (0x1<<4) //Busy flag
#define mskSPI_RX_EMPTY (0x1<<2) //RX FIFO empty flag
#define mskSPI_TXFIFOTHF (0x1<<5) //TX FIFO threshold flag

#define SPI_TEST_CNT 32

static uint16_t hwSPI_Tx_Fifo[SPI_TEST_CNT] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
};
static uint16_t hwSPI_Rx_Fifo[SPI_TEST_CNT] = { 0 };

static void SPI0_Init(void)
{
    //Enable HCLK for SPI0
    SN_SYS1->AHBCLKEN_b.SPI0CLKEN = 1; //Enable clock for SPI0.

    //SPI0 setting
    SN_SPI0->CTRL0_b.DL = SPI_DL_8; //3 ~ 16 Data length
    SN_SPI0->CTRL0_b.MS = SPI_MS_MASTER_MODE; //Master/Slave selection bit
    SN_SPI0->CTRL0_b.LOOPBACK = SPI_LOOPBACK_DIS; 	//Loop back mode
    SN_SPI0->CTRL0_b.SDODIS = SPI_SDODIS_EN; //Slave data output
                                             //(ONLY used in slave mode)

    SN_SPI0->CLKDIV_b.DIV = (SPI_DIV / 2) - 1; //SPIn clock divider

    //SPI0 SPI mode
    SN_SPI0->CTRL1 = mskSPI_CPHA_FALLING_EDGE | //Clock phase for edge sampling
                     mskSPI_CPOL_SCK_IDLE_LOW | //Clock polarity selection bit
                     mskSPI_MLSB_MSB; //MSB/LSB selection bit

    //SPI0 SEL0 setting
    SN_SPI0->CTRL0_b.SELDIS = SPI_SELDIS_DIS; //Auto-SEL bit
    //SN_GPIO2->MODE_b.MODE9 = 1; //SEL(P2.9) is outout high
    //__SPI0_SET_SEL0;

    //SPI0 Fifo reset
    __SPI0_FIFO_RESET;

    //SPI0 interrupt enable
    //SPI0_InterruptEnable();
    NVIC_ClearPendingIRQ(SPI0_IRQn);
    NVIC_EnableIRQ(SPI0_IRQn);
    //NVIC_SetPriority(SPI0_IRQn,0);

    //__SPI0_DATA_FETCH_HIGH_SPEED; //Enable if Freq. of SCK > 6MHz

    //SPI0 enable
    SN_SPI0->CTRL0_b.SPIEN  = SPI_SPIEN_EN; //SPI enable bit
}

static void SPI0_NBytesTxRx(uint32_t N_Bytes)
{
	uint32_t  wSPI_Send_Pointer = 0;
	uint32_t  wSPI_Get_Pointer = 0;

	while(wSPI_Send_Pointer != N_Bytes)
	{
		SN_SPI0->DATA = hwSPI_Tx_Fifo[wSPI_Send_Pointer++];

		while (!(SN_SPI0->STAT & mskSPI_TXFIFOTHF)); //TX Half-Empty

		if(!(SN_SPI0->STAT & mskSPI_RX_EMPTY)) //Check having any data in RXFIFO
		{
			hwSPI_Rx_Fifo[wSPI_Get_Pointer++] = SN_SPI0->DATA;
		}
	}

	__SPI0_FIFO_RESET;
}
#endif

void keyboard_pre_init_kb(void) {
//    SN_GPIO0->CFG = 0x00A0AAAA;
//    SN_GPIO1->CFG = 0x0AAAAA80;
//    SN_GPIO2->CFG = 0x80AAAAAA;

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
#if USE_SPI_DRIVER
    spiTransportInit();
#else
    SPI0_Init();
    SPI0_NBytesTxRx(SPI_TEST_CNT);
#endif
}

bool transport_master(matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    gpio_write_pin_low(SPI_MASTER_PIN);

#if USE_SPI_DRIVER
    sendLedState();
#else
    SPI0_Init();
    SPI0_NBytesTxRx(SPI_TEST_CNT);
#endif

    gpio_write_pin_high(SPI_MASTER_PIN);

    return false;
}
