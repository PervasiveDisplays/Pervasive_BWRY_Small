//
// Pervasive_BWRY_Small.cpp
// Class library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
//
// Copyright (c) Pervasive Displays, 2010-2025
// Licence All rights reserved
//
// See Pervasive_BWRY_Small.h for references
//
// Release 611: Added proxy functions to normal update
// Release 611: Added support for red and yellow colour screens
// Release 614: Improved BWRY examples
// Release 702: Added support for 417-QS-0A
// Release 700: Refactored screen and board functions
// Release 800: Read OTP memory
// Release 801: Improved OTP implementation
// Release 802: Added references to application notes
// Release 802: Refactored CoG functions
// Release 900: Added new driver library
// Release 904: Added support for 206-QS-06
// Release 905: Added support for 437-QS-0B
//

// Header
#include "Pervasive_BWRY_Small.h"

//
// === COG section
//
//
// --- Small screens with Q film
//
void Pervasive_BWRY_Small::COG_reset()
{
	switch (u_eScreen_EPD)
	{
		case eScreen_EPD_154_QS_0F: // 1.54”
		case eScreen_EPD_213_QS_0F: // 2.13”
		    b_reset(10, 10, 20, 40, 10);
			break;
			
		default:
			b_reset(20, 10, 20, 10, 10);
			break;
	}
	b_waitBusy(); // BWRY specific
}

void Pervasive_BWRY_Small::COG_getDataOTP()
{
    hV_HAL_SPI_end(); // With unicity check
    hV_HAL_SPI3_begin(); // Define 3-wire SPI pins

    // 1.6 Read OTP memory mapping data
    uint16_t _chipId;
    uint16_t _readBytes = 0;
    u_flagOTP = false;

    // Size cSize cType Driver
    switch (u_eScreen_EPD)
    {
		case eScreen_EPD_154_QS_0F: // 1.54”
		case eScreen_EPD_213_QS_0F: // 2.13”
			_chipId = 0x0302;
			_readBytes = 48;
			break;
			
		case eScreen_EPD_266_QS_0F: // 2.66”

			_chipId = 0x8302;
			_readBytes = 48;
			break;

		case eScreen_EPD_417_QS_0A: // 4.17”

			_chipId = 0x0605;
			_readBytes = 112;
			break;

		case eScreen_EPD_206_QS_06: // 2.06”

			_chipId = 0xc901;
			_readBytes = 48;
			break;

		case eScreen_EPD_437_QS_0B: // 4.37”

			_chipId = 0x0b04;
			_readBytes = 112;
			break;
			
		default:

			break;
    }

    // GPIO
    // COG_reset(); // Although not mentioned, reset to ensure stable state?
    hV_HAL_GPIO_set(b_pin.panelReset);

    // Check
    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x70);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_delayMilliseconds(8); // Added

    uint16_t ui16 = 0;
    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    ui16 = hV_HAL_SPI3_read();
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    ui16 <<= 8;
    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    ui16 |= hV_HAL_SPI3_read();
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

    hV_HAL_Serial_crlf();
    if (ui16 == _chipId)
    {
        hV_HAL_log(LEVEL_INFO, "OTP check 1 passed - Chip ID %04x as expected", ui16);
    }
    else
    {
        hV_HAL_Serial_crlf();
        hV_HAL_log(LEVEL_CRITICAL, "OTP check 1 failed - Chip ID 0x%04x, expected 0x%04x", ui16, _chipId);
        hV_HAL_exit(0x01);
    }

    // Read OTP
    uint8_t ui8 = 0;
    uint16_t offset = 0x0000;

    if ((_chipId == 0x8302) or (_chipId == 0x0302))
    {
        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa4);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x15);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x01);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

		b_waitBusy();
        
		hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa1);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        ui8 = hV_HAL_SPI3_read();
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        // Check table start and set bank offset
        if (COG_data[0] == 0xa5)
        {
            offset = 0x00;
        }
        else
        {
            hV_HAL_Serial_crlf();
            hV_HAL_log(LEVEL_CRITICAL, "OTP check 2 failed - Bank %i, first 0x%02x, expected 0x%02x", 0, COG_data[0], 0xa5);
            hV_HAL_exit(0x01);
        }
    }
    else if (_chipId == 0x0605)
    {
        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa2);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x15);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa0);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x92);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_delayMilliseconds(10);

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        ui8 = hV_HAL_SPI3_read(); // Dummy
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        if (COG_data[0] == 0xa5) // First byte check = 0xa5
        {
            offset = 0x00;
        }
        else
        {
            offset = 0x70;
            for (uint16_t index = 1; index < offset; index += 1)
            {
                hV_HAL_GPIO_clear(b_pin.panelCS); // Select
                hV_HAL_SPI3_read(); // Ignore bytes 1..offset
                hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
            }

            hV_HAL_GPIO_clear(b_pin.panelCS); // Select
            COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
            hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

            if (COG_data[0] == 0xa5) // First byte check = 0xa5
            {
                offset = 0x70;
            }
            else
            {
                hV_HAL_Serial_crlf();
                hV_HAL_log(LEVEL_CRITICAL, "OTP check 2 failed - Bank %i, first 0x%02x, expected 0x%02x", 0, COG_data[0], 0xa5);
                hV_HAL_exit(0x01);
            }
        }
    }
	else if (_chipId == 0xc901)
    {
		hV_HAL_GPIO_clear(b_pin.panelDC); // Command
		hV_HAL_GPIO_clear(b_pin.panelCS); // Select
		hV_HAL_SPI3_write(0xf0);
		hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
		
		hV_HAL_GPIO_set(b_pin.panelDC); // Data
		hV_HAL_GPIO_clear(b_pin.panelCS); // Select
		hV_HAL_SPI3_write(0x0b);
		hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x90);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

		b_waitBusy();
		
        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa2);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x33);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa0);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

		b_waitBusy();

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xf6);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x0d);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
		hV_HAL_GPIO_clear(b_pin.panelCS); // Select
		hV_HAL_SPI3_write(0x80);
		hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x92);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_delayMilliseconds(10);

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        ui8 = hV_HAL_SPI3_read(); // Dummy
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        if (COG_data[0] == 0xa5) // First byte check = 0xa5
        {
            offset = 0x00;
        }
        else
        {
            offset = 0x00;
            for (uint16_t index = 1; index < offset; index += 1)
            {
                hV_HAL_GPIO_clear(b_pin.panelCS); // Select
                hV_HAL_SPI3_read(); // Ignore bytes 1..offset
                hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
            }

            hV_HAL_GPIO_clear(b_pin.panelCS); // Select
            COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
            hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

            if (COG_data[0] == 0xa5) // First byte check = 0xa5
            {
                offset = 0x00;
            }
            else
            {
                hV_HAL_Serial_crlf();
                hV_HAL_log(LEVEL_CRITICAL, "OTP check 2 failed - Bank %i, first 0x%02x, expected 0x%02x", 0, COG_data[0], 0xa5);
                hV_HAL_exit(0x01);
            }
        }
    }
    else if (_chipId == 0x0b04)
    {
		hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x90);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        
		hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xa2);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x15);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x00);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0xe0);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
		
        hV_HAL_GPIO_clear(b_pin.panelDC); // Command
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        hV_HAL_SPI3_write(0x92);
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_delayMilliseconds(10);

        hV_HAL_GPIO_set(b_pin.panelDC); // Data
        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        ui8 = hV_HAL_SPI3_read(); // Dummy
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        if (COG_data[0] == 0xa5) // First byte check = 0xa5
        {
            offset = 0x00;
        }
        else
        {
            offset = 0x70;
            for (uint16_t index = 1; index < offset; index += 1)
            {
                hV_HAL_GPIO_clear(b_pin.panelCS); // Select
                hV_HAL_SPI3_read(); // Ignore bytes 1..offset
                hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
            }

            hV_HAL_GPIO_clear(b_pin.panelCS); // Select
            COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
            hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

            if (COG_data[0] == 0xa5) // First byte check = 0xa5
            {
                offset = 0x70;
            }
            else
            {
                hV_HAL_Serial_crlf();
                hV_HAL_log(LEVEL_CRITICAL, "OTP check 2 failed - Bank %i, first 0x%02x, expected 0x%02x", 0, COG_data[0], 0xa5);
                hV_HAL_exit(0x01);
            }
        }
    }
	hV_HAL_log(LEVEL_INFO, "OTP check 2 passed - Bank %i, first 0x%02x as expected", (offset > 0x00), COG_data[0]);

    // Populate COG_data
    for (uint16_t index = 1; index < _readBytes; index += 1)
    {
        digitalWrite(b_pin.panelCS, LOW); // Select
        COG_data[index] = hV_HAL_SPI3_read(); // Read OTP
		hV_HAL_log(LEVEL_INFO, "OTP memory [%i] : 0x%02x", index, COG_data[index]);
        digitalWrite(b_pin.panelCS, HIGH); // Unselect
    }

    u_flagOTP = true;

#if (DEBUG_OTP == 1) // Debug COG_data
    debugOTP(COG_data, _readBytes, COG_BWRY_SMALL, SCREEN_DRIVER(u_eScreen_EPD));
#endif // DEBUG_OTP
}

void Pervasive_BWRY_Small::COG_initial()
{
    // Application note § 3. COG initial
    b_sendCommandData8(0xe0, 0x02);
	b_sendCommandData8(0xe6, 0x19); // 25C temp

    switch (u_eScreen_EPD)
    {
		case eScreen_EPD_206_QS_06:
			b_sendCommand8(0xa5);
			b_waitBusy();
			b_sendIndexData(0x01, &COG_data[16], 2);
			b_sendIndexData(0x00, &COG_data[18], 2);
			b_waitBusy();
			b_sendIndexData(0x61, &COG_data[20], 4);
			b_waitBusy();
			b_sendIndexData(0x06, &COG_data[24], 4);
			b_sendIndexData(0x03, &COG_data[30], 1);
			b_sendIndexData(0xe7, &COG_data[33], 1);
			b_sendIndexData(0x65, &COG_data[34], 4);
			b_sendIndexData(0x30, &COG_data[38], 1);
			b_sendIndexData(0x50, &COG_data[39], 1);
			b_sendIndexData(0x60, &COG_data[40], 2);
			b_sendIndexData(0xe3, &COG_data[42], 1);
			b_sendIndexData(0x62, &COG_data[43], 2);
			b_sendCommandData8(0xe9, 0x01);
			break;

		case eScreen_EPD_154_QS_0F: // 1.54”
		case eScreen_EPD_213_QS_0F: // 2.13”		
		case eScreen_EPD_266_QS_0F:
			b_sendCommand8(0xa5);
			b_waitBusy();
			b_sendIndexData(0x01, &COG_data[16], 1);
			b_sendIndexData(0x00, &COG_data[17], 2);
			b_sendIndexData(0x03, &COG_data[30], 3);
			b_sendIndexData(0x06, &COG_data[23], 7);
			
			b_sendCommandData8(0x50, COG_data[39]);
			b_sendIndexData(0x60, &COG_data[40], 2);
			b_sendIndexData(0x61, &COG_data[19], 4);
			b_sendCommandData8(0xe7, COG_data[33]); //
			b_sendCommandData8(0xe3, COG_data[42]);

			b_sendCommandData8(0x4d, COG_data[43]); //
			b_sendCommandData8(0xb4, COG_data[44]); //
			b_sendCommandData8(0xb5, COG_data[45]); //

			b_sendCommandData8(0xe9, 0x01); //
			b_sendCommandData8(0x30, 0x08); // PLL
			
			break;
			
		case eScreen_EPD_417_QS_0A:
			b_sendIndexData(0x01, &COG_data[16], 1);
			b_sendIndexData(0x00, &COG_data[17], 2);
			b_sendIndexData(0x03, &COG_data[30], 3);
			b_sendIndexData(0x06, &COG_data[23], 3);
			b_sendCommandData8(0x50, COG_data[39]);
			b_sendIndexData(0x60, &COG_data[40], 2);
			b_sendIndexData(0x61, &COG_data[19], 4);
			b_sendCommandData8(0xe3, COG_data[42]);
			b_sendCommandData8(0xe7, COG_data[33]);
			b_sendIndexData(0x65, &COG_data[34], 4);
			b_sendCommandData8(0x30, COG_data[38]);
			b_sendCommandData8(0xe9, 0x01);
			b_sendCommand8(0x04); // Power on
			b_waitBusy();

		case eScreen_EPD_437_QS_0B:
			b_sendCommand8(0xa5);
			b_waitBusy();
			b_sendIndexData(0x00, &COG_data[17], 2);
			b_sendIndexData(0x01, &COG_data[16], 1);
			b_sendIndexData(0x03, &COG_data[30], 3);
			b_sendIndexData(0x06, &COG_data[23], 3);
			b_sendIndexData(0x30, &COG_data[38], 1);
			b_sendIndexData(0x50, &COG_data[39], 1);
			b_sendIndexData(0x60, &COG_data[40], 2);			
			b_sendIndexData(0x61, &COG_data[19], 4);			
			b_sendIndexData(0x65, &COG_data[34], 4);
			b_sendIndexData(0xe7, &COG_data[33], 1);
			b_sendIndexData(0xe3, &COG_data[42], 1);
			b_sendCommandData8(0xe9, 0x01);
			break;

		default:        
			break;
    }
}

void Pervasive_BWRY_Small::COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame) // First frame, blackBuffer
{
    // Application note § 4. Input image to the EPD
    b_sendIndexData(0x10, frame, sizeFrame); // First frame, blackBuffer
}

void Pervasive_BWRY_Small::COG_update()
{
    switch (u_eScreen_EPD)
    {
		case eScreen_EPD_417_QS_0A:

			break;

		case eScreen_EPD_437_QS_0B:
			b_sendCommandData8(0xff, 0xa5); //
			b_sendIndexData(0xef, &COG_data[43], 8);
			b_sendCommandData8(0xc3, COG_data[64]);			
			b_sendCommandData8(0xdc, COG_data[59]);
			b_sendCommandData8(0xdd, COG_data[60]);
			b_sendCommandData8(0xde, COG_data[61]);
			b_sendCommandData8(0xfd, COG_data[65]);			
			b_sendCommandData8(0xe8, COG_data[62]);
			b_sendCommandData8(0xda, COG_data[63]);
			b_sendCommandData8(0xc9, COG_data[67]);
			b_sendCommandData8(0xa8, COG_data[66]);
			b_sendCommandData8(0xff, 0xe3); //
			b_sendCommand8(0x04); // Power on
			b_waitBusy();
			
			b_sendCommandData8(0xff, 0xa5); //
			b_sendIndexData(0xef, &COG_data[51], 8);
			b_sendCommandData8(0xc3, COG_data[64]);			
			b_sendCommandData8(0xdc, COG_data[59]);
			b_sendCommandData8(0xdd, COG_data[60]);
			b_sendCommandData8(0xde, COG_data[61]);
			b_sendCommandData8(0xfd, COG_data[65]);			
			b_sendCommandData8(0xe8, COG_data[62]);
			b_sendCommandData8(0xda, COG_data[63]);
			b_sendCommandData8(0xc9, COG_data[67]);
			b_sendCommandData8(0xa8, COG_data[66]);
			b_sendCommandData8(0xff, 0xe3); //
			break;

		default:

			b_sendCommand8(0x04); // Power on
			b_waitBusy();
			break;
    }
	
    b_sendCommandData8(0x12, 0x00); // Display Refresh
    b_waitBusy();	
}

void Pervasive_BWRY_Small::COG_stopDCDC()
{
    b_sendCommandData8(0x02, 0x00); // Turn off DC/DC
    b_waitBusy();

    switch (u_eScreen_EPD)
    {
		case eScreen_EPD_206_QS_06:
			b_sendCommandData8(0x07, 0xa5);
			hV_HAL_delayMilliseconds(50);
			// clear all IOs to LOW
			// delay 1000 ms
			// cut Vcc off
			// delay 200 ms
			break;

		case eScreen_EPD_266_QS_0F:
			// clear all IOs to LOW
			// cut Vcc off
			// delay 100 ms
			// set RES pin to FLOAT
			// delay 100 ms
			break;

		case eScreen_EPD_417_QS_0A:
			hV_HAL_delayMilliseconds(5000);
			b_sendIndexData(0x00, &COG_data[26], 2); // PSR
			hV_HAL_delayMilliseconds(100);
			// clear all IOs to LOW
			// cut Vcc off
			// set RES pin to FLOAT
			// delay 200 ms
			break;

		case eScreen_EPD_437_QS_0B:
			hV_HAL_delayMilliseconds(200);
			// clear all IOs to LOW
			// delay 50 ms
			// cut Vcc off
			// delay 100 ms
			break;

		default:

			break;
    }
}
//
// --- End of Small screens with Q film
//
/// @endcond
//
// === End of COG section
//

Pervasive_BWRY_Small::Pervasive_BWRY_Small(eScreen_EPD_t eScreen_EPD, pins_t board)
{
    d_COG = COG_BWRY_SMALL;
    u_eScreen_EPD = eScreen_EPD;
    b_pin = board;
}

void Pervasive_BWRY_Small::begin()
{
    b_begin(b_pin, FAMILY_SMALL, b_delayCS);
    b_resume(); // GPIO

    COG_reset(); // Reset
    COG_getDataOTP(); // 3-wire SPI read OTP memory
}

STRING_CONST_TYPE Pervasive_BWRY_Small::reference()
{
    return formatString("%s v%i.%i.%i", DRIVER_EPD_VARIANT, DRIVER_EPD_RELEASE / 100, (DRIVER_EPD_RELEASE / 10) % 10, DRIVER_EPD_RELEASE % 10);
}

void Pervasive_BWRY_Small::updateNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame)
{
    b_resume(); // GPIO
    COG_reset(); // Reset

    if (u_flagOTP == false)
    {
        COG_getDataOTP(); // 3-wire SPI read OTP memory
        COG_reset(); // Reset
    }

    // Start SPI
    hV_HAL_SPI_begin(16000000); // Fast 16 MHz, with unicity check

    COG_initial(); // Initialise
    COG_sendImageDataNormal(frame, sizeFrame);

    COG_update(); // Update
    COG_stopDCDC(); // Power DC/DC off
}

