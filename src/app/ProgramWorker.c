#include "ProgramWorker.h"

static CoreMode previousMode ;
static int isHandled = 0;

/* temp SRAM address 0x20005000 */
static uint32_t tempAddress = 0x20005000;
static Tlv *packet;

/** IsStubBusy is a function to check if stub
  * is busy with the last operation
  *
  * input   : NONE
  *
  * return  : 1 stub is free for next operation
  *           0 stub is busy with the last operation
  */
int IsStubBusy(void)  {
  unsigned int stubStatus = memoryReadAndReturnWord((uint32_t)&STUB->status);

  if(stubStatus == STUB_OK) {
    return 1;
  }

  else {
    return 0;
  }
}

/** requestStubErase is a function to load the sector erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : startAddress is the address to begin erase
  *           endAddress is the address to end erase
  *
  * output  : NONE
  */
void requestStubErase(uint32_t address, int size) {

  /* load flash start and end address to sram */
  memoryWriteWord((uint32_t)&STUB->flashAddress, (uint32_t)address);
  memoryWriteWord((uint32_t)&STUB->dataSize, (uint32_t)size);

  /* load instruction to sram */
  memoryWriteWord((uint32_t)&STUB->instruction, STUB_ERASE);
}

/** requestMassErase is a function to load the mass erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : bankSelect can be one of the following value
  *            + FLASH_BANK_1: Bank1 to be erased
  *            + FLASH_BANK_2: Bank2 to be erased
  *            + FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  *
  * output  : NONE
  */
void requestStubMassErase(uint32_t bankSelect)  {

  /* load bank select to sram */
  memoryWriteWord((uint32_t)&STUB->banks, (uint32_t)bankSelect);

  /* load instruction to sram */
  memoryWriteWord((uint32_t)&STUB->instruction, STUB_MASSERASE);
}

/** requestStubCopy is a function copy data from src (SRAM) to dest (Flash)
  *
  * input   : src is the beginning SRAM address contain all the information
  *           dest is the flash address all the information need to copy over there
  *           length is to determine how many words need to copy over
  *
  * output  : NONE
  */
void requestStubCopy(uint32_t dataAddress, uint32_t destAddress, int size) {

  /* load SRAM start address into sram */
  memoryWriteWord((uint32_t)&STUB->sramAddress, (uint32_t)dataAddress);

  /* load Flash start address into sram */
  memoryWriteWord((uint32_t)&STUB->flashAddress, (uint32_t)destAddress);

  /* load length into sram */
  memoryWriteWord((uint32_t)&STUB->dataSize, (uint32_t)size);

	/* load copy instructoin into sram */
  memoryWriteWord((uint32_t)&STUB->instruction, STUB_COPY);
}

/** writeTargetRegister is a function to write value into target register using swd
  *
  * input     : session contain a element/handler used by tlv protocol
  *             registerAddress is the address of the target register
  *             data is the data need to write into the target register
  *
  * return    : NONE
  */
int writeTargetRegister(Tlv_Session *session, uint32_t registerAddress, uint32_t data) {

  writeCoreRegister(registerAddress, data);
  tlvReply(session, TLV_OK, 0, NULL);

  returnThis(1);
}

/** readTargetRegister is a function to read value from target register using swd
  *
  * input     : session contain a element/handler used by tlv protocol
  *             registerAddress is the address of the target register
  *
  * return    : NONE
  */
int readTargetRegister(Tlv_Session *session, uint32_t registerAddress) {
  uint32_t data = 0;

  data = readCoreRegister(registerAddress);
  tlvReply(session, TLV_OK, 4, (uint8_t *)&data);

  returnThis(1);
}

/** readAllTargetRegister is a function to read value from all target register using swd
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void readAllTargetRegister(Tlv_Session *session)
{
  int i = 0 , j = 0;
  uint32_t data[52] = {} ;

  for (i = 0 ; i < 20 ; i ++)
    data[i] = readCoreRegister(i);

  data[i] = readCoreRegister(CORE_REG_FPSCR);

  for(j = 64 ; j < 96 ; j++)
  {
    i++ ;
    data[i] = readCoreRegister(j);
  }

  Tlv *tlv = tlvCreatePacket(TLV_OK, (4*52), (uint8_t *)&data);

  tlvSend(session,tlv);
}

/** Perform a soft reset on the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int performSoftResetOnTarget(Tlv_Session *session)
{
  softResetTarget();
  tlvReply(session, TLV_OK, 0, NULL);

  returnThis(1);
}

/** Perform a hard reset on the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int performHardResetOnTarget(Tlv_Session *session)
{
  hardResetTarget();
  tlvReply(session, TLV_OK, 0, NULL);

  returnThis(1);
}

/** Perform a hard reset on the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int performVectorResetOnTarget(Tlv_Session *session)
{
  vectorResetTarget();
  tlvReply(session, TLV_OK, 0, NULL);

  returnThis(1);
}

/** Halt the processor of the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int haltTarget(Tlv_Session *session)
{
  setCoreMode(CORE_DEBUG_HALT);

  if(getCoreMode() == CORE_DEBUG_HALT)
    tlvReply(session, TLV_OK, 0, NULL);

  else Throw(TLV_NOT_HALTED);

  returnThis(1);
}

/** Run the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int runTarget(Tlv_Session *session)
{

	  Tlv *tlv ;
	  uint32_t pc =0 , empty = 0 ;
	  setCoreMode(CORE_DEBUG_MODE);

	  if(previousMode == CORE_DEBUG_HALT)
	  {
		  disableFPBUnit();
		  stepOnly(1);
		  enableFPBUnit();
		  isHandled = 1 ;
	  }

	  if(getCoreMode() == CORE_DEBUG_MODE)
	  {
		  tlv = tlvCreatePacket(TLV_OK, 4, (uint8_t *)&empty);
		  previousMode = CORE_DEBUG_MODE ;
	  }
	  else
	  {
		isHandled = 0;
		previousMode = CORE_DEBUG_HALT;
	    pc = readCoreRegister(CORE_REG_PC);
	    tlv = tlvCreatePacket(TLV_OK, 4,(uint8_t *)& pc);
	  }
	  //else Throw(TLV_NOT_RUNNING);

	  tlvSend(session, tlv);


  returnThis(1);
}

/** Step the processor of target device once and send the current PC to host
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
uint32_t performSingleStepInto(Tlv_Session *session)
{
  uint32_t pc = 0 , initialPC = 0 ;

  initialPC = readCoreRegister(CORE_REG_PC);


  pc = stepIntoOnce();

  if(pc == initialPC)
    Throw(TLV_NOT_STEPPED);

  return pc;
}

/** Step the processor of target device multiple times and send the current PC to host
  *
  * Input     : session contain a element/handler used by tlv protocol
  *           : nInstructions is the number of instructions to step
  *
  */
int performMultipleStepInto(Tlv_Session *session, int nInstructions)
{
  int i = 0; uint32_t pcAddress = 0;

  for(i = 0 ; i < nInstructions ; i ++)
    pcAddress = performSingleStepInto(session);

  tlvReply(session, TLV_OK, 4, (uint8_t *)&pcAddress);

  returnThis(1);
}

/** Step over single instruction
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int performStepOver(Tlv_Session *session)
{
  uint32_t pc = 0 , initialPC = 0  ;

  initialPC = readCoreRegister(CORE_REG_PC);
  pc = stepOver();

  if(pc == 0 || pc == initialPC)
    Throw(TLV_NOT_STEPOVER);
  else
    tlvReply(session, TLV_OK, 4, (uint8_t *)&pc);

  returnThis(1);
}

/** Step out from subroutine
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
int performStepOut(Tlv_Session *session)
{
  uint32_t pc = 0 ;

  pc = stepOut();

  if(pc == 0)
    Throw(TLV_NOT_STEPOUT);
  else
    tlvReply(session, TLV_OK, 4, (uint8_t *)&pc);

  returnThis(1);
}

/** Set instruction breakpoint
 *
 * Input : session contain a element/handler used by tlv protocol
 *         instructionAddress is the address that will be breakpointed
 *
 */
int setBreakpoint(Tlv_Session *session, uint32_t instructionAddress)
{
  Tlv *tlv ;
  int comparatorUsed = 0;
  comparatorUsed = autoSetInstructionBreakpoint(instructionAddress);

  if( comparatorUsed == -1)
    Throw(TLV_BKPT_MAXSET);
  else  {
    SET_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG);
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
    tlvSend(session, tlv);
  }

  returnThis(1);
}

/** Set data watchpoint
 *
 * Input     : session contain a element/handler used by tlv protocol
 *             address is the address to be compared
 *             addressMask is the mask going to be applied to the address
 *             Possible value :
 *              WATCHPOINT_MASK_NOTHING  		  Compare all 32 bits of address set in DWT_COMPn
 *							WATCHPOINT_MASK_BIT0  			  Ignore Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT1_BIT0,		Ignore Bit1 and Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT2_BIT0,		Ignore Bit2 to Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT3_BIT0,		Ignore Bit3 to Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT4_BIT0     Ignore Bit4 to Bit 0 of address set in DWT_COMPn during comparison
 *                        ""                                              ""
 *							WATCHPOINT_MASK_BIT12_BIT0		Ignore Bit12 to Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT13_BIT0		Ignore Bit13 to Bit 0 of address set in DWT_COMPn during comparison
 *							WATCHPOINT_MASK_BIT14_BIT0	  Ignore Bit14 to Bit 0 of address set in DWT_COMPn during comparison
 *
 *          accessMode is the access mode for the watchpoint
 *          Possible value :
 *              WATCHPOINT_READ         Watchpoint on read access
 *              WATCHPOINT_WRITE        Watchpoint on write access
 *              WATCHPOINT_READWRITE    Watchpoint on read/write access
 */
int setWatchpoint(Tlv_Session *session,uint32_t address,Watchpoint_AddressMask addressMask,
                   uint32_t matchedData,Watchpoint_DataSize dataSize,Watchpoint_AccessMode accessMode)
{
  Tlv *tlv ;

  setDataWatchpoint_MatchingOneComparator(COMPARATOR_3,address,addressMask,matchedData,dataSize,accessMode);

  SET_FLAG_STATUS(session, TLV_SET_WATCHPOINT_FLAG);
  tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);

  returnThis(1);
}

/**
 *  Set instruction address remapping
 *
 *  Input :  session contain a element/handler used by tlv protocol
 *           instructionAddress is the address that will be remapped
 *           machineCode is the machine code that will remapped from the instructionAddress
 *
 */
int setInstructionRemapping(Tlv_Session *session,uint32_t instructionAddress,uint32_t machineCode)
{
  Tlv *tlv ;
  
  int comparatorUsed = 0;
  
  comparatorUsed = autoSetInstructionRemapping(instructionAddress,machineCode);
  if( comparatorUsed == -1)
    Throw(TLV_REMAP_MAXSET);
  else
  {
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
    tlvSend(session, tlv);
  }
  
  returnThis(1);
}

/**
 *  Set literal data remapping
 *
 *  Input :  session contain a element/handler used by tlv protocol 
 *           literalAddress is the address that will be remapped
 *           literalData is the data that will remapped from the literalAddress
 *
 */
int setLiteralRemapping(Tlv_Session *session,uint32_t literalAddress,uint32_t literalData)
{
  Tlv *tlv ;
  
  int comparatorUsed = 0;
  
  comparatorUsed = autoSetLiteralRemapping(literalAddress,literalData);
  if( comparatorUsed == -1)
    Throw(TLV_REMAP_MAXSET);
  else
  {
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
    tlvSend(session, tlv);
  }
  
  returnThis(1);
}


/** Remove single instruction breakpoint
 *
 * Input     : session contain a element/handler used by tlv protocol
 *             instructionAddress is the address set to breakpoint previously and going to be removed
 */
int removeHardwareBreakpoint(Tlv_Session *session, uint32_t instructionAddress)
{
  Tlv *tlv ;

  if(disableFlashPatchComparatorLoadedWithAddress(instructionAddress,INSTRUCTION_TYPE) == -1)
    Throw(TLV_ADDRESS_NOT_FOUND);
  else
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);

  returnThis(1);
}

/** Remove all hardware breakpoint
 *
 * Input     : session contain a element/handler used by tlv protocol
 */
int removeAllHardwareBreakpoint(Tlv_Session *session)
{
  Tlv *tlv ;

  disableAllFlashPatchComparatorSetToBreakpoint();

  tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);

  returnThis(1);
}

/** Remove watchpoint
 *
 * Input     : session contain a element/handler used by tlv protocol
 */
int removeWatchpoint(Tlv_Session *session)
{
  Tlv *tlv ;
  
  disableDWTComparator(COMPARATOR_1);
  tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);
  
  returnThis(1);
}

/** Stop single flash patch remapping
 *
 * Input     : session contain a element/handler used by tlv protocol
 *             address is the address going to be stop from remapping
 */
int stopFlashPatchRemapping(Tlv_Session *session,uint32_t address)
{
  Tlv *tlv ;
  int found = 0 ;

  found = disableFlashPatchComparatorLoadedWithAddress(address,INSTRUCTION_TYPE);
  found += disableFlashPatchComparatorLoadedWithAddress(address,LITERAL_TYPE);

  if(found < 0)
    Throw(TLV_ADDRESS_NOT_FOUND);
  else
    tlv = tlvCreatePacket(TLV_OK, 0, 0);

  tlvSend(session, tlv);

  returnThis(1);
}

/**
 * Stop all flash patch remapping
 *
 * Input     : session contain a element/handler used by tlv protocol
 */
int stopAllFlashPatchRemapping(Tlv_Session *session)
{
  Tlv *tlv ;
  disableAllFlashPatchComparatorSetToRemap();

  tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);

  returnThis(1);
}

/**
 *  Get all the address of the active breakpoint
 *
 * Input  : session contain a element/handler used by tlv protocol
 */
int getAllActiveBreakpoint(Tlv_Session *session)
{
  Tlv *tlv ;
  
  uint32_t *address;
  int size = 0, i=0 ;
  
  address = getAllActiveBreakpointAddress();
  
  for(i=0 ; i < INSTRUCTION_COMP_NUM ; i ++ )
  {
    if(address[i] != 0)
      size ++ ;
  }
  
  tlv = tlvCreatePacket(TLV_OK, size, (uint8_t*)address);
  tlvSend(session, tlv);
  
  returnThis(1);
}

/**
 * Check for breakpoint event and return the PC if breakpoint occurs
 *
 * Input  : session contain a element/handler used by tlv protocol
 */
int breakpointEventHandler(Tlv_Session *session)
{
  Tlv *tlv ;
  uint32_t pc =0 ;

  if(!hasBreakpointDebugEventOccured())
    return 0;
  else
  {
    pc = readCoreRegister(CORE_REG_PC);
    disableFPBUnit();
    SET_FLAG_STATUS(session,TLV_BREAKPOINT_WAS_SET_FLAG);
    clearBreakpointDebugEvent();
  }

  /* Clear ongoingProcess and breakPoint flag to indicate the breakpoint event is over*/
  CLEAR_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG);
  CLEAR_FLAG_STATUS(session, TLV_SET_BREAKPOINT_FLAG);

  tlv = tlvCreatePacket(TLV_OK, 4, (uint8_t *)&pc);
  tlvSend(session, tlv);

  returnThis(1);
}

/**
 * Check for watchpoint event and return the PC if watchpoint occurs
 *
 * Input  : session contain a element/handler used by tlv protocol
 */
int watchpointEventHandler(Tlv_Session *session)
{
  Tlv *tlv ;
  uint32_t pc =0 ;

  if(!(hasDataWatchpointOccurred())) {
	  return 0;
  }
  else
  {
    pc = readCoreRegister(CORE_REG_PC);
    disableDWTComparator(COMPARATOR_1);
    clearDWTTrapDebugEvent() ;
  }
  
  /* Clear ongoingProcess and breakPoint flag to indicate the breakpoint event is over*/
  CLEAR_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG);
  CLEAR_FLAG_STATUS(session, TLV_SET_WATCHPOINT_FLAG);

  tlv = tlvCreatePacket(TLV_OK, 4, (uint8_t *)&pc);
  tlvSend(session, tlv);

  returnThis(1);
}

/** readTargetMemory is a function to read target RAM using swd
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           destAddress is the address of the data need to be read
  *           size is the size of the data can be any value
  *
  * return  : NONE
  */
int readTargetMemory(Tlv_Session *session, uint32_t destAddress, int size) {
  int i;
  uint8_t dataBlock[size];

  /* Read from RAM using swd */
  for(i = 0; i < size; i++, destAddress++)
    dataBlock[i] = memoryReadAndReturnByte(destAddress);

  tlvReply(session, TLV_OK, size, dataBlock);

  returnThis(1);
}

/** writeTargetRam is a function to write target RAM using swd
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           dataAddress is the address of the data need to send
  *           destAddress is the address of the data need to be store
  *           size is the size of the data can be any value
  *
  * return  : NONE
  */
int writeTargetRam(Tlv_Session *session, uint8_t *dataAddress, uint32_t destAddress, int size) {
  /* Write data into correct address boundary */
  while(size > 0) { writeDataWithCorrectDataType(&dataAddress, &destAddress, &size); }
  tlvReply(session, TLV_OK, 0, NULL);

  returnThis(1);
}

/** writeTargetFlash is a function to write target flash (require flashLoader/flashProgrammer)
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           dataAddress is the address of the data need to send
  *           destAddress is the address of the data need to be store
  *           size is the size of the data can be any value
  *
  * return  : NONE
  */
int writeTargetFlash(Tlv_Session *session, uint8_t *dataAddress, uint32_t destAddress, int size) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

	uint32_t tempAddr = tempAddress;
	int tempSize = size;

  startTask(tb);
  /* Write to RAM using swd */
  while(tempSize > 0) { writeDataWithCorrectDataType(&dataAddress, &tempAddr, &tempSize); }
  /* Wait if stub is busy */
  previousTime = getSystemTime();
  while(IsStubBusy() == 0) {
    if(isTimeout(FIVE_SECOND, previousTime)) {
      resetTask(tb);
      Throw(PROBE_STUB_NOT_RESPONDING);
    }
    yield(tb);
  }
  /* Request flashProgrammer to copy data in tempAddress into flash */
  requestStubCopy(tempAddress, destAddress, size);
  /* Reply tlv acknowledge */
  tlvReply(session, TLV_OK, 0, NULL);

  endTask(tb);
  returnThis(1);
}

/** eraseTargetFlash is a function to erase target flash by section
  * (require flashLoader/flashProgrammer)
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           address is the address of flash need to erase
  *           size is the size of flash memory need to erase
  *
  * return  : NONE
  */
int eraseTargetFlash(Tlv_Session *session, uint32_t address, int size) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  startTask(tb);
  /* Wait if stub is busy */
  previousTime = getSystemTime();
  while(IsStubBusy() == 0) {
    if(isTimeout(FIVE_SECOND, previousTime)) {
      resetTask(tb);
      Throw(PROBE_STUB_NOT_RESPONDING);
    }
    yield(tb);
  }
  /* Request flashProgrammer to erase target flash */
  requestStubErase(address, size);
  /* Reply tlv acknowledge */
  tlvReply(session, TLV_OK, 0, NULL);

  endTask(tb);
  returnThis(1);
}

/** massEraseTargetFlash is a function erase flash by bank
  * (require flashLoader/flashProgrammer)
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           bankSelect is the flash bank to erase
  *
  * return  : NONE
  */
int massEraseTargetFlash(Tlv_Session *session, uint32_t bank) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  startTask(tb);
  /* Wait if stub is busy */
  previousTime = getSystemTime();
  while(IsStubBusy() == 0) {
    if(isTimeout(FIVE_SECOND, previousTime)) {
        resetTask(tb);
        Throw(PROBE_STUB_NOT_RESPONDING);
    }
    yield(tb);
  }
  /* Request flashProgrammer to erase target flash */
  requestStubMassErase(bank);
  /* Reply tlv acknowledge */
  tlvReply(session, TLV_OK, 0, NULL);

  endTask(tb);
  returnThis(1);
}

void eventOccured(Tlv_Session *session, EventType event) {
  CLEAR_FLAG_STATUS(session, TLV_ONGOING_PROCESS_FLAG);

  Tlv *tlv = tlvCreatePacket(TLV_OK, 0, (uint8_t *)&event);

  tlvSend(session, tlv);
}

EventType isDebugEventOccur(EventType event) {

  if(event == BREAKPOINT_EVENT)       { if(hasBreakpointDebugEventOccured())  return event; }
  else if(event == WATCHPOINT_EVENT)  { if(hasDataWatchpointOccurred())       return event; }

  return NO_EVENT;
}

int debugEventHandler(Tlv_Session *session, EventType event) {
  static EventType eventOccured;
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  startTask(tb);
  eventOccured = NO_EVENT;
  previousTime = getSystemTime();
  while((eventOccured = isDebugEventOccur(event)) == NO_EVENT) {
    if(isTimeout(TWO_SECOND, previousTime)) {
      resetTask(tb);
      Throw(PROBE_EVENT_NOT_RESPONDING);
    }
    yield(tb);
  }
  tlvReply(session, TLV_OK, 1, (uint8_t *)&eventOccured);
  endTask(tb);

  returnThis(1);
}

int loopBack(Tlv_Session *session, Tlv *packet) {
  Tlv *tlv;
  int i = 0;

  /* Size minus 1 because of the checksum value */
  for(i = 0; i < packet->length -1; i++) {
	 packet->value[i] += 2;
  }
  tlv = tlvCreatePacket(TLV_OK, packet->length - 1, packet->value);

  tlvSend(session, tlv);
  returnThis(1);
}

int comPortVerification(Tlv_Session *session) {

  uint8_t reply[] = {"Hello From The Other Side"};

  tlvReply(session, TLV_OK, sizeof(reply), reply);

  returnThis(1);
}

/** selectTask is a function to select instruction
  * base on tlv->type
  *
  * Input   : tlv is pointer pointing to tlv packet
  *
  * Return  : NONE
  */
int selectTask(Tlv_Session *session, Tlv *tlv)  {

  switch(tlv->type) {
    case TLV_WRITE_RAM                  : writeTargetRam(session, &tlv->value[4], get4Byte(&tlv->value[0]), tlv->length - 5);       break;
    case TLV_WRITE_FLASH                : writeTargetFlash(session, &tlv->value[4], get4Byte(&tlv->value[0]), tlv->length - 5);     break;
    case TLV_READ_MEMORY                : readTargetMemory(session, get4Byte(&tlv->value[0]), tlv->value[4]);                       break;
    case TLV_WRITE_REGISTER             : writeTargetRegister(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));         break;
    case TLV_READ_REGISTER              : readTargetRegister(session, get4Byte(&tlv->value[0]));                                    break;
    case TLV_HALT_TARGET                : haltTarget(session);                                                                      break;
    case TLV_RUN_TARGET                 : runTarget(session);                                                                       break;
    case TLV_STEP                       : performMultipleStepInto(session, get4Byte(&tlv->value[0]));                               break;
    case TLV_STEPOVER					          : performStepOver(session);																	                                break;
    case TLV_LISTBREAKPOINT             : getAllActiveBreakpoint(session);
    case TLV_BREAKPOINT                 : setBreakpoint(session, get4Byte(&tlv->value[0]));                                         break;
    case TLV_REMOVE_BREAKPOINT          : removeHardwareBreakpoint(session, get4Byte(&tlv->value[0]));                              break;
    case TLV_REMOVE_ALL_BREAKPOINT      : removeAllHardwareBreakpoint(session);                                                     break;
    case TLV_WATCHPOINT                 : setWatchpoint(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]), 
                                                        get4Byte(&tlv->value[8]), get4Byte(&tlv->value[12]), 
                                                        get4Byte(&tlv->value[16]));                                                 break;
    case TLV_INSTRUCTION_REMAP          : setInstructionRemapping(session,get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));      break;
    case TLV_LITERAL_REMAP              : setLiteralRemapping(session,get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));          break;
    case TLV_REMOVE_WATCHPOINT          : removeWatchpoint(session);                                                                break;
    case TLV_STOP_REMAP                 : stopFlashPatchRemapping(session, get4Byte(&tlv->value[0]));                               break;
    case TLV_STOP_ALL_REMAP             : stopAllFlashPatchRemapping(session);                                                      break;
    case TLV_FLASH_ERASE                : eraseTargetFlash(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4]));            break;
    case TLV_FLASH_MASS_ERASE           : massEraseTargetFlash(session, get4Byte(&tlv->value[0]));                                  break;
    case TLV_SOFT_RESET                 : performSoftResetOnTarget(session);                                                        break;
    case TLV_HARD_RESET                 : performHardResetOnTarget(session);                                                        break;
    case TLV_VECT_RESET					        : performVectorResetOnTarget(session);														                          break;
    case TLV_LOOP_BACK                  : loopBack(session, tlv);                                                                   break;
    case TLV_DEBUG_EVENTS               : debugEventHandler(session, tlv->value[0]);                                                break;
    case TLV_VERIFY_COM_PORT            : comPortVerification(session);                                                             break;

    default : break;
  }
  return 0;
}

/** taskManager is a function to handle the coming tlv packet and interpret it
  *
  * input     :
  *
  * return    :
  */
int taskManager(Tlv_Session *session)  {
  CEXCEPTION_T err;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  Try {
    startTask(tb);
    /* Wait packet to arrive */
    while((packet = tlvReceive(session)) == NULL) { yield(tb); }
    /* Wait for task to complete */
    await(selectTask(session, packet), tb);
    endTask(tb);
  }
  Catch(err) {
    resetTask(tb);
    Throw(err);
  }

  returnThis(1);
}


/** Select the best method (Word,Halfword or Byte) to write into RAM
  * Input   : data is the address of the buffer storing the data to be written
  *           destAddress is the starting address where the data is going to be written
  *           size is total amount of data to be written
  */
void selectAppropriateMethodToWriteRAM(uint8_t *data, uint32_t destAddress, int size)
{
  uint32_t data32 = 0 , data16 = 0;
  int i ;

  while(size !=0)
  {
    if((destAddress %4 == 0) && (size >=4))
    {
      data32 = 0 ;
      for(i = 0 ; i < 4 ; data++ ,i ++)
        data32 += (*data << i*8);

      memoryWriteWord(destAddress,data32);
      size -= 4 ;
      destAddress +=4;
    }
    else if((destAddress % 4 <= 2) && (size >=2))
    {
      data16 = 0 ;
      for(i = 0 ; i < 2 ; data++ ,i ++)
        data16 += (*data << i*8);

      memoryWriteHalfword(destAddress,data16);
      size -=2 ; destAddress +=2 ;
    }
    else
    {
      memoryWriteByte(destAddress,*data);
      size --;  destAddress ++; data ++;
    }
  };
}

/**==============================================================================
                     ########## Write Ram internal function ##########
   ==============================================================================*/

/** getDataType is a function to determine the suitable data type
  * to write into the correct address boundary
  *
  * Input   : address is the address of the data need to be store
  *           size is the size of the data can be any value
  *
  * return  : WORD_TYPE       if address is 4byte boundary and size >= 4
  *           HALFWORD_TYPE   if address is 2byte boundary and size >= 2
  *           BYTE_TYPE       if address is 1byte boundary
  */
DataType getDataType(uint32_t address, int size) {
  int boundary = (address & BOUNDARY_MASK);
  int dataType = 0;

  /* Case 1 */
  if(boundary == WORD_BOUNDARY) {
    if(size >= WORD_SIZE)
      dataType = WORD_TYPE;
    else if(size > BYTE_SIZE && size < WORD_SIZE)
      dataType = HALFWORD_TYPE;
    else
      dataType = BYTE_TYPE;
  }
  /* Case 2 */
  else if(boundary == HALFWORD_BOUNDARY) {
    if(size >= HALFWORD_SIZE)
      dataType = HALFWORD_TYPE;
    else dataType = BYTE_TYPE;
  }
  /* Case 3 */
  else dataType = BYTE_TYPE;

  return dataType;
}

/** writeDataWithCorrectDataType is a function to write data
  * into correct address boundary
  *
  * Input   : data is the pointer-to-pointer contain address of the data need to write
  *           address is the address of the data need to be store
  *           size is the size of the data can be any value
  *
  * return  : NONE
  */
void writeDataWithCorrectDataType(uint8_t **data, uint32_t *address, int *size) {
  DataType type = 0;
  uint8_t *dataAddress = *data;
  uint32_t destAddress = *address;

  type = getDataType(destAddress, *size);

  if(type == WORD_TYPE) {
    // printf("word %x\n", get4Byte(dataAddress));
    memoryWriteWord(destAddress, get4Byte(dataAddress));
  }
  else if(type == HALFWORD_TYPE) {
    // printf("halfword %x\n", get2Byte(dataAddress));
    memoryWriteHalfword(destAddress, get2Byte(dataAddress));
  }
  else if(type == BYTE_TYPE) {
    // printf("byte %x\n", *dataAddress);
    memoryWriteByte(destAddress, *dataAddress);
  }

  *data += type;
  *address += type;
  *size -= type;
}
