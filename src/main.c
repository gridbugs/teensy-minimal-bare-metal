#include <math.h>
#include "printf.h"
#include "imxrt1062.h"
#include "util.h"

void lpuart1_putchar(char data) {
  while (!(LPUART1_STAT & LPUART_STAT_TDRE));
  LPUART1_DATA = data;
}

void lpuart2_putchar(char data) {
  while (!(LPUART2_STAT & LPUART_STAT_TDRE));
  LPUART2_DATA = data;
}

void _putchar(char data) {
  lpuart1_putchar(data);
}

const char* dma_test_string = "Hello DMA!\n\r";

int main(void) {

    // Derive UART clock from osc_clk (24 MHz)
    CCM_CSCDR1 |= CCM_CSCDR1_UART_CLK_SEL;

    // Divide by 1 so UART clock is 24MHz
    CCM_CSCDR1 =
      (CCM_CSCDR1 & ~(CCM_CSCDR1_UART_CLK_PODF(MASK(6)))) |
      CCM_CSCDR1_UART_CLK_PODF(0);

    // Enable the clock for UART1 and UART2
    CCM_CCGR5 |= CCM_CCGR5_LPUART1(3);
    CCM_CCGR0 |= CCM_CCGR0_LPUART2(3);

    // Select the LPUART1_TX mux mode for pin 24
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12 = 2;

    // Select the LPUART2_TX mux mode for pin 14
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_02 = 2;

    // Configure the LPUART1_TX pin
    IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_12 =
      IOMUXC_PAD_PUS(3) | // 22k pull-up
      IOMUXC_PAD_SPEED(0) | // Low speed
      IOMUXC_PAD_DSE(2); // Drive strength: R0/2

    // Configure the LPUART2_TX pin
    IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_02 =
      IOMUXC_PAD_PUS(3) | // 22k pull-up
      IOMUXC_PAD_SPEED(0) | // Low speed
      IOMUXC_PAD_DSE(2); // Drive strength: R0/2

    // Disable transmitter and receiver before changing baud rate
    LPUART1_CTRL &= ~(LPUART_CTRL_RE | LPUART_CTRL_TE);
    // Divide 24 MHz by 2500 for a baud rate of 9600
    LPUART1_BAUD = LPUART_BAUD_OSR(9) | LPUART_BAUD_SBR(250);
    // Enable transmit FIFO
    LPUART1_FIFO |= LPUART_FIFO_TXFE;
    // Enable transmitter and receiver
    LPUART1_CTRL |= (LPUART_CTRL_RE | LPUART_CTRL_TE);

    // Disable transmitter and receiver before changing baud rate
    LPUART2_CTRL &= ~(LPUART_CTRL_RE | LPUART_CTRL_TE);
    // Divide 24 MHz by 2500 for a baud rate of 9600
    LPUART2_BAUD = LPUART_BAUD_OSR(9) | LPUART_BAUD_SBR(250);
    // Enable transmit FIFO
    LPUART2_FIFO |= LPUART_FIFO_TXFE;
    // Enable transmitter and receiver
    LPUART2_CTRL |= (LPUART_CTRL_RE | LPUART_CTRL_TE);

    printf("Hello, World! %f\n\r", M_PI);

    // Enable the clock gate for the DMA hardware
    CCM_CCGR5 |= CCM_CCGR5_DMA(3);

    // Disable DMA channel 0 in the eDMA
    DMA_CERQ = 0;

    // Disable DMA channel 0 in the DMAMUX
    DMAMUX_CHCFG0 = 0;

    // Enable DMA channel 0 in the eDMA
    DMA_SERQ = 0;

    // Enable DMA channel 0 in the DMAMUX and associate it with LPUART1_TX
    DMAMUX_CHCFG0 = DMAMUX_CHCFG_ENBL | DMAMUX_SOURCE_LPUART1_TX;

    // Allow LPUART1 to trigger DMA transfers
    LPUART1_BAUD |= LPUART_BAUD_TDMAE;

    // Example DMA transfer to LPUART1
    DMA_TCD0_SADDR = dma_test_string;
    DMA_TCD0_DADDR = &LPUART1_DATA;
    DMA_TCD0_NBYTES_MLNO = 1;
    DMA_TCD0_CITER_ELINKNO = 14;
    DMA_TCD0_BITER_ELINKNO = 14;
    DMA_TCD0_SOFF = 1;
    DMA_TCD0_DOFF = 0;
    DMA_TCD0_ATTR = DMA_TCD_ATTR_SSIZE(0) | DMA_TCD_ATTR_DSIZE(0);
    DMA_TCD0_CSR = DMA_TCD_CSR_INTMAJOR;

    for(;;);
    return 0;
}
