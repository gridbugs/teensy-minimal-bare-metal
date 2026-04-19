#include <math.h>
#include "printf.h"
#include "imxrt1062.h"
#include "util.h"

void fpu_enable(void) {
    SCB_CPACR |= (0xF << 20);  // Set CP10 and CP11 to full access
    __asm__ volatile ("dsb");
    __asm__ volatile ("isb");
}

void lpuart1_putchar(char data) {
  while (!(LPUART1_STAT & LPUART_STAT_TDRE));
  LPUART1_DATA = data;
}

void _putchar(char data) {
  lpuart1_putchar(data);
}

int main(void) {
    fpu_enable();

    // Derive UART clock from osc_clk (24 MHz)
    CCM_CSCDR1 |= CCM_CSCDR1_UART_CLK_SEL;

    // Divide by 1 so UART clock is 24MHz
    CCM_CSCDR1 =
      (CCM_CSCDR1 & ~(CCM_CSCDR1_UART_CLK_PODF(MASK(6)))) |
      CCM_CSCDR1_UART_CLK_PODF(0);

    // Enable the clock for UART1
    CCM_CCGR5 |= CCM_CCGR5_LPUART1(3);

    // Select the LPUART1_TX mux mode for pin 24
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12 = 2;

    // Configure the LPUART1_TX pin
    IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_12 =
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

    printf("Hello, World! %f\n\r", M_PI);

    for(;;);
    return 0;
}
