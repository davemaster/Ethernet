/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

/* For Max32 */
#define __32MX795F512L__ 1

#include <stdio.h>
#include <string.h>

#if defined(_BOARD_MEGA_)

// neither PROGMEM or PSTR are needed for PIC32, just define them as null
#define PROGMEM
#define PSTR(s) (s)
#define pgm_read_byte(x)	        (*((char *)x))
#define pgm_read_byte_near(x)	(*((char *)x))
#define pgm_read_byte_far(x)	(*((char *)x))
#define pgm_read_word(x)    	(*((short *)x))
#define pgm_read_word_near(x)	(*((short *)x))
#define pgm_read_workd_far(x)	(*((short *)x))

#define	prog_void	 const void
#define	prog_char	 const char
#define	prog_uchar	 const unsigned char
#define	prog_int8_t	 const int8_t
#define	prog_uint8_t	const uint8_t
#define	prog_int16_t	const int16_t
#define	prog_uint16_t	const uint16_t
#define	prog_int32_t	const int32_t
#define	prog_uint32_t	const uint32_t
#define	prog_int64_t	const int64_t
#define	prog_uint64_t	const uint64_t

#else
    #include <avr/pgmspace.h>
#endif

#include "w5100.h"

#if defined(W5500_ETHERNET_SHIELD)

// W5500 controller instance
W5500Class W5100;


void W5500Class::init(void)
{

    initSS();
    delay(300);
    SPI.begin();
    //SPI.setClockDivider(SPI_CLOCK_DIV128);

    for (int i=0; i<MAX_SOCK_NUM; i++) {
        uint8_t cntl_byte = (0x0C + (i<<5));
        write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
        write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
    }
}

uint16_t W5500Class::getTXFreeSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0)
            val = readSnTX_FSR(s);
    }
    while (val != val1);
    return val;
}

uint16_t W5500Class::getRXReceivedSize(SOCKET s)
{
    uint16_t val=0,val1=0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0)
            val = readSnRX_RSR(s);
    }
    while (val != val1);
    return val;
}

void W5500Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
  send_data_processing_offset(s, 0, data, len);

}

void W5500Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{

    uint16_t ptr = readSnTX_WR(s);
    uint8_t cntl_byte = (0x14+(s<<5));
    ptr += data_offset;
    write(ptr, cntl_byte, data, len);
    ptr += len;
    writeSnTX_WR(s, ptr);

}

void W5500Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr;
    ptr = readSnRX_RD(s);

    read_data(s, (uint8_t *)ptr, data, len);//  *************************** COMENTADO *************************
    if (!peek)
    {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}

///void read_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len);
void W5500Class::read_data(SOCKET s, volatile uint8_t *src, volatile uint8_t *dst, uint16_t len)
{
    uint8_t cntl_byte = (0x18+(s<<5));/*
    uint16_t temp=*(src)<<8+*(src+1);

    read(temp, cntl_byte, (uint8_t *)dst, len);*/
	
	uint16_t size;
	uint16_t src_mask;
	uint16_t src_ptr;

	src_mask = (uint16_t)((uint32_t)src & RMASK);	
	
	src_ptr = RBASE[s] + src_mask;

	if( (src_mask + len) > RSIZE ) 
	{
		size = RSIZE - src_mask;
		///read(src_ptr, (uint8_t *)dst, size);
		read(src_ptr, cntl_byte, (uint8_t *)dst, len);
		dst += size;
		///read(RBASE[s], (uint8_t *) dst, len - size);
		read(RBASE[s], cntl_byte, (uint8_t *)dst, len);
	} 
	else
		///read(src_ptr, (uint8_t *) dst, len);
		read(src_ptr, cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500Class::write(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    SPI.transfer(_data);
    resetSS();
    return 1;
}

uint16_t W5500Class::write(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        SPI.transfer(_buf[i]);
    }
    resetSS();
    return _len;
}

uint8_t W5500Class::read(uint16_t _addr, uint8_t _cb)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    uint8_t _data = SPI.transfer(0);
    resetSS();
    return _data;
}

uint16_t W5500Class::read(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        _buf[i] = SPI.transfer(0);
    }
    resetSS();

    return _len;
}

void W5500Class::execCmdSn(SOCKET s, SockCMD _cmd) {
    // Send command to socket
    writeSnCR(s, _cmd);
    // Wait for command to complete
    while (readSnCR(s))
    ;
}
#endif
