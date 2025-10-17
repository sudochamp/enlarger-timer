/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * General stdiodemo defines
 *
 * $Id$
 */

/* CPU frequency */
#define F_CPU 16000000UL

/* UART baud rate */
#define BAUD  9600UL

#define MYUBRR F_CPU / 16 / BAUD - 1

/* Whether to read the busy flag, or fall back to
   worst-time delays. */
#define USE_BUSY_BIT 1
