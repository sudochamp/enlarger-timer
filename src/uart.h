/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, UART declarations
 *
 * $Id$
 */

/*
 * Perform UART startup initialization.
 */
void uart_init(unsigned int);

/*
 * Send one character to the UART.
 */
int	uart_putchar(char c, FILE *stream);

/*
 * Size of internal line buffer used by uart_getchar().
 */
#define RX_BUFSIZE 80

int	uart_getchar(FILE *stream);
