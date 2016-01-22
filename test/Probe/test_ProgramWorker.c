#include "unity.h"
#include <string.h>
#include "Tlv.h"
#include "TlvEx.h"
#include "Yield.h"
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
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, SWD_NO_ERROR);     //Set flash Address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 2048, SWD_NO_ERROR);               //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, SWD_NO_ERROR);      //Set Stub Instruction

  requestStubErase(0x08000000, 2048);
}

void test_requestStubMassErase_should_write_bankSelect_into_STUB_banks(void)
{
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->banks, FLASH_BANK_2, SWD_NO_ERROR);            //Set flash banks
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_MASSERASE, SWD_NO_ERROR);    //Set Stub Instruction

  requestStubMassErase(FLASH_BANK_2);
}

void test_requestStubCopy_should_write_into_STUB_flashAddress_sramAddress_and_size_to_copy(void)
{
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20000000, SWD_NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, SWD_NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 248, SWD_NO_ERROR);                  //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, SWD_NO_ERROR);         //Set Stub Instruction

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

void test_readTargetRam_should_reply_back_with_the_correct_chksum()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  uint32_t buffer[] = {0x20000000, 4};
  Tlv *tlv = tlvCreatePacket(TLV_WRITE_RAM, 8, (uint8_t *)buffer);

  // 0xABCDABCD
  memoryReadAndReturnByte_ExpectAndReturn(0x20000000, 0xCD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000001, 0xAB);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000002, 0xCD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000003, 0xAB);

  readTargetMemory(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));

  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(5, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX32(0xABCDABCD, get4Byte(&session->txBuffer[2]));
  TEST_ASSERT_EQUAL_HEX8(0x44, session->txBuffer[10]); //chksum
}

void test_readTargetRam_should_only_read_2_byte()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  uint32_t buffer[] = {0x20000004, 2};
  Tlv *tlv = tlvCreatePacket(TLV_WRITE_RAM, 8, (uint8_t *)buffer);

  // 0xDEAD
  memoryReadAndReturnByte_ExpectAndReturn(0x20000004, 0xAD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000005, 0xDE);

  readTargetMemory(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));

  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(3, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX16(0xDEAD, get4Byte(&session->txBuffer[2]));
  TEST_ASSERT_EQUAL_HEX8(0x22, session->txBuffer[8]); //chksum
}
/*=======================================================================================
                                    Debug Features
  =======================================================================================*/


/*--------------performSoftResetOnTarget--------------------*/
void test_performSoftResetOnTarget_should_call_softResetTarget_and_send_TLV_ack()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  memoryWriteWord_ExpectAndReturn(AIRCR_REG,REQUEST_SYSTEM_RESET,SWD_NO_ERROR);

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

  setCoreMode_Expect(CORE_DEBUG_HALT);
  memoryWriteWord_ExpectAndReturn(AIRCR_REG,REQUEST_VECTOR_RESET,SWD_NO_ERROR);

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
// void test_runTarget_should_return_ACK_if_successful()
// {
  // uartInit_Ignore();
  // Tlv_Session *session = tlvCreateSession();

  // stepIntoOnce_ExpectAndReturn(0);
  // memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),ENABLE_FPB,0);
  // setCoreMode_Expect(CORE_DEBUG_MODE);
  // getCoreMode_ExpectAndReturn(CORE_DEBUG_MODE);

  // runTarget(session);
// }

// void test_runTarget_should_return_NACK_and_ERR_NOT_RUNNING_if_unsuccessful()
// {
  // CEXCEPTION_T err;

  // Try {
    // uartInit_Ignore();
    // Tlv_Session *session = tlvCreateSession();

    // stepIntoOnce_ExpectAndReturn(0);
    // memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),ENABLE_FPB,0);
    // setCoreMode_Expect(CORE_DEBUG_MODE);
    // getCoreMode_ExpectAndReturn(CORE_DEBUG_HALT);

    // runTarget(session);
  // } Catch(err) {
    // TEST_ASSERT_EQUAL(TLV_NOT_RUNNING, err);
  // }
// }

// void test_runTarget_should_run_breakpointEventHandler_if_breakPointFlag_is_set()
// {
  // uartInit_Ignore();
  // Tlv_Session *session = tlvCreateSession();

  // SET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG);

  // readDebugEventRegister_ExpectAndReturn(0x2);

  // readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08000000);
  // memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB,0);
  // memoryWriteWord_ExpectAndReturn(DFSR_REG,BKPT_DEBUGEVENT,0);

  // runTarget(session);
  // TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));
  // TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
// }

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

/*---------setInstructionRemapping----------------------*/
void test_setInstructionRemapping_should_Throw_TLV_REMAP_MAXSET_if_all_comparators_are_in_use()
{
  CEXCEPTION_T err;

  Try {
    uartInit_Ignore();
    Tlv_Session *session = tlvCreateSession();

    autoSetInstructionRemapping_ExpectAndReturn(0x12345678,0xAABB,-1);
    setInstructionRemapping(session,0x12345678,0xAABB);
  } Catch(err) {
    TEST_ASSERT_EQUAL(TLV_REMAP_MAXSET, err);
  }
}

void test_setInstructionRemapping_set_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  autoSetInstructionRemapping_ExpectAndReturn(0x12345678,0xAABB,INSTRUCTION_COMP5);
  setInstructionRemapping(session,0x12345678,0xAABB);
}

/*---------setLiteralRemapping----------------------*/
void test_setLiteralRemapping_should_Throw_TLV_REMAP_MAXSET_if_all_comparators_are_in_use()
{
  
  CEXCEPTION_T err;

  Try {
    uartInit_Ignore();
    Tlv_Session *session = tlvCreateSession();

    autoSetLiteralRemapping_ExpectAndReturn(0x12345678,0xAABB,-1);
    setLiteralRemapping(session,0x12345678,0xAABB);
  } Catch(err) {
    TEST_ASSERT_EQUAL(TLV_REMAP_MAXSET, err);
  }
  
}

void test_setLiteralRemapping_set_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  autoSetLiteralRemapping_ExpectAndReturn(0x12345678,0xAABB,LITERAL_COMP0);
  setLiteralRemapping(session,0x12345678,0xAABB);
}


/*---------removeHardwareBreakpoint----------------------*/
void test_removeHardwareBreakpoint_should_remove_breakpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x10203040,INSTRUCTION_TYPE,1);

  removeHardwareBreakpoint(session,0x10203040);
}

void test_removeHardwareBreakpoint_should_throw_TLV_ADDRESS_NOT_FOUND_if_address_not_found()
{
  CEXCEPTION_T err ;
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  Try
  {
    disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x10203040,INSTRUCTION_TYPE,-1);
    removeHardwareBreakpoint(session,0x10203040);
  }Catch(err)
  {
    TEST_ASSERT_EQUAL(TLV_ADDRESS_NOT_FOUND,err);
  }


}

/*---------removeAllHardwareBreakpoint----------------------*/
void test_removeAllHardwareBreakpoint_should_remove_all_breakpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableAllFlashPatchComparatorSetToBreakpoint_Expect();

  removeAllHardwareBreakpoint(session);
}

/*---------removeWatchpoint----------------------*/
void test_removeWatchpoint_should_remove_watchpoint_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  
  disableDWTComparator_ExpectAndReturn(COMPARATOR_1,0);
  
  removeWatchpoint(session);
}


/*---------stopFlashPatchRemapping----------------------*/
void test_stopFlashPatchRemapping_should_stop_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,INSTRUCTION_TYPE,1);
  disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,LITERAL_TYPE,1);

  stopFlashPatchRemapping(session,0x12345678);
}

void test_stopFlashPatchRemapping_should_throw_TLV_ADDRESS_NOT_FOUND_if_address_not_found()
{
  CEXCEPTION_T err ;
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  Try
  {
    disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,INSTRUCTION_TYPE,-1);
    disableFlashPatchComparatorLoadedWithAddress_ExpectAndReturn(0x12345678,LITERAL_TYPE,-1);

    stopFlashPatchRemapping(session,0x12345678);
  }Catch(err)
  {
    TEST_ASSERT_EQUAL(TLV_ADDRESS_NOT_FOUND,err);
  }
}

/*---------stopAllFlashPatchRemapping----------------------*/
void test_stopAllFlashPatchRemapping_should_stop_all_remapping_and_return_ACK()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  disableAllFlashPatchComparatorSetToRemap_Expect();

  disableAllFlashPatchComparatorSetToRemap(session);
}

/*---------breakpointEventHandler----------------------*/
void test_breakpointEventHandler_should_force_quit_if_breakpoint_not_occur()
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  readDebugEventRegister_ExpectAndReturn(0);

  breakpointEventHandler(session);
}

void test_breakpointEventHandler_should_read_PC_and_disable_FPBUnit_and_clear_bkptEvent_if_breakpoint_occur(void)
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

void test_writeTargetFlash_should_write_into_target_ram_first_then_wait_for_stub_to_ready(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  uint32_t dataAddress[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};

  memoryWriteWord_ExpectAndReturn(0x20005000, 0x11111111, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005004, 0x22222222, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005008, 0x33333333, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x2000500C, 0x44444444, SWD_NO_ERROR);

  /* Stub status is BUSY */
  getSystemTime_ExpectAndReturn(10);
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_BUSY);
  isTimeout_ExpectAndReturn(FIVE_SECOND, 10, 0);

  writeTargetFlash(session, (uint8_t *)dataAddress, 0x08001000, 16);
  TEST_ASSERT_EQUAL(1, isYielding);

  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);

  /* Request stub copy */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20005000, SWD_NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08001000, SWD_NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 16, SWD_NO_ERROR);                    //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, SWD_NO_ERROR);         //Set Stub Instruction

  writeTargetFlash(session, (uint8_t *)dataAddress, 0x08001000, 16);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void test_writeTargetFlash_should_throw_if_stub_is_timeout(void)
{
  CEXCEPTION_T err;
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  uint32_t dataAddress[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};

  Try {
    memoryWriteWord_ExpectAndReturn(0x20005000, 0x11111111, SWD_NO_ERROR);
    memoryWriteWord_ExpectAndReturn(0x20005004, 0x22222222, SWD_NO_ERROR);
    memoryWriteWord_ExpectAndReturn(0x20005008, 0x33333333, SWD_NO_ERROR);
    memoryWriteWord_ExpectAndReturn(0x2000500C, 0x44444444, SWD_NO_ERROR);

    /* Stub status is BUSY */
    getSystemTime_ExpectAndReturn(10);
    memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_BUSY);
    isTimeout_ExpectAndReturn(FIVE_SECOND, 10, 0);

    writeTargetFlash(session, (uint8_t *)dataAddress, 0x08001000, 16);
    TEST_ASSERT_EQUAL(1, isYielding);

    /* Stub status is OK */
    memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_BUSY);
    isTimeout_ExpectAndReturn(FIVE_SECOND, 10, 1);

    writeTargetFlash(session, (uint8_t *)dataAddress, 0x08001000, 16);
    printf("Should Throw PROBE_STUB_NOT_RESPONDING\n");
  }
  Catch(err) {
    TEST_ASSERT_EQUAL(PROBE_STUB_NOT_RESPONDING, err);
    TEST_ASSERT_EQUAL(0, isYielding);
    TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
    TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
    TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
    TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
  }
}

void test_eraseTargetFlash_should_request_erase_if_stub_is_ready(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  /* Stub status is OK */
  getSystemTime_ExpectAndReturn(10);
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_BUSY);
  isTimeout_ExpectAndReturn(FIVE_SECOND, 10, 0);

  eraseTargetFlash(session, 0x08000000, 20000);

  TEST_ASSERT_EQUAL(1, isYielding);

  /* Stub status is OK */
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);

  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, SWD_NO_ERROR);     //Set flash Address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 20000, SWD_NO_ERROR);              //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, SWD_NO_ERROR);      //Set Stub Instruction

  eraseTargetFlash(session, 0x08000000, 20000);

  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(0, isYielding);
}

void test_massEraseTargetFlash_should_request_erase_if_stub_is_ready(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  /* Stub status is OK */
  getSystemTime_ExpectAndReturn(10);
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);

  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->banks, FLASH_BANK_1, SWD_NO_ERROR);            //Set flash banks
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_MASSERASE, SWD_NO_ERROR);    //Set Stub Instruction

  massEraseTargetFlash(session, FLASH_BANK_1);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_isDebugEventOccur_should_return_BREAKPOINT_EVENT_if_event_occur(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  readDebugEventRegister_ExpectAndReturn(0x2);

  TEST_ASSERT_EQUAL(BREAKPOINT_EVENT, isDebugEventOccur(BREAKPOINT_EVENT));
}

void test_isDebugEventOccur_should_return_WATCHPOINT_EVENT_if_event_occur(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  hasDataWatchpointOccurred_ExpectAndReturn(1);
  TEST_ASSERT_EQUAL(WATCHPOINT_EVENT, isDebugEventOccur(WATCHPOINT_EVENT));
}

void test_debugEventHandler_should_reply_when_event_occured(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  getSystemTime_ExpectAndReturn(10);
  readDebugEventRegister_ExpectAndReturn(0xabc);
  isTimeout_ExpectAndReturn(TWO_SECOND, 10, 0);
  debugEventHandler(session, BREAKPOINT_EVENT);

  readDebugEventRegister_ExpectAndReturn(0x2);
  debugEventHandler(session, BREAKPOINT_EVENT);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(2, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(BREAKPOINT_EVENT, session->txBuffer[2]);
}

void test_taskManager_given_tlv_packet_with_invalid_data_should_send_tlv_error_code(void)
{
  CEXCEPTION_T err;

  Try {
    uartInit_Ignore();
    Tlv_Session *session = tlvCreateSession();

    SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
    session->rxBuffer[0] = 0xFF; //invalid command

    taskManager(session);

    printf("should throw an error\n");
  }
  Catch(err) {
    TEST_ASSERT_EQUAL(TLV_INVALID_COMMAND, err);
  }
}

void test_taskManager_should_receive_TLV_WRITE_REGISTER_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  session->rxBuffer[0] = TLV_WRITE_REGISTER; //command
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

  writeCoreRegister_Expect(0x01, 0xAABBCCDD);
  taskManager(session);

  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
}

void test_taskManager_should_receive_TLV_READ_REGISTER_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();
  uint32_t readData = 0;

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  session->rxBuffer[0] = TLV_READ_REGISTER; //invalid command
  session->rxBuffer[1] = 5;
  session->rxBuffer[2] = 0x44;
  session->rxBuffer[3] = 0x33;
  session->rxBuffer[4] = 0x22;
  session->rxBuffer[5] = 0x11;
  session->rxBuffer[6] = 0x56; //chksum

  readCoreRegister_ExpectAndReturn(0x11223344, 0xDEADBEEF);

  taskManager(session);

  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(5, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0xDEADBEEF, get4Byte(&session->txBuffer[2]));
}

void test_taskManager_should_receive_TLV_READ_MEMORY_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
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

  // 0xDEADBEEF
  memoryReadAndReturnByte_ExpectAndReturn(0x20000000, 0xEF);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000001, 0xBE);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000002, 0xAD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000003, 0xDE);

  // 0xABCDABCD
  memoryReadAndReturnByte_ExpectAndReturn(0x20000004, 0xCD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000005, 0xAB);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000006, 0xCD);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000007, 0xAB);

  // 0x12345678
  memoryReadAndReturnByte_ExpectAndReturn(0x20000008, 0x78);
  memoryReadAndReturnByte_ExpectAndReturn(0x20000009, 0x56);
  memoryReadAndReturnByte_ExpectAndReturn(0x2000000A, 0x34);
  memoryReadAndReturnByte_ExpectAndReturn(0x2000000B, 0x12);

  taskManager(session);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(13, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, get4Byte(&session->txBuffer[2]));
  TEST_ASSERT_EQUAL_HEX32(0xABCDABCD, get4Byte(&session->txBuffer[6]));
  TEST_ASSERT_EQUAL_HEX32(0x12345678, get4Byte(&session->txBuffer[10]));
}

void test_taskManager_should_receive_TLV_WRITE_RAM_and_perform_the_task(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);

  uint8_t buffer[] = { TLV_WRITE_RAM, 25, 0x02, 0x00, 0x00, 0x20, //22
                      //deadbeef
                      0xef, 0xbe, 0xad, 0xde, //338
                      //0xaabbccdd
                      0xdd, 0xcc, 0xbb, 0xaa, //30e
                      //0x12345678
                      0x78, 0x56, 0x34, 0x12, //114
                      //0xfacebeef
                      0xef, 0xbe, 0xce, 0xfa, //375
                      //0xbeefbeef
                      0xef, 0xbe, 0xef, 0xbe, //35a
                      0xB5
                   };

  memcpy(session->rxBuffer, buffer, sizeof(buffer));
  memoryWriteHalfword_ExpectAndReturn(0x20000002, 0xbeef, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000004, 0xccdddead, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000008, 0x5678aabb, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x2000000C, 0xbeef1234, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000010, 0xbeefface, SWD_NO_ERROR);
  memoryWriteHalfword_ExpectAndReturn(0x20000014, 0xbeef, SWD_NO_ERROR);

  taskManager(session);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
}

// void test_taskManager_should_run_checkPointEvent_if_set_breakPoint_is_called(void)
// {
  // uartInit_Ignore();
  // Tlv_Session *session = tlvCreateSession();
  // uint32_t readData = 0;

  // session->rxBuffer[0] = TLV_BREAKPOINT;
  // session->rxBuffer[1] = 5;
  // session->rxBuffer[2] = 0x00;
  // session->rxBuffer[3] = 0x00;
  // session->rxBuffer[4] = 0x00;
  // session->rxBuffer[5] = 0x20;
  // session->rxBuffer[6] = 0xE0; //chksum
  // SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);

  // taskManager(session);
  // TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  // TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  // autoSetInstructionBreakpoint_ExpectAndReturn(0x20000000, INSTRUCTION_COMP0);
  // taskManager(session);

  // TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  // TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));
  // TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  // TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));

  // session->rxBuffer[0] = TLV_RUN_TARGET;
  // session->rxBuffer[1] = 1;
  // session->rxBuffer[2] = 0x00; //chksum
  // SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
  // CLEAR_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG);

  // taskManager(session);
  // TEST_ASSERT_EQUAL(PROBE_INTERPRET_PACKET, session->probeState);
  // TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG));

  // readDebugEventRegister_ExpectAndReturn(0x2);
  // readCoreRegister_ExpectAndReturn(CORE_REG_PC,0x08001110);
  // memoryWriteWord_ExpectAndReturn((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB,0);
  // memoryWriteWord_ExpectAndReturn(DFSR_REG,BKPT_DEBUGEVENT,0);

  // taskManager(session);
  // TEST_ASSERT_EQUAL(PROBE_RECEIVE_PACKET, session->probeState);
  // TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG));
  // TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
// }

void xtest_taskManager_given_flash_command_should_run_writeTargetFlash(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);
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

  /* Mocking write into ram */
  memoryWriteWord_ExpectAndReturn(0x20005000, 0x11223344, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20005004, 0x55667788, SWD_NO_ERROR);

  /* Stub status is OK */
  getSystemTime_ExpectAndReturn(10);
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);
  /* Request stub copy */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->sramAddress, 0x20005000, SWD_NO_ERROR);        //Set sram address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, SWD_NO_ERROR);       //Set flash address
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, 8, SWD_NO_ERROR);                    //Set data size
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_COPY, SWD_NO_ERROR);         //Set Stub Instruction

  /* Received packet */
  taskManager(session);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void xtest_taskManager_given_flash_erase_command_should_run_eraseFlashTarget(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);

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

  /* Stub status is OK */
  getSystemTime_ExpectAndReturn(10);
  memoryReadAndReturnWord_ExpectAndReturn((uint32_t)&STUB->status, STUB_OK);

  /* Mocking Request Erase */
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->flashAddress, 0x08000000, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->dataSize, (int)20000, SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn((uint32_t)&STUB->instruction, STUB_ERASE, SWD_NO_ERROR);

  /* Received packet */
  taskManager(session);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(1, session->txBuffer[1]);
  TEST_ASSERT_EQUAL(0, session->txBuffer[2]);
}

void xtest_taskManager_given_TLV_DEBUG_EVENTS_should_call_checkDebugEvent(void)
{
  uartInit_Ignore();
  Tlv_Session *session = tlvCreateSession();

  SET_FLAG_STATUS(session, TLV_DATA_RECEIVE_FLAG);

  session->rxBuffer[0] = TLV_DEBUG_EVENTS; // command
  session->rxBuffer[1] = 2; //length
  session->rxBuffer[2] = BREAKPOINT_EVENT; //specific event
  session->rxBuffer[3] = 0xFF; //chksum

  getSystemTime_ExpectAndReturn(10);
  readDebugEventRegister_ExpectAndReturn(0x2);

  /* Received packet */
  taskManager(session);

  TEST_ASSERT_EQUAL(0, isYielding);
  TEST_ASSERT_EQUAL(FLAG_CLEAR, GET_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG));
  TEST_ASSERT_EQUAL(FLAG_SET, GET_FLAG_STATUS(session, TLV_DATA_TRANSMIT_FLAG));
  TEST_ASSERT_EQUAL(TLV_OK, session->txBuffer[0]);
  TEST_ASSERT_EQUAL(2, session->txBuffer[1]);
  TEST_ASSERT_EQUAL_HEX8(BREAKPOINT_EVENT, session->txBuffer[2]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, session->txBuffer[3]); //chksum
}

/*---------selectAppropriateMethodToWriteRAM----------------------*/
void test_selectAppropriateMethodToWriteRAM_given_0xE0000000_size_10_should_write_4bytes_4bytes_2bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteWord_ExpectAndReturn(0xE0000000,0x44332211,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000004,0x88776655,SWD_NO_ERROR);
  memoryWriteHalfword_ExpectAndReturn(0xE0000008,0xAA99,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0xE0000000, 10);
}

void test_selectAppropriateMethodToWriteRAM_given_0xE0000001_size_10_should_write_2bytes_1bytes_4bytes_2bytes_1bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteHalfword_ExpectAndReturn(0xE0000001,0x2211,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0xE0000003,0x33,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000004,0x77665544,SWD_NO_ERROR);
  memoryWriteHalfword_ExpectAndReturn(0xE0000008,0x9988,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0xE000000A,0xAA,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0xE0000001, 10);
}

void test_selectAppropriateMethodToWriteRAM_given_0xE0000002_size_10_should_write_2bytes_4bytes_4bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteHalfword_ExpectAndReturn(0xE0000002,0x2211,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000004,0x66554433,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000008,0xAA998877,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0xE0000002, 10);
}

void test_selectAppropriateMethodToWriteRAM_given_0xE0000003_size_10_should_write_1bytes_4bytes_4bytes_1bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteByte_ExpectAndReturn(0xE0000003,0x11,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000004,0x55443322,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0xE0000008,0x99887766,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0xE000000C,0xAA,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0xE0000003, 10);
}

void test_selectAppropriateMethodToWriteRAM_given_0x20000004_size_9_should_write_4bytes_4bytes_1bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteWord_ExpectAndReturn(0x20000004,0x44332211,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000008,0x88776655,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0x2000000C,0x99,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0x20000004, 9);
}

void test_selectAppropriateMethodToWriteRAM_given_0x20000005_size_9_should_write_2bytes_1bytes_4bytes_2bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteHalfword_ExpectAndReturn(0x20000005,0x2211,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0x20000007,0x33,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000008,0x77665544,SWD_NO_ERROR);
  memoryWriteHalfword_ExpectAndReturn(0x2000000C,0x9988,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0x20000005, 9);
}

void test_selectAppropriateMethodToWriteRAM_given_0x20000006_size_9_should_write_2bytes_4bytes_2bytes_1bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteHalfword_ExpectAndReturn(0x20000006,0x2211,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000008,0x66554433,SWD_NO_ERROR);
  memoryWriteHalfword_ExpectAndReturn(0x2000000C,0x8877,SWD_NO_ERROR);
  memoryWriteByte_ExpectAndReturn(0x2000000E,0x99,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0x20000006, 9);
}

void test_selectAppropriateMethodToWriteRAM_given_0x20000007_size_9_should_write_1bytes_4bytes_4bytes()
{
  uint8_t buffer[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};

  memoryWriteByte_ExpectAndReturn(0x20000007,0x11,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x20000008,0x55443322,SWD_NO_ERROR);
  memoryWriteWord_ExpectAndReturn(0x2000000C,0x99887766,SWD_NO_ERROR);

  selectAppropriateMethodToWriteRAM(buffer, 0x20000007, 9);
}