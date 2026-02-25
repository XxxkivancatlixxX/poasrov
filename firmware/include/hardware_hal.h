#pragma once

#include <cstdint>

class PWMDriver {
public:
    PWMDriver();
    ~PWMDriver();
    
    bool init();
    void set_pwm(uint8_t channel, uint16_t pulse_us);
    void set_all_pwm(uint16_t pulse_us);
    
private:
    static const uint16_t PWM_FREQ_HZ = 400;
    static const uint16_t MIN_PULSE_US = 1000;
    static const uint16_t MAX_PULSE_US = 2000;
};

class UARTDriver {
public:
    UARTDriver();
    ~UARTDriver();
    
    bool init(uint32_t baudrate = 57600);
    void write_byte(uint8_t byte);
    void write_bytes(const uint8_t* data, uint16_t len);
    uint8_t read_byte();
    uint16_t read_available();
    
    void set_simulation_mode(bool sim) { simulation_mode = sim; }
    
private:
    uint8_t rx_buffer[512];
    uint16_t rx_head = 0;
    uint16_t rx_tail = 0;
    bool simulation_mode = true;  // Default to simulation for safety
};

extern PWMDriver g_pwm;
extern UARTDriver g_uart;
