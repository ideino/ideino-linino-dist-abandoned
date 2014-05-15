/*
 * StreamSPI.h - Hardware SPI stream
 *
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author Federico Vaga <federicov@linino.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef HardwareSPIstream_h
#define HardwareSPIstream_h

#include <inttypes.h>
#include "SPI_Class.h"
#include "Stream.h"

#define SPI_DEFAULT_BUFFER_SIZE	64
#define SPI_DEFAULT_IGNORE_TX 0xFF
#define SPI_DEFAULT_IGNORE_RX 0xFF
#define SPI_IGNORE_CODE_LENGHT_RX 2
#define SPI_IGNORE_CODE_LENGHT_TX 2

#define SPI_DEFAULT_MAXIMUM_WAIT_CLOCK 50	/* milli-seconds */


#define SPI_TX_FLAG_REQ_TRANS	(1 << 0)
#define SPI_FLAG_WAIT_LENGTH (1 << 0)
#define SPI_FLAG_WAIT_TRANSFER (1 << 1)
#define SPI_FLAG_IGNORE_RX (1 << 2)

#define SPI_OP_STORE_RX		(1 << 0)
#define SPI_OP_RETRIEVE_TX	(1 << 1)
#define SPI_OP_LENGTH_TX	(1 << 2)

enum buffer_type {
	TX_BUFFER,
	RX_BUFFER,
};

enum stm_status {
	STM_WAIT_LENGTH,
	STM_CHECK_BYTE1,
	STM_CHECK_BYTE2,
	STM_STORE_BYTES,
	STM_IGNORE_BYTES,
};

class StreamSPI : public Stream
{
	protected:
	SPIClass spi;
	unsigned long flags;

	uint8_t rx_length;
	uint8_t rx_byte_left;

	uint8_t tx_length;
	uint8_t tx_byte_left;

	uint8_t *rx_buffer;	/* Allocated on begin(), freed on end() */
	uint8_t *tx_buffer;	/* Allocated on begin(), freed on end() */
	unsigned int buffer_size;
	/*
	 * head: is where we put a value
	 * tail: is where we get a value
	 *
	 * head == tail     ----> the buffer is empty
	 * head == tail + 1 ----> the buffer is full
	 */
	volatile uint8_t *rx_head;
	volatile uint8_t *tx_head;
	volatile uint8_t *rx_tail;
	volatile uint8_t *tx_tail;

	volatile unsigned int tx_flag;
	virtual void raiseInterrupt();
	virtual void waitRequestByteTransfer();

	enum stm_status status;

	/*
	 * In order to allow the transmission of the clock without data, it si
	 * necessary an invalid byte to ignore. In the serial stream is used
	 * for binary data transmission all values are valid, so it is
	 * necessary a multi-byte code.
	 *
	 * If the value to ignore is repeated three consecutive time, it means
	 * that the value is valid.
	 * If the value to ignore occur just one time, followed by two 0x0 bytes
	 * it means that the byte is to ignore.
	 *
	 * The main processor will send at least 3 bytes when it is sending the
	 * invalid character.
	 *
	 * On this side on the bus, the processor use 2 bytes to send the
	 * invalid character. The first byte is the invalid character,
	 * followed by 0x0 if the byte is to ignore.
	 */
	uint8_t tx_ignore;
	uint8_t rx_ignore;

	uint8_t tx_last_ignore;
	uint8_t rx_buf_ignore[SPI_IGNORE_CODE_LENGHT_RX];

	unsigned int tx_ignore_index;
	unsigned int rx_ignore_index;

	unsigned int maximum_wait_clock;

	public:
	StreamSPI(SPIClass spidev);
	int begin();
	int begin(unsigned int buf_size, unsigned int spi_mode);
	void end();

	int storeTX(uint8_t val);
	uint8_t retrieveTX();
	int storeRX(uint8_t val);
	uint8_t retrieveRX();

	virtual unsigned long checkInterrupt(uint8_t val);
	virtual uint8_t getLengthTX();

	/* From Stream.h */
	virtual int available(void);
	virtual int peek(void);
	virtual int read(void);
	virtual void flush(void);
	/* From Print.h */
	virtual size_t write(uint8_t val);
	inline size_t write(unsigned long n) { return write((uint8_t)n); }
	inline size_t write(long n) { return write((uint8_t)n); }
	inline size_t write(unsigned int n) { return write((uint8_t)n); }
	inline size_t write(int n) { return write((uint8_t)n); }
};

extern StreamSPI StreamSPI0;

#endif // HardwareSPIstream_h
