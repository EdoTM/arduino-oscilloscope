#include "uart.h"
#include <avr/io.h>
#include <stdio.h>

#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD - 1)

void UART_init(void) {
    // Set baud rate
    UBRR0H = (uint8_t) (MYUBRR >> 8);
    UBRR0L = (uint8_t) MYUBRR;

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */
}

void UART_putChar(uint8_t c) {
    // wait for transmission completed, looping on status bit
    while (!(UCSR0A & _BV(UDRE0)));

    // Start transmission
    UDR0 = c;
}

uint8_t UART_getChar(void) {
    // Wait for incoming data, looping on status bit
    while (!(UCSR0A & _BV(RXC0)));

    // Return the data
    return UDR0;
}

int UART_putChar_printf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
    if (var == '\n') UART_putChar('\r');
    UART_putChar(var);
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(UART_putChar_printf, NULL, _FDEV_SETUP_WRITE);

/**
 * Reads a string until the first newline or 0
 * @param buf - The buffer to write
 * @return The number of bytes read
 */
uint8_t UART_getString(uint8_t *buf) {
    uint8_t *b0 = buf; //beginning of buffer
    while (1) {
        uint8_t c = UART_getChar();
        *buf = c;
        ++buf;
        // reading a 0 terminates the string
        if (c == 0)
            return buf - b0;
        // reading a \n  or a \r return results
        // in forcedly terminating the string
        if (c == '\n' || c == '\r') {
            *buf = 0;
            ++buf;
            return buf - b0;
        }
    }
}

void printf_init(void) {
    stdout = &mystdout;

    // fire up the usart
    UART_init();
}