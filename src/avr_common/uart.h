#pragma once

// useful defines for code completion in IDE
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#define __AVR_3_BYTE_PC__
#define F_CPU 16000000UL
#endif

void printf_init(void);
