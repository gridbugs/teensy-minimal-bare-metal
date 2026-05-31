# Minimal Teensy 4.1 bare metal starting point

Build and flash with `make flash`.

Prints at 9600 baud via the lpuart1 device whose TX line is attached to pin 24
of the Teensy 4.1 (called TX6 in the Teensy literature).

# Notes

## Raw Pinout
To find out which pin in the teensy documentation corresponds to which pin in
the i.MX RT1060 documentation, [this  file](https://github.com/PaulStoffregen/cores/blob/master/teensy4/core_pins.h)
from the teensy arduino-style sdk contains useful information.

## LPUART

To enable a LPUART device:
  1. Select the clock source and clock divider for LPUART. The only
     needs to happen once for all LPUART devices. This is configured in the
     `CCM_CSCDR1` register.
  1. Enable the clock gate for each LPUART device that will be used. This is
     configured in the `CCM_CCGRx` registers.
  1. Configure the IO multiplexer so whichever pins have the potential to be
     the LPUART devices' TX/RX pins are assigend that function. This is
     configured in the `IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_xx_xx` registers.
  1. Configure the pin behaviour in the IO multiplexer appropriate for what
     will be attached to the LPUART's pins. For the TX pin printing via a
     USB-serial adapter, an appropriate config is
     `IOMUXC_PAD_PUS(3)|IOMUXC_PAD_SPEED(0)|IOMUXC_PAD_DSE(2)`.
  1. Configure the LPUART device itself via its control registers.

## DMA

To set up DMA transfers for a device:
  1. Associate the device's DMA source with a channel in `DMAMUX` and enable
     the channel. If the device will be triggering the DMA transfers itself
     then don't set the `DMAMUX_CHCFG_TRIG` bit.

# Credits

This is based on [Teensy 4.0 Bare-Metal
Example](https://github.com/blazer82/baremetal-blinky.teensy.git) which in turn
borrows some code from the [Teensy Core Libraries for
Arduino](https://github.com/PaulStoffregen/cores) by Paul Stoffregen.
