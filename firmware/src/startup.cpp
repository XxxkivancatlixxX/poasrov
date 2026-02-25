// Minimal startup code for ARM Cortex-M4
extern int main(void);
extern void _stack(void);

void Reset_Handler(void) {
    extern char _etext, _sdata, _edata, _sbss, _ebss;
    
    // Copy .data section from FLASH to RAM
    char *src = &_etext;
    char *dst = &_sdata;
    
    // Bounds check to prevent crashes in emulation
    if (src != 0 && dst != 0) {
        while (dst < &_edata) {
            *dst++ = *src++;
        }
    }
    
    // Zero .bss section
    dst = &_sbss;
    if (dst != 0) {
        while (dst < &_ebss) {
            *dst++ = 0;
        }
    }
    
    main();
    
    while (1);
}

void Default_Handler(void) {
    while (1);
}

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t vectors[16] __attribute__((section(".vectors"))) = {
    (uint32_t)&_stack,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t)SVC_Handler,
    (uint32_t)DebugMon_Handler,
    0,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,
};
