/*
 * StreamSPI.cpp - Hardware SPI stream
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

#include "StreamSPI.h"
//#define DEBUG 1
//#define VDEBUG 1

/* Preinstantiate objects */
StreamSPI StreamSPI0(SPI);

StreamSPI::StreamSPI(SPIClass spidev)
{
	spi = spidev;
}

int StreamSPI::begin()
{
	#if DEBUG
	Serial.begin(250000);
	#endif
	return StreamSPI::begin(SPI_DEFAULT_BUFFER_SIZE, SPI_MODE0);
}

int StreamSPI::begin(unsigned int buf_size, unsigned int spi_mode)
{
	int err;

	buffer_size = buf_size;

	/* Allocates RX and TX buffers */
	rx_buffer = (uint8_t *) malloc(buf_size);
	if (!rx_buffer)
		return -1;

	tx_buffer = (uint8_t *) malloc(buf_size);
	if (!rx_buffer) {
		free(rx_buffer);
		return -1;
	}

	/* Clean up memory */
	memset(rx_buffer, 0, buf_size);
	memset(tx_buffer, 0, buf_size);

	/* Reset buffer current position */
	rx_head = rx_buffer;
	tx_head = tx_buffer;
	rx_tail = rx_buffer;
	tx_tail = tx_buffer;

	/* * * Configure SPI as Slave * * */
	pinMode(MISO, OUTPUT);	/* We have to send on master in, *slave out* */
	pinMode(SS, INPUT);
	pinMode(SCK, INPUT);
	pinMode(MOSI, INPUT);

	/* turn on SPI in slave mode */
	SPCR |= _BV(SPE);
	SPCR &= ~_BV(MSTR);

	/* Initialize data before start interrupt */
	rx_ignore = SPI_DEFAULT_IGNORE_RX;
	tx_ignore = SPI_DEFAULT_IGNORE_TX;
	SPDR = 0;	/* first byte is length, then ignore it */
	tx_last_ignore = 0;

	spi.setDataMode(spi_mode);
	spi.attachInterrupt();
	PORTB &= ~(0x1);	/* Enable pull-up on SS signal */

	/* * * Configure Interrupt for main processor */
	ACSR  &= (~0x04); /* Disable Analog Comparator interrupt
	                     to prevent interrupt during disable */
	ACSR  &= (~0x80); /* Disable Analog Comparator on PE6 */
	EIMSK &= (~0x40); /* Disable INT6 on PE6 */

	PORTE &= (~0x40); /* Set PE6 to 0 */
	DDRE  |= (0x40);  /* Set PE6 as Output */

	tx_flag = 0;

	maximum_wait_clock = SPI_DEFAULT_MAXIMUM_WAIT_CLOCK;

	/* Nuovo */
	flags |= SPI_FLAG_WAIT_LENGTH;

	rx_length = 0;
	rx_byte_left = 0;

	tx_length = 0;
	tx_byte_left = 0;

	status = STM_WAIT_LENGTH;

	return 0;
}

void StreamSPI::end()
{
	spi.detachInterrupt();
	free(tx_buffer);
	free(rx_buffer);
}

void StreamSPI::raiseInterrupt()
{
	#if DEBUG
	Serial.println("Do interrupt ===");
	#endif

	/*
	 * Do not raise another interrupt if the driver is already waiting
	 * for it
	 */
	if (flags & SPI_FLAG_WAIT_TRANSFER) {
	#if DEBUG
	Serial.println("Pending interrupt ===");
	#endif
		return;
	}

	flags |= SPI_FLAG_WAIT_TRANSFER;
	PORTE |= 0x40;
	PORTE &= ~0x40;
}

void StreamSPI::waitRequestByteTransfer()
{
	int max_try = maximum_wait_clock + 1;

	/* Raise an interrupt to request transmission */
	raiseInterrupt();

	/* Wait until transmission occur */
	while ((--max_try) && (flags & SPI_FLAG_WAIT_TRANSFER))
		delay(1);

	/*
	 * If we waited too much to obtain the clock, probably the SPI TTY
	 * driver is not loaded on the other side. So, consume the byte and
	 * continue.
	 */
	if (!max_try) {
		#if DEBUG
		Serial.println("No interrupt occur ===");
		#endif
		retrieveTX();
	} else {
		#if DEBUG
		Serial.print(max_try, DEC);
		Serial.println(" Interrupt done ===");
		#endif
	}
}


/*
 * Write on the receive buffer. Here we have to control the incoming frames.
 * It works like a state machine.
 *
 * 1. wait for the byte that tell us the length of the frame
 * 2. store byte until the end of frame
 * 3. start again from step (1)
 */
int StreamSPI::storeRX(uint8_t val)
{
	/*
	 * FIXME here we can loose bytes because buffer is full and we cannot
	 * do anything to consume it. It is duty of the program to consume it
	 */
	if (rx_head == rx_tail - 1 || (rx_tail == rx_buffer && rx_head == rx_buffer + buffer_size - 1))
		return 0;	/* Buffer is full */

	#if DEBUG
	Serial.print("RX store  Head pre: ");
	Serial.print((unsigned long)rx_head, HEX);
	Serial.print(" | RX store  Tail pre: ");
	Serial.print((unsigned long)rx_tail, HEX);
	Serial.print(" | val: ");
	Serial.print(val, HEX);
	Serial.println(" ===");
	#endif

	*rx_head = val;
	rx_head = rx_head < rx_buffer + buffer_size - 1 ? rx_head + 1 : rx_buffer;

	#if VDEBUG
	Serial.print("TX store  Head post: ");
	Serial.print((unsigned long)tx_head, HEX);
	Serial.print(" | TX store  Tail post: ");
	Serial.print((unsigned long)tx_tail, HEX);
	Serial.println(" ===");
	#endif

	return 1;
}


/*
 * Write on the transmission buffer. This function write the incoming byte
 * into the TX buffer (if it is not full). If the byte to write is the
 * 'ignore' byte, then it replace it with its escape code.
 */
int StreamSPI::storeTX(uint8_t val)
{
	int n;

	/*
	 * FIXME here we can loose bytes because buffer is full and we cannot
	 * do anything to consume it. It is duty of the program to consume it
	 */
	if (tx_head == tx_tail - 1 || (tx_tail == tx_buffer && tx_head == tx_buffer + buffer_size - 1)) {
		#if DEBUG
		Serial.print("Buffer FULL");
		Serial.print(val, HEX);
		Serial.println(" ===");
		#endif
		return 0;	/* Buffer is full */
	}

	#if DEBUG
	Serial.print("TX store  Head pre: ");
	Serial.print((unsigned long)tx_head, HEX);
	Serial.print(" | TX store  Tail pre: ");
	Serial.print((unsigned long)tx_tail, HEX);
	Serial.print(" | val: ");
	Serial.print(val, HEX);
	Serial.println(" ===");
	#endif

	*tx_head = val;
	tx_head = tx_head < tx_buffer + buffer_size - 1 ? tx_head + 1 : tx_buffer;
	tx_length++;

	#if VDEBUG
	Serial.print("TX store  Head post: ");
	Serial.print((unsigned long)tx_head, HEX);
	Serial.print(" | TX store  Tail post: ");
	Serial.print((unsigned long)tx_tail, HEX);
	Serial.println(" ===");
	#endif

	/*
	 * When storing the invalid value in the transmission buffer, this
	 * function must repeat the value in order to build the code to
	 * validate the byte.
	 */
	if (val == tx_ignore && tx_ignore_index < SPI_IGNORE_CODE_LENGHT_TX - 1) {
		tx_ignore_index++;
		write(tx_ignore);
	} else {
		tx_ignore_index = 0;
	}

	return 1;
}


/*
 * Read from the receive buffer. There is no special control to do, if a byte
 * is stored in the RX buffer, then is ready to be pushed to the application.
 *
 * This function just get a byte from the buffer and update the buffer's
 * index.
 */
uint8_t StreamSPI::retrieveRX()
{
	uint8_t val;

	if (rx_head == rx_tail)
		return 0;	/* There are no byte to read */

	#if DEBUG
	Serial.print("RX retrieve  Head pre: ");
	Serial.print((unsigned long)rx_head, HEX);
	Serial.print(" | RX retrieve  Tail pre: ");
	Serial.print((unsigned long)rx_tail, HEX);
	Serial.print(" | val: ");
	Serial.print(*rx_tail, HEX);
	Serial.println(" ===");
	#endif

	val = *rx_tail;
	rx_tail = rx_tail < rx_buffer + buffer_size - 1 ? rx_tail + 1 : rx_buffer;

	#if VDEBUG
	Serial.print("RX retrieve  Head post: ");
	Serial.print((unsigned long)rx_head, HEX);
	Serial.print(" | RX retrieve  Tail post: ");
	Serial.print((unsigned long)rx_tail, HEX);
	Serial.println(" ===");
	#endif

	return 	val;
}

uint8_t StreamSPI::retrieveTX()
{
	uint8_t val;

	/*
	 * When we retrieve a value from he transmission buffer it to
	 * send it. So here we can clear the transmission request flag
	 * because we are going to do it.
	 */
	flags &= ~SPI_FLAG_WAIT_TRANSFER;

	#if DEBUG
	Serial.print("TX retrieve  Head pre: ");
	Serial.print((unsigned long)tx_head, HEX);
	Serial.print(" | TX retrieve  Tail pre: ");
	Serial.print((unsigned long)tx_tail, HEX);
	Serial.print(" | val: ");
	Serial.print(*tx_tail, HEX);
	Serial.println(" ===");
	#endif

	if (tx_head == tx_tail || tx_last_ignore == tx_ignore) {
		/*
		 * There are no byte to send, so send invalid bytes.
		 * Invalid transmission byte is 2 byte length so we can just
		 * toggle the value on each transfer.
		 *
		 * If the buffer become not empty, but it was sent only half
		 * of the code, we MUST send the other part of it. (this is
		 * the reason of the || in the if)
		 */
		tx_last_ignore = (tx_last_ignore == tx_ignore ? 0x0 : tx_ignore);

		#if DEBUG
		Serial.print("TX ignore retrieve: ");
		Serial.print((unsigned long)tx_last_ignore, HEX);
		Serial.println(" ===");
		#endif

		return tx_last_ignore;
	}

	val = *tx_tail;
	tx_tail = tx_tail < tx_buffer + buffer_size - 1 ? tx_tail + 1 : tx_buffer;
	tx_length--;

	#if VDEBUG
	Serial.print("TX retrieve  Head post: ");
	Serial.print((unsigned long)tx_head, HEX);
	Serial.print(" | TX retrieve  Tail post: ");
	Serial.print((unsigned long)tx_tail, HEX);
	Serial.println(" ===");
	#endif

	return 	val;
}

uint8_t StreamSPI::getLengthTX()
{
	#if DEBUG
	Serial.print("TX Len: ");
	Serial.println(tx_length, DEC);
	#endif

	return tx_length;
}

/*
 * It check the incoming byte and it compares the value with the current
 * state of the driver to decide the next step.
 *
 *  < 0 : Error
 *  0 : Store the value
 *  1 : Skip value
 *
 */
unsigned long StreamSPI::checkInterrupt(uint8_t val)
{
	unsigned long op = 0;

	op |= SPI_OP_RETRIEVE_TX;

#if DEBUG
	Serial.print("Status: ");
	Serial.println(status, DEC);
#endif

	if (status != STM_WAIT_LENGTH && rx_byte_left > 0)
		rx_byte_left--;

	/* handle state machine status */
	switch(status) {
	case STM_WAIT_LENGTH:
		/* update indexes */
		rx_length = val;
		rx_byte_left = rx_length;
		tx_last_ignore = 0;
		rx_ignore_index = 0;

		/* Set up operations */
		op &= ~SPI_OP_STORE_RX;		/* do not store */

		if (rx_length == 0) {
			op &= ~SPI_OP_RETRIEVE_TX;
			op |= SPI_OP_LENGTH_TX;	/* get TX length */
		} else {
			/* update state machine status */
			status = STM_CHECK_BYTE1;
		}

		break;
	case STM_CHECK_BYTE1:
		rx_buf_ignore[rx_ignore_index] = val;
		rx_ignore_index++;
		if (val == rx_ignore) {
			/* Set up operations */
			op &= ~SPI_OP_STORE_RX;	/* do not store */
			/* update state machine status */
			status = STM_CHECK_BYTE2;
		} else {
			/* Set up operations */
			op |= SPI_OP_STORE_RX;	/* store */
			/* update state machine status */
			status = STM_STORE_BYTES;
		}
		break;
	case STM_CHECK_BYTE2:
		rx_buf_ignore[rx_ignore_index] = val;
		rx_ignore_index++;
		if (val == rx_ignore) {
			/* Set up operations */
			op |= SPI_OP_STORE_RX;	/* store */
			/* update state machine status */
			status = STM_STORE_BYTES;
		} else {
			/* Set up operations */
			op &= ~SPI_OP_STORE_RX;	/* do not store */
			/* update state machine status */
			status = STM_IGNORE_BYTES;
		}
		break;
	case STM_IGNORE_BYTES:
		/* Set up operations */
		op &= ~SPI_OP_STORE_RX;	/* do not store */
		break;
	case STM_STORE_BYTES:
		/* Set up operations */
		op |= SPI_OP_STORE_RX;	/* store */
		break;
	}

	if (rx_byte_left == 0) {
		/*
		 * When the driver reach 0, it means that a frame is over,
		 * so do not retrieve the next byte because it is the length
		 * of the next frame.
		 */
		op &= ~SPI_OP_RETRIEVE_TX;
		status = STM_WAIT_LENGTH;

		#if DEBUG
		Serial.println("EOF");
		#endif
	}

out:
	return op;
}

/*
 * Each time a byte transfer is complete, the AVR invokes this
 * interrupt which stores the incoming byte and write the next
 * byte to transfer to the master
 */
#ifdef SPI_STC_vect
ISR (SPI_STC_vect)
{
	unsigned long op;
	uint8_t val = 0, status;
	int err;

	status = SPSR; /* in order to clear flags */
	val = SPDR;
	op = StreamSPI0.checkInterrupt(val);

	#if DEBUG
	Serial.print("Operations: 0x");
	Serial.print((unsigned long)op, HEX);
	Serial.println(" ===");
	#endif

	/* Retrieve the next byte to send and store the incoming byte */
	if (op & SPI_OP_STORE_RX)
		err = StreamSPI0.storeRX(val);

	if (op & SPI_OP_RETRIEVE_TX)
		val = StreamSPI0.retrieveTX();
	else
		val = StreamSPI0.getLengthTX();

	SPDR = val;
}
#endif

/* * * Stream methods implementations * * */
int StreamSPI::available(void)
{
	return (rx_tail != rx_head);
}

int StreamSPI::peek(void)
{
	if (rx_head == rx_tail)
		return -1; /* No byte available*/
	else
		return *rx_tail;
}

int StreamSPI::read(void)
{
	/* FIXME HardwareSerial return -1 when empty, not 0 (EOF) */
	return retrieveRX();
}

void StreamSPI::flush(void)
{
	unsigned int tmp;

	/* Continue transmission until buffer is empty */
	while (tx_head != tx_tail) {
		#if DEBUG
		Serial.print("TX flush  Head: ");
		Serial.print((unsigned long)tx_head, HEX);
		Serial.print(" | Tail: ");
		Serial.print((unsigned long)tx_tail, HEX);
		Serial.println(" ===");
		#endif
		waitRequestByteTransfer();
	}
}
/* * * Print methods implementations * * */
size_t StreamSPI::write(uint8_t val)
{
	int n;

	do {
		/* Try to store the value in the buffer */
		n = storeTX(val);

		/*
		 * TX buffer is full, transmit something
		 * FIXME I prefer to just return 0
		 */
		if (n == 0) {
			waitRequestByteTransfer();
		} else if (getLengthTX()) {
			waitRequestByteTransfer();
		}
	} while(n == 0);

	return 1;
}
