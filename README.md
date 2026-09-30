# STM32F072 bare-metal tutorials

Register-level drivers and examples for the **STM32F072RB** (NUCLEO-F072RB board), written while following **Laurent Latorre's STM32 tutorials** on [pomad.fr](https://pomad.fr/tutorials/stm32_2026). No HAL, no LL: just CMSIS headers and the reference manual.

All credit for the course, the approach and the original examples goes to Laurent Latorre. This repository is my own implementation of the exercises, kept as a working code base to start future projects from.

## What's inside

A single STM32CubeIDE project that grows tutorial after tutorial. Each commit is one step, and each tutorial ends on a tag, so any stage can be checked out on its own.

```
app/     application code (main.c, interrupt handlers, printf)
bsp/     board support: GPIO, USART, ADC, DAC, timers, I2C, SPI, DMA, RTC...
cmsis/   ARM CMSIS core + ST device headers, startup and system files (STM32F072xB)
STM32F072RBTx_FLASH.ld   linker script
```

| Tag | Tutorial | Steps |
|---|---|---|
| `2.1-gpio` | GPIOs | LED driver, push-button driver |
| `2.2-uart-printf` | UART & printf() | console driver, printf() |
| `2.3-adc` | Analog input (ADC) | ADC driver |
| `2.4-timers` | Timers | basic delay, timer delay, input capture, output compare PWM |
| `2.5-dac` | Analog output (DAC) | DAC driver |
| `2.6-i2c` | I2C | read, write, final driver |
| `2.7-spi` | SPI | read, write, sensor driver |
| `3.1-external-interrupts` | External interrupts | event polling, EXTI #1, EXTI #2 |
| `3.2-timer-interrupts` | Timer interrupts | periodic interrupt |
| `3.3-uart-rx-interrupts` | UART RX interrupts | polling, interrupt, interrupt + buffer |
| `3.4-uart-rx-dma` | UART RX DMA | DMA, DMA polling, DMA interrupt |
| `3.5-adc-dma` | Analog input DMA | multi-channel ADC with DMA |
| `3.6-dac-dma` | Analog output DMA | sine wave, sine wave with DMA |
| `4.1-rtc` | RTC introduction | basics, dealing with reset, backup registers |
| `4.2-rtc-alarm` | RTC alarm | alarm interrupt |
| `4.3-watchdog` | Watchdog timer | hard fault, watchdog recovery |
| `5.1-digital-filtering` | Digital filtering | follower, floating point, fixed point |
| `5.2-low-power` | Introduction to low-power | steps #0 to #4 |

The I2C and SPI tutorials need external sensors wired to the board.

## Getting started

```bash
git clone https://github.com/<you>/stm32f072-baremetal-tutorials.git
```

Then in STM32CubeIDE: **File → Import → General → Existing Projects into Workspace**, select the cloned folder, and build. All paths in the project are relative, so the folder can live anywhere.

To go back to a given tutorial:

```bash
git checkout 3.2-timer-interrupts
```

To start a new application, copy the folder, rename the project in `.project`, and import it (tutorial 1.6 on pomad.fr describes this).

## Notes

- Target: STM32F072RBTx, Cortex-M0, 48 MHz from the HSE bypass (ST-LINK MCO), GNU Tools for STM32.
- The history was cleaned up when moving from the school GitLab: the startup file is the official `startup_stm32f072xb.s` from ST's CMSIS (v2.3.6) in every commit (the original mistakenly used the STM32F070xB one, so `TIM6_DAC_IRQHandler` and `DMA1_Channel4_5_6_7_IRQHandler` were never reached), and the linker script path is now relative.
- CMSIS files are © ARM and STMicroelectronics under their own licenses (see file headers).
