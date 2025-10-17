/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, UART implementation
 *
 * $Id$
 */

#include "defines.h"

#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>

#include "uart.h"

/*
 * Initialize the UART to 9600 Bd, tx/rx, 8N1.
 */
void uart_init(unsigned int ubrr)
{
	// Set  Baud Rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	// Enable rx and tx
	UCSR0B |= _BV(RXEN0) | _BV(TXEN0);

	// Set frame format: 8data, 2stop bit
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart_putchar(char c, FILE *stream)
{

	if (c == '\a')
	{
		fputs("*ring*\n", stderr);
		return 0;
	}

	if (c == '\n')
		uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;

	return 0;
}

int uart_getchar(FILE *stream) {
	return 0;
}