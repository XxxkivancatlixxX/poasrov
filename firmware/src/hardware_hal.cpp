#include "hardware_hal.h"
#include <cstdio>

#define HWREG(x) (*((volatile uint32_t *)(x)))
#define HWREG16(x) (*((volatile uint16_t *)(x)))

#define RCC_BASE 0x40023800
#define RCC_AHB1ENR (RCC_BASE + 0x30)
#define RCC_APB2ENR (RCC_BASE + 0x44)

#define GPIOA_BASE 0x40020000
#define GPIOC_BASE 0x40020800
#define GPIO_MODER(base) (base + 0x00)
#define GPIO_AFRL(base) (base + 0x20)
#define GPIO_AFRH(base) (base + 0x24)

#define TIM1_BASE 0x40010000
#define TIM3_BASE 0x40000400
#define TIM_CR1(base) (base + 0x00)
#define TIM_CCMR1(base) (base + 0x18)
#define TIM_CCMR2(base) (base + 0x1C)
#define TIM_CCER(base) (base + 0x20)
#define TIM_PSC(base) (base + 0x28)
#define TIM_ARR(base) (base + 0x2C)
#define TIM_CCR1(base) (base + 0x34)
#define TIM_CCR2(base) (base + 0x38)
#define TIM_CCR3(base) (base + 0x3C)
#define TIM_CCR4(base) (base + 0x40)

#define USART1_BASE 0x40011000
#define USART_SR(base) (base + 0x00)
#define USART_DR(base) (base + 0x04)
#define USART_BRR(base) (base + 0x08)
#define USART_CR1(base) (base + 0x0C)
#define USART_CR3(base) (base + 0x14)

PWMDriver g_pwm;
UARTDriver g_uart;

PWMDriver::PWMDriver() {}
PWMDriver::~PWMDriver() {}

bool PWMDriver::init() {
    HWREG(RCC_AHB1ENR) |= (1 << 0);
    HWREG(RCC_AHB1ENR) |= (1 << 2);
    HWREG(RCC_APB2ENR) |= (1 << 0);
    
    uint32_t tim1_arr = (168000000 / (PWM_FREQ_HZ * 1680)) - 1;
    uint32_t tim1_psc = 1680 - 1;
    
    HWREG(TIM_PSC(TIM1_BASE)) = tim1_psc;
    HWREG(TIM_ARR(TIM1_BASE)) = tim1_arr;
    
    HWREG(TIM_CCMR1(TIM1_BASE)) = 0x6868;
    HWREG(TIM_CCMR2(TIM1_BASE)) = 0x6868;
    HWREG(TIM_CCER(TIM1_BASE)) = 0x1111;
    
    HWREG(TIM_CR1(TIM1_BASE)) |= 1;
    
    uint32_t tim3_arr = (84000000 / (PWM_FREQ_HZ * 840)) - 1;
    uint32_t tim3_psc = 840 - 1;
    
    HWREG(TIM_PSC(TIM3_BASE)) = tim3_psc;
    HWREG(TIM_ARR(TIM3_BASE)) = tim3_arr;
    
    HWREG(TIM_CCMR1(TIM3_BASE)) = 0x6868;
    HWREG(TIM_CCMR2(TIM3_BASE)) = 0x6868;
    HWREG(TIM_CCER(TIM3_BASE)) = 0x1111;
    
    HWREG(TIM_CR1(TIM3_BASE)) |= 1;
    
    HWREG(GPIO_MODER(GPIOA_BASE)) |= 0x0A8000;
    HWREG(GPIO_AFRH(GPIOA_BASE)) = 0x00001111;
    
    HWREG(GPIO_MODER(GPIOC_BASE)) |= 0x0A8000;
    HWREG(GPIO_AFRL(GPIOC_BASE)) = 0x11110000;
    
    set_all_pwm(1500);
    
    return true;
}

void PWMDriver::set_pwm(uint8_t channel, uint16_t pulse_us) {
    if (pulse_us < MIN_PULSE_US) pulse_us = MIN_PULSE_US;
    if (pulse_us > MAX_PULSE_US) pulse_us = MAX_PULSE_US;
    
    uint32_t ccr_value = (pulse_us * 100) / 125;
    
    if (channel == 0) HWREG(TIM_CCR1(TIM1_BASE)) = ccr_value;
    else if (channel == 1) HWREG(TIM_CCR2(TIM1_BASE)) = ccr_value;
    else if (channel == 2) HWREG(TIM_CCR3(TIM1_BASE)) = ccr_value;
    else if (channel == 3) HWREG(TIM_CCR4(TIM1_BASE)) = ccr_value;
    else if (channel == 4) HWREG(TIM_CCR1(TIM3_BASE)) = ccr_value;
    else if (channel == 5) HWREG(TIM_CCR2(TIM3_BASE)) = ccr_value;
    else if (channel == 6) HWREG(TIM_CCR3(TIM3_BASE)) = ccr_value;
    else if (channel == 7) HWREG(TIM_CCR4(TIM3_BASE)) = ccr_value;
}

void PWMDriver::set_all_pwm(uint16_t pulse_us) {
    for (uint8_t i = 0; i < 8; i++) {
        set_pwm(i, pulse_us);
    }
}

UARTDriver::UARTDriver() {}
UARTDriver::~UARTDriver() {}

bool UARTDriver::init(uint32_t baudrate) {
    HWREG(RCC_AHB1ENR) |= (1 << 0);
    HWREG(RCC_APB2ENR) |= (1 << 4);
    
    uint32_t brr = 168000000 / (16 * baudrate);
    HWREG(USART_BRR(USART1_BASE)) = brr;
    HWREG(USART_CR1(USART1_BASE)) = 0x200C;
    HWREG(USART_CR3(USART1_BASE)) = 0x0000;
    
    HWREG(GPIO_MODER(GPIOA_BASE)) |= 0x00A00000;
    HWREG(GPIO_AFRH(GPIOA_BASE)) |= 0x00000770;
    
    return true;
}

void UARTDriver::write_byte(uint8_t byte) {
    while (!(HWREG(USART_SR(USART1_BASE)) & (1 << 7)));
    HWREG(USART_DR(USART1_BASE)) = byte;
}

void UARTDriver::write_bytes(const uint8_t* data, uint16_t len) {
    if (simulation_mode) {
        // In simulation, output to stdout (can be piped to telemetry bridge)
        fwrite(data, 1, len, stdout);
        fflush(stdout);
    } else {
        // Real hardware UART
        for (uint16_t i = 0; i < len; i++) {
            write_byte(data[i]);
        }
    }
}

uint8_t UARTDriver::read_byte() {
    while (!(HWREG(USART_SR(USART1_BASE)) & (1 << 5)));
    return (uint8_t)HWREG(USART_DR(USART1_BASE));
}

uint16_t UARTDriver::read_available() {
    return (HWREG(USART_SR(USART1_BASE)) & (1 << 5)) ? 1 : 0;
}
