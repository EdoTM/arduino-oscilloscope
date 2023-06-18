#include <stdio.h>
#include "avr_common/uart.h"
#include <avr/io.h>
#include <util/delay.h>

int main() {
    printf_init();
    while (1) {
        printf("Hello, world!\n");
        _delay_ms(1000);
    }
}
