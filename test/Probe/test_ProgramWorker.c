#include "unity.h"
#include "Tlv.h"
#include "TlvEx.h"
#include "ProgramWorker.h"
#include "ErrorCode.h"
#include "CoreDebugEx.h"
#include "mock_IoOperations.h"
#include "mock_Uart.h"
#include "mock_CoreDebug.h"
#include "mock_FPBUnit.h"
#include "mock_DWTUnit.h"
#include "mock_stm32f4xx_hal_uart.h"
#include "mock_MemoryReadWrite.h"
#include "mock_SwdStub.h"
#include "mock_CodeStepping.h"
#include "mock_SystemTime.h"

void setUp(void)  {}

void tearDown(void) {}

void test_IsStubBusy_should_read_STUB_status_and_return_1(void)
{
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  TEST_ASSERT_EQUAL(1, IsStubBusy());
}

void test_requestStubErase_should_write_flashAddress_and_size_into_STUB_flahsAddress_and_dataSize(void)
{ 
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, NO_ERROR);     //Set flash Address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 2048, NO_ERROR);               //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, NO_ERROR);      //Set Stub Instruction
  
  requestStubErase(0x08000000, 2048);
}

void test_requestStubMassErase_should_write_bankSelect_into_STUB_banks(void)
{
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->banks, FLASH_BANK_2, NO_ERROR);            //Set flash banks
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_MASSERASE, NO_ERROR);    //Set Stub Instruction
  
  requestStubMassErase(FLASH_BANK_2);
}

void test_requestStubCopy_should_write_into_STUB_flashAddress_sramAddress_and_size_to_copy(void)
{ 
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20000000, NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 248, NO_ERROR);                  //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, NO_ERROR);         //Set Stub Instruction
  
  requestStubCopy(0x20000000, 0x08000000, 248); 
}

void test_writeTargetRegister_given_register_address_and_data(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t address = 0xABCDABCE, data = 0x12345678;
  
  writeCoreRegister_Expect(address, data);
  
	writeTargetRegister(session, address, data);
}

void test_readTargetRegister_given_register_address_should_read_the_given_register_address(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  readCoreRegister_ExpectAndReturn(0xBEEFBEEF,0x12345670);
 
  readTargetRegister(session, 0xBEEFBEEF);
}

void test_readAllTargetRegister_should_read_all_target_register()
{
  int i = 0, j = 0 ;
  
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  for(i = 0 ; i < 20 ; i ++)
    readCoreRegister_ExpectAndReturn(i,0x11223344);
  
  readCoreRegister_ExpectAndReturn(CORE_REG_FPSCR,0x11223344);
  
  for(j=64 ; j < 96 ; j++)
    readCoreRegister_ExpectAndReturn(j,0x11223344);
  
  readAllTargetRegister(session);
}

void test_writeTargetRam_should_write_data_to_specified_RAM_address(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t buffer[] = {0x20000000, 0x12345678, 0xABCDABCD};
  Tlv *tlv = tlvCreatePacket(TLV_WRITE_RAM, 12, (uint8_t *)buffer);
  
  memoryWriteWord_ExpectAndReturn(0x20000000, 0x12345678, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000004, 0xABCDABCD, NO_ERROR);
  
  writeTargetRam(session, &get4Byte(&tlv->value[4]), get4Byte(&tlv->value[0]), tlv->length - 5);
}

void test_readTargetRam_should_reply_back_with_the_correct_chksum()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t buffer[] = {0x20000000, 4};
  Tlv *tlv = tlvCreatePacket(TLV_WRITE_RAM, 8, (uint8_t *)buffer);
  
  memoryReadAndReturnWord_ExpectAndReturn(0x20000000, 0xABCDABCD);
  
  readTargetMemory(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));
  
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(9, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX32(0x20000000, get4Byte(&session->txBuffer[2]));
  TEST_ASSERT_EQUAL_HEX32(0xABCDABCD, get4Byte(&session->txBuffer[6]));
  TEST_ASSERT_EQUAL_HEX8(0xF0, session->txBuffer[10]); //chksum
}

void test_readTargetRam_should_read_data_from_specified_RAM_address()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t buffer[] = {0x20000000, 8};
  Tlv *tlv = tlvCreatePacket(TLV_WRITE_RAM, 8, (uint8_t *)buffer);
  
  memoryReadAndReturnWord_ExpectAndReturn(0x20000000, 0xDEADBEEF);
  memoryReadAndReturnWord_ExpectAndReturn(0x20000004, 0xABCDABCD);
  
  readTargetMemory(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));
}

void test_probeTaskManager_given_initial_state_receive_packet_when_packet_arrived_should_change_state(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  session->rxBuffer[0] = TLV_OK;
  session->rxBuffer[1] = 1;
  session->rxBuffer[2] = 0;
  
  probeTaskManager(session);
  
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_given_tlv_packet_with_invalid_data_should_send_tlv_error_code(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  session->rxBuffer[0] = 0xFF; //invalid command
  
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_should_receive_TLV_WRITE_REGISTER_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  session->rxBuffer[0] = TLV_WRITE_REGISTER; //invalid command
  session->rxBuffer[1] = 9;
  session->rxBuffer[2] = 0x01;
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x00;
  session->rxBuffer[6] = 0xDD;
  session->rxBuffer[7] = 0xCC;
  session->rxBuffer[8] = 0xBB;
  session->rxBuffer[9] = 0xAA;
  session->rxBuffer[10] = 0xF1; //chksum
  
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  
  writeCoreRegister_Expect(0x01, 0xAABBCCDD);
  probeTaskManager(session);
  
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_should_receive_TLV_READ_REGISTER_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;

  session->rxBuffer[0] = TLV_READ_REGISTER; //invalid command
  session->rxBuffer[1] = 5;
  session->rxBuffer[2] = 0x44;
  session->rxBuffer[3] = 0x33;
  session->rxBuffer[4] = 0x22;
  session->rxBuffer[5] = 0x11;
  session->rxBuffer[6] = 0x56; //chksum
  
  /*** Received Type ***/
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[0], 0); //data arrive
  
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(HOST_WAIT_USER_COMMAND, session->probeState);

  /*** Received Length ***/
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[1], 0); //data arrive
  /*** Received Value ***/
  getBytes_ExpectAndReturn(session->handler, &session->rxBuffer[2], 5, 0); //data arrive
  
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(HOST_WAIT_USER_COMMAND, session->probeState);
  
  uartRxReady = 1;
  tlvService(session);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  
  readCoreRegister_ExpectAndReturn(0x11223344,0x1);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_should_receive_TLV_READ_RAM_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;
  
  session->rxBuffer[0] = TLV_READ_MEMORY;
  session->rxBuffer[1] = 9;
  session->rxBuffer[2] = 0x00;
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x20;
  session->rxBuffer[6] = 12;
  session->rxBuffer[7] = 0x00;
  session->rxBuffer[8] = 0x00;
  session->rxBuffer[9] = 0x00;
  session->rxBuffer[10] = 0xD4; //chksum
  
  /* Type */
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[0], 0x00); //data arrive
  
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);

  /* Length */
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[1], 0x00); //data arrive
  /* Value */
  getBytes_ExpectAndReturn(session->handler, &session->rxBuffer[2], 9, 0x00); //data arrive
  
  tlvService(session);
  TEST_ASSERT_EQUAL(TLV_RECEIVE_VALUE, session->receiveState);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  
  uartRxReady = 1;
  tlvService(session);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[0], 0x01); //no data arrive

  tlvService(session);
  memoryReadAndReturnWord_ExpectAndReturn(0x20000000, 0xDEADBEEF);
  memoryReadAndReturnWord_ExpectAndReturn(0x20000004, 0xABCDABCD);
  memoryReadAndReturnWord_ExpectAndReturn(0x20000008, 0xABCDABCD);
  
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_should_receive_TLV_WRITE_RAM_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;
  
  session->rxBuffer[0] = TLV_WRITE_RAM;
  session->rxBuffer[1] = 9;
  session->rxBuffer[2] = 0x00;
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x20;
  session->rxBuffer[6] = 0x40;
  session->rxBuffer[7] = 0x30;
  session->rxBuffer[8] = 0x20;
  session->rxBuffer[9] = 0x10;
  session->rxBuffer[10] = 0x40; //chksum
  
  /*** Received Type ***/
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[0], 0x00); //data arrive
  
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  
  /*** Received Length ***/
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[1], 0x00); //data arrive
  /*** Received Value ***/
  getBytes_ExpectAndReturn(session->handler, &session->rxBuffer[2], 9, 0x00); //data arrive
  
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  
  uartRxReady = 1;
  tlvService(session);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  
  /*** Received Last Byte ***/
  getByte_ExpectAndReturn(session->handler, &session->rxBuffer[0], 0x01); //no data arrive
  tlvService(session);
  memoryWriteWord_ExpectAndReturn(0x20000000, 0x10203040, NO_ERROR);
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_should_run_checkPointEvent_if_set_breakPoint_is_called(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;
  
  session->rxBuffer[0] = TLV_BREAKPOINT;
  session->rxBuffer[1] = 5;
  session->rxBuffer[2] = 0x00;
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x20;
  session->rxBuffer[6] = 0xE0; //chksum
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  autoSetInstructionBreakpoint_ExpectAndReturn(0x20000000, INSTRUCTION_COMP0);
  probeTaskManager(session);
  
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  
  session->rxBuffer[0] = TLV_RUN_TARGET;
  session->rxBuffer[1] = 1;
  session->rxBuffer[2] = 0x00; //chksum
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  CLEAR_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG);
  
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));

  readDebugEventRegister_ExpectAndReturn(0x2);
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001110);
  memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB,0);
  memoryWriteWord_ExpectAndReturn(DFSR_REG,BKPT_DEBUGEVENT,0);
  
  probeTaskManager(session);    
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

/*=======================================================================================
                                    Debug Features
  =======================================================================================*/
  
  
/*--------------performSoftResetOnTarget--------------------*/
void test_performSoftResetOnTarget_should_call_softResetTarget_and_send_TLV_ack()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  memoryWriteWord_ExpectAndReturn(AIRCR_REG,REQUEST_SYSTEM_RESET,NO_ERROR);
  
  performSoftResetOnTarget(session);
}

/*--------------performHardResetOnTarget--------------------*/
void test_performHardResetOnTarget_should_call_hardResetTarget_and_send_TLV_ack()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  hardResetTarget_Expect();
  
  performHardResetOnTarget(session);
}


/*--------------performVectorResetOnTarget--------------------*/
void test_performVectotrResetOnTarget_should_call_vectorResetTarget_and_send_TLV_ack()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  memoryWriteWord_ExpectAndReturn(AIRCR_REG,REQUEST_VECTOR_RESET,NO_ERROR);
  
  performVectorResetOnTarget(session);
}

/*--------------haltTarget--------------------*/
void test_haltTarget_should_return_ACK_if_successful()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  setCoreMode_Expect(CORE_DEBUG_HALT);
  getCoreMode_ExpectAndReturn(CORE_DEBUG_HALT);
  
  haltTarget(session);
}

void test_haltTarget_should_return_NACK_and_ERR_NOT_HALTED_if_not_successful()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  Try {
    setCoreMode_Expect(CORE_DEBUG_HALT);
    getCoreMode_ExpectAndReturn(CORE_DEBUG_MODE);
    
    haltTarget(session);    
  } Catch(err) {
    TEST_ASSERT_EQUAL(TLV_NOT_HALTED, err);
  }
}

/*--------------runTarget--------------------*/
void test_runTarget_should_return_ACK_if_successful()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  stepIntoOnce_ExpectAndReturn(0);
  memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),ENABLE_FPB,0);
  setCoreMode_Expect(CORE_DEBUG_MODE);
  getCoreMode_ExpectAndReturn(CORE_DEBUG_MODE);
  
  runTarget(session);
}

void test_runTarget_should_return_NACK_and_ERR_NOT_RUNNING_if_unsuccessful()
{
  CEXCEPTION_T err;
  
  Try {
    uartInit_Ignore();
    Tlv_Session *session = tlvCreateSession();
    
    stepIntoOnce_ExpectAndReturn(0);
    memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),ENABLE_FPB,0);
    setCoreMode_Expect(CORE_DEBUG_MODE);
    getCoreMode_ExpectAndReturn(CORE_DEBUG_HALT);
    
    runTarget(session);      
  } Catch(err) {
    TEST_ASSERT_EQUAL(TLV_NOT_RUNNING, err);
  }
}

void test_runTarget_should_run_breakpointEventHandler_if_breakPointFlag_is_set()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  SET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG);
  
  readDebugEventRegister_ExpectAndReturn(0x2);
  
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08000000);
  memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB,0);
  memoryWriteWord_ExpectAndReturn(DFSR_REG,BKPT_DEBUGEVENT,0);
  
  runTarget(session);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
}

/*---------performSingleStepInto----------------------*/
void test_performSingleStepInto_should_readPC_step_and_return_PC_if_successful()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001108);
  stepIntoOnce_ExpectAndReturn(0x08001110);
  
  performSingleStepInto(session);    
}

void test_performSingleStepInto_should_readPC_step_and_Throw_TLV_NOT_STEPPED_if_unsuccessfu()
{
  CEXCEPTION_T err;
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  Try
  {
    readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001110);
    stepIntoOnce_ExpectAndReturn(0x08001110);
    performSingleStepInto(session);   
  }
  Catch(err)
  {
    
    TEST_ASSERT_EQUAL(TLV_NOT_STEPPED,err);
  }
}

/*---------performStepOver----------------------*/
void test_performStepOver_should_return_PC_after_successful_step()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001108);
  stepOver_ExpectAndReturn(0x08001110);
  
  performStepOver(session);
}

void test_performStepOver_should_throw_TLV_NOT_STEPOVER_if_fail()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  Try
  {
    readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001108);
    stepOver_ExpectAndReturn(0);
    performStepOver(session);
  }
  Catch(err)
  {
    TEST_ASSERT_EQUAL(TLV_NOT_STEPOVER,err);
  }
}

void test_performStepOver_should_throw_TLV_NOT_STEPOVER_if_fail_same_pc_case()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  Try
  {
    readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001108);
    stepOver_ExpectAndReturn(0x08001108);
    performStepOver(session);
  }
  Catch(err)
  {
    TEST_ASSERT_EQUAL(TLV_NOT_STEPOVER,err);
  }
}

/*---------performStepOut----------------------*/
void test_performStepOut_should_return_PC_after_successful_step()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  stepOut_ExpectAndReturn(0x08001110);
  
  performStepOut(session);
}

void test_performStepOut_should_throw_TLV_NOT_STEPOUT_if_fail()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  CEXCEPTION_T err;
  
  Try
  {
    stepOut_ExpectAndReturn(0);
    performStepOut(session);
  }
  Catch(err)
  {
    TEST_ASSERT_EQUAL(TLV_NOT_STEPOUT,err);
  }
}

/*---------setBreakpoint----------------------*/
void test_setBreakpoint_should_set_breakpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  autoSetInstructionBreakpoint_ExpectAndReturn(0x12345678,INSTRUCTION_COMP0);

  setBreakpoint(session,0x12345678);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));
}

void test_setBreakpoint_should_return_NACK_and_ERR_BKPT_MAXSET_if_all_comparators_are_in_use()
{
  CEXCEPTION_T err;
  
  Try {
    uartInit_Ignore();
    Tlv_Session *session = tlvCreateSession();
    
    autoSetInstructionBreakpoint_ExpectAndReturn(0x12345678,-1);
    setBreakpoint(session,0x12345678);
  } Catch(err) {
    TEST_ASSERT_EQUAL(TLV_BKPT_MAXSET, err);
  }
}

/*---------setWatchpoint----------------------*/
void test_setWatchpoint_should_set_watchpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  setDataWatchpoint_MatchingOneComparator_ExpectAndReturn(COMPARATOR_3,0x88884444,WATCHPOINT_MASK_BIT2_BIT0,0xAABB,WATCHPOINT_BYTE,WATCHPOINT_READ,0);
  
  setWatchpoint(session,0x88884444,WATCHPOINT_MASK_BIT2_BIT0,0xAABB,WATCHPOINT_BYTE,WATCHPOINT_READ);
}

/*---------removeHardwareBreakpoint----------------------*/
void test_removeHardwareBreakpoint_should_remove_breakpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x10203040,INSTRUCTION_TYPE,1);
  
  removeHardwareBreakpoint(session,0x10203040);
}

void test_removeHardwareBreakpoint_should_return_NACK_if_not_found()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x10203040,INSTRUCTION_TYPE,-1);
  
  removeHardwareBreakpoint(session,0x10203040);
}

/*---------removeSoftwareBreakpoint----------------------*/
void test_removeSoftwareBreakpoint_should_restore_the_address_with_original_machineCode()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  restoreSoftwareBreakpointOriginalInstruction_Expect(0x08001000,0xABCD);
  
  removeSoftwareBreakpoint(session,0x08001000,0xABCD);
}

/*---------removeAllHardwareBreakpoint----------------------*/
void test_removeAllHardwareBreakpoint_should_remove_all_breakpoint_and_return_ACK()
{  
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  removeAllFPComparatorSetToBreakpoint_Expect();
  
  removeAllHardwareBreakpoint(session);
}

/*---------stopFlashPatchRemapping----------------------*/
void test_stopFlashPatchRemapping_should_stop_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,INSTRUCTION_TYPE,1);
  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,LITERAL_TYPE,1);
  
  stopFlashPatchRemapping(session,0x12345678);
}

void test_stopFlashPatchRemapping_should_return_NACK_if_not_found()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,INSTRUCTION_TYPE,-1);
  disableFPComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,LITERAL_TYPE,-1);
  
  stopFlashPatchRemapping(session,0x12345678);
}

/*---------stopAllFlashPatchRemapping----------------------*/
void test_stopAllFlashPatchRemapping_should_stop_all_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  stopAllFPRemapping_Expect();
  
  stopAllFlashPatchRemapping(session);
}

/*---------breakpointEventHandler----------------------*/
void test_breakpointEventHandler_should_force_quit_if_breakpoint_not_occur()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  readDebugEventRegister_ExpectAndReturn(0);
  
  breakpointEventHandler(session);
}

void test_breakpointEventHandler_should_read_PC_and_disable_comparator_if_breakpoint_occur(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  readDebugEventRegister_ExpectAndReturn(0x2);
  
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08000000);
  memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB,0);
  memoryWriteWord_ExpectAndReturn(DFSR_REG,BKPT_DEBUGEVENT,0);
  
  breakpointEventHandler(session);
}

/*---------watchpointEventHandler----------------------*/
void test_watchpointEventHandler_should_force_quit_if_watchpoint_not_occur()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  hasDataWatchpointOccurred_ExpectAndReturn(0);
  
  watchpointEventHandler(session);
}

void test_watchpointEventHandler_should_read_PC_and_disable_comparator_if_watchpoint_occur()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  hasDataWatchpointOccurred_ExpectAndReturn(1);
  
  readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08000000);
  disableDWTComparator_ExpectAndReturn(COMPARATOR_1,0);
  memoryWriteWord_ExpectAndReturn(DFSR_REG,DWTTRAP_DEBUGEVENT,0);
  
  watchpointEventHandler(session);
}

void test_writeTargetFlash_should_write_into_target_ram_first_then_change_state(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t dataAddress[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};

  memoryWriteWord_ExpectAndReturn(0x20005000, 0x11111111, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005004, 0x22222222, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005008, 0x33333333, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x2000500C, 0x44444444, NO_ERROR);
  
  writeTargetFlash(session, dataAddress, 0x08001000, 16);
  
  TEST_ASSERT_EQUAL(COPY_TO_FLASH, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
}

void test_writeTargetFlash_should_copy_from_sram_to_flash_by_sending_request_copy(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t dataAddress[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  session->pFlashState = COPY_TO_FLASH;
  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  /* Request stub copy */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20005000, NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08001000, NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 16, NO_ERROR);                   //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, NO_ERROR);         //Set Stub Instruction
  
  writeTargetFlash(session, dataAddress, 0x08001000, 16);
  
  TEST_ASSERT_EQUAL(WRITE_TO_RAM, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_writeTargetFlash_should_copy_data_into_sram_and_request_stub_copy(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t dataAddress[] = {0xaaaaaaaa, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd};
  
  memoryWriteWord_ExpectAndReturn(0x20005000, 0xaaaaaaaa, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005004, 0xbbbbbbbb, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005008, 0xcccccccc, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x2000500C, 0xdddddddd, NO_ERROR);
  
  writeTargetFlash(session, dataAddress, 0x08001000, 16);
  
  TEST_ASSERT_EQUAL(COPY_TO_FLASH, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  
  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  /* Request stub copy */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20005000, NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08001000, NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 16, NO_ERROR);                   //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, NO_ERROR);         //Set Stub Instruction
  
  writeTargetFlash(session, dataAddress, 0x08001000, 16);
  
  TEST_ASSERT_EQUAL(WRITE_TO_RAM, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
}

void test_eraseTargetFlash_should_erase_target_flash_by_given_address_and_size(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, NO_ERROR);     //Set flash Address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 20000, NO_ERROR);               //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, NO_ERROR);      //Set Stub Instruction
  
  eraseTargetFlash(session, 0x08000000, 20000);
  
  TEST_ASSERT_EQUAL(WAIT_OPERATION_COMPLETE, session->pEraseState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  
  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  eraseTargetFlash(session, 0x08000000, 20000);
  
  TEST_ASSERT_EQUAL(REQUEST_ERASE, session->pEraseState);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
}

void test_writeTargetInWord_should_write_word_data_into_specified_address(void)
{ 
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t address = 0x12345678, data = 0xDEADDEAD;
  
  memoryWriteWord_ExpectAndReturn(address, data, NO_ERROR);     //Set flash Address
  
  writeTargetInWord(session, address, data);
  
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_writeTargetInHalfWord_should_write_word_data_into_specified_address(void)
{ 
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t address = 0x12345678, data = 0xDEAD;
  
  memoryWriteHalfword_ExpectAndReturn(address, data, NO_ERROR);     //Set flash Address
  
  writeTargetInHalfword(session, address, data);
  
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_writeTargetInByte_should_write_word_data_into_specified_address(void)
{ 
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  uint32_t address = 0xABCD1234, data = 0xAA;
  
  memoryWriteByte_ExpectAndReturn(address, data, NO_ERROR);     //Set flash Address
  
  writeTargetInByte(session, address, data);
  
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_checkDebugEvent_should_return_BREAKPOINT_EVENT_if_event_occur(void)
{ 
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  readDebugEventRegister_ExpectAndReturn(0x2);
  checkDebugEvent(session, BREAKPOINT_EVENT);
  
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(2, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX8(BREAKPOINT_EVENT, session->txBuffer[2]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, session->txBuffer[3]); //chksum
}

void test_checkDebugEvent_should_return_WATCHPOINT_EVENT_if_event_occur(void)
{ 
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  hasDataWatchpointOccurred_ExpectAndReturn(1);
  checkDebugEvent(session, WATCHPOINT_EVENT);
  
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(2, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX8(WATCHPOINT_EVENT, session->txBuffer[2]);
  TEST_ASSERT_EQUAL_HEX8(0xFE, session->txBuffer[3]); //chksum
}

void test_probeTaskManager_given_flash_command_should_run_writeTargetFlash(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;
  
  session->rxBuffer[0] = TLV_WRITE_FLASH; //invalid command
  session->rxBuffer[1] = 13;
  
  session->rxBuffer[2] = 0x00; //address
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x08;
  
  session->rxBuffer[6] = 0x44; //data
  session->rxBuffer[7] = 0x33; 
  session->rxBuffer[8] = 0x22; 
  session->rxBuffer[9] = 0x11; 
  
  session->rxBuffer[10] = 0x88; //data
  session->rxBuffer[11] = 0x77; 
  session->rxBuffer[12] = 0x66; 
  session->rxBuffer[13] = 0x55;
  
  session->rxBuffer[14] = 0x94; //chksum
  
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);

  /* Received packet */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  
  /* Mocking write into ram */
  memoryWriteWord_ExpectAndReturn(0x20005000, 0x11223344, NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005004, 0x55667788, NO_ERROR);
  
  /* Intepret packet and goes to writeTargetFlash() */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(COPY_TO_FLASH, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  
  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  /* Request stub copy */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20005000, NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 8, NO_ERROR);                    //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, NO_ERROR);         //Set Stub Instruction
  
  /* Intepret packet and goes to writeTargetFlash() */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(WRITE_TO_RAM, session->pFlashState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_probeTaskManager_given_flash_erase_command_should_run_eraseFlashTarget(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  session->rxBuffer[0] = TLV_FLASH_ERASE; // command
  session->rxBuffer[1] = 9;
  
  session->rxBuffer[2] = 0x00; //address
  session->rxBuffer[3] = 0x00;
  session->rxBuffer[4] = 0x00;
  session->rxBuffer[5] = 0x08;
  
  session->rxBuffer[6] = 0x20; //20000
  session->rxBuffer[7] = 0x4E;
  session->rxBuffer[8] = 0x00;
  session->rxBuffer[9] = 0x00;
  session->rxBuffer[10] = 0x8A; //chksum
  
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  
  /* Received packet */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  
  /* Mocking Request Erase */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, NO_ERROR);
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, (int)20000, NO_ERROR);
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, NO_ERROR);
  
  /* Intepret packet and goes to eraseTargetFlash() */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(WAIT_OPERATION_COMPLETE, session->pEraseState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  
  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  
  /* Intepret packet and goes to writeTargetFlash() */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  TEST_ASSERT_EQUAL(REQUEST_ERASE, session->pEraseState);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_probeTaskManager_given_TLV_DEBUG_EVENTS_should_call_checkDebugEvent(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  
  session->rxBuffer[0] = TLV_DEBUG_EVENTS; // command
  session->rxBuffer[1] = 2; //length
  session->rxBuffer[2] = BREAKPOINT_EVENT; //specific event
  session->rxBuffer[3] = 0xFF; //chksum
  
  /* Received packet */
  probeTaskManager(session);
  TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  
  /* Intepret packet and goes to eraseTargetFlash() */
  readDebugEventRegister_ExpectAndReturn(0x2);
  probeTaskManager(session);
  
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(2, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX8(BREAKPOINT_EVENT, session->txBuffer[2]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, session->txBuffer[3]); //chksum
}