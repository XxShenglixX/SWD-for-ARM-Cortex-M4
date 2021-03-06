#include <stdint.h>
#include "unity.h"
#include "Emulator.h"
#include "Swd.h"
#include "IoOperations.h"
#include "mock_IoOperationsEx.h"
#include "mock_configurePort.h"

void setUp(void) {}

void tearDown(void) {}

void test_convertMSB_LSB_given_0xEE2805D4_should_return_0x2BA01477()
{
	TEST_ASSERT_EQUAL(0x2BA01477,interconvertMSBandLSB(0xEE2805D4));
}

void test_convertMSB_LSB_given_0x2BA01477_should_return_0xEE2805D4()
{
	TEST_ASSERT_EQUAL(0xEE2805D4,interconvertMSBandLSB(0x2BA01477));
}

void test_emulateWrite_given_0x2_and_number_of_bits_2()
{
  emulateWrite(0x2, 2);
  
	sendBits(0x2, 2);
}

void test_emulateWrite_given_0x5231_and_number_of_bits_14()
{
  emulateWrite(0x5231, 14);
  
	sendBits(0x5231, 14);
}

void test_emulateWrite_given_0xA1B2C3D4_and_number_of_bits_32()
{
  emulateWrite(0xA1B2C3D4, 32);
  
	sendBits(0xA1B2C3D4, 32);
}

void test_emulateRead_given_0x4_and_number_of_bits_3()
{
  uint32_t readData = 0;
  emulateRead(0x4, 3);
  
	readBits(&readData, 3);
  
  TEST_ASSERT_EQUAL(0x1, readData);
}

void test_emulateRead_given_0xEE2805D4_and_number_of_bits_32()
{
  uint32_t readData = 0;
  emulateRead(0xEE2805D4, 32);
  
	readBits(&readData, 32);
  
  TEST_ASSERT_EQUAL(0x2BA01477, readData);
}

void test_emulateTurnAroundRead_clock_should_off_once()
{
  emulateTurnAroundRead();
  
	turnAroundRead();
}

void test_emulateTurnAroundWrite_clock_should_on_off_on()
{
  emulateTurnAroundWrite();
  
	turnAroundWrite();
}

void test_emulateSwdInput()
{
  emulateSwdInput();
  
  setSWDIOInputMode();
}

void test_emulateSwdOutput()
{
  emulateSwdOutput();
  
  setSWDIOOutputMode();
}

void test_emulateIdleClock_should_generate_SWDIO_low_and_SWDCLK_OFF_ON_8_times()
{
  emulateIdleClock(8);
  
  extraIdleClock(8);
}

void test_emulateLineReset_given_60clock_should_generate_clock_cycles_with_setHighSWDIO()
{
	emulateLineReset(60);
	
	lineReset(60);
}

void test_emulatehardResetTarget_should_call_ResetPinLow_setHighNRST()
{
	emulatehardResetTarget();
	
	hardResetTarget();
}

void test_emulateswdRegisterWrite_should_send_SWD_Request_readACK_and_Write_data()
{
	emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, 0x2BA01477);
	swdRegisterWrite(TAR_REG, SWD_AP, 0x2BA01477);
}

void test_emulateswdRegisterRead_should_send_SWD_Request_readACK_and_readData_readParity()
{
	int ACK = 0;
	uint32_t dataRead;
	
	emulateSwdRegisterRead(TAR_REG, SWD_AP, OK, 0, 0xEE2805D4);
	swdRegisterRead(TAR_REG, SWD_AP, &dataRead);
	
	TEST_ASSERT_EQUAL(0x2BA01477, dataRead);
}