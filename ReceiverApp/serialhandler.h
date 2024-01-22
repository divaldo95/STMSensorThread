#ifndef SerialHandler_h
#define SerialHandler_h

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */
int serialOpen (const char *device, const int baud);

/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serialFlush (const int fd);

/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose (const int fd);

/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */

void serialPutchar (const int fd, const unsigned char c);

/*
 * serialPuts:
 *	Send a string to the serial port
 *********************************************************************************
 */

void serialPuts (const int fd, const char *s);

/*
 * serialPrintf:
 *	Printf over Serial
 *********************************************************************************
 */

void serialPrintf (const int fd, const char *message, ...);


/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int serialDataAvail (const int fd);


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

int serialGetchar (const int fd);

int serialGetarray(const int fd, uint8_t out[], unsigned int size);
int32_t serialGetLine(const int fd, uint8_t out[], uint32_t max_size);


#endif