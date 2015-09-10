#include "ProgramWorker.h"

/** load_SectorErase_Instruction is a function to load the sector erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : startAddress is the address to begin erase
  *           endAddress is the address to end erase
  *
  * output  : NONE
  */
void loadEraseSectorInstruction(uint32_t *startSector, uint32_t *endSector)  {
  uint32_t targetStatus = 0;
  /* Continues wait for target to release */
  do  {
    targetStatus = memoryReadAndReturnWord(SWD_TARGET_STATUS);
  } while(targetStatus != TARGET_OK);
  
  /* load flash start and end address to sram */
  memoryWriteWord(SWD_FLASH_START_ADDRESS, (uint32_t)startSector);
  memoryWriteWord(SWD_FLASH_END_ADDRESS, (uint32_t)endSector);
  
  /* load instruction to sram */
  memoryWriteWord(SWD_INSTRUCTION, INSTRUCTION_ERASE_SECTOR);
}

/** loadMassEraseInstruction is a function to load the mass erase
  * instruction into SRAM to tell the swdStub
  *
  * input   : bankSelect can be one of the following value
  *            + FLASH_BANK_1: Bank1 to be erased
  *            + FLASH_BANK_2: Bank2 to be erased
  *            + FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  *
  * output  : NONE
  */
void loadMassEraseInstruction(uint32_t bankSelect)  {
  uint32_t targetStatus = 0;
  
  /* Continues wait for target to release */
  do  {
    targetStatus = memoryReadAndReturnWord(SWD_TARGET_STATUS);
  } while(targetStatus != TARGET_OK);
  
  /* load bank select to sram */
  memoryWriteWord(SWD_BANK_SELECT, bankSelect);
  
  /* load instruction to sram */
  memoryWriteWord(SWD_INSTRUCTION, INSTRUCTION_MASS_ERASE);  
}

/** loadCopyInstruction is a function copy data from src (SRAM) to dest (Flash)
  *
  * input   : src is the beginning SRAM address contain all the information
  *           dest is the flash address all the information need to copy over there
  *           length is to determine how many words need to copy over
  *
  * output  : NONE
  */
void loadCopyFromSRAMToFlashInstruction(uint32_t *dataAddress, uint32_t *destAddress, int size) {
  uint32_t targetStatus = 0;
  
  /* Continues wait for target to release */
  do  {
    targetStatus = memoryReadAndReturnWord(SWD_TARGET_STATUS);
  } while(targetStatus != TARGET_OK);

  /* load SRAM start address into sram */
  memoryWriteWord(SWD_SRAM_START_ADDRESS, (uint32_t)dataAddress);
  
  /* load Flash start address into sram */
  memoryWriteWord(SWD_FLASH_START_ADDRESS, (uint32_t)destAddress);
  
  /* load length into sram */
  memoryWriteWord(SWD_DATA_SIZE, size);

	/* load copy instructoin into sram */
  memoryWriteWord(SWD_INSTRUCTION, INSTRUCTION_COPY);
}

/** writeTargetRegister is a function to write value into target register using swd
  *
  * input     : session contain a element/handler used by tlv protocol
  *             registerAddress is the address of the target register
  *             data is the data need to write into the target register
  *
  * return    : NONE
  */
void writeTargetRegister(Tlv_Session *session, uint32_t registerAddress, uint32_t data) {
  
  writeCoreRegister(registerAddress, data);
  
  Tlv *tlv = tlvCreatePacket(TLV_OK, 0, 0);
  
  tlvSend(session, tlv);
}

/** readTargetRegister is a function to read value from target register using swd
  *
  * input     : session contain a element/handler used by tlv protocol
  *             registerAddress is the address of the target register
  *
  * return    : NONE
  */
void readTargetRegister(Tlv_Session *session, uint32_t registerAddress) {
  uint32_t data = 0;
  
  readCoreRegister(registerAddress, &data);
  
  Tlv *tlv = tlvCreatePacket(TLV_OK, 4, (uint8_t *)&data);
  
  tlvSend(session, tlv);
}

/** Perform a soft reset on the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void performSoftResetOnTarget(Tlv_Session *session)
{
  softResetTarget();
  Tlv *tlv = tlvCreatePacket(TLV_OK, 0, 0);
  
  tlvSend(session, tlv);
}

/** Perform a hard reset on the target device
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void performHardResetOnTarget(Tlv_Session *session)
{
  hardResetTarget();
  Tlv *tlv = tlvCreatePacket(TLV_OK, 0, 0);
  
  tlvSend(session, tlv);
}


/** Halt the processor of the target device 
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void haltTarget(Tlv_Session *session)
{
  Tlv *tlv ;
  uint8_t errorCode = TLV_NOT_HALTED ;
  setCoreMode(CORE_DEBUG_HALT);
  
  if(getCoreMode() == CORE_DEBUG_HALT)
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
  else
    tlv = tlvCreatePacket(TLV_NOT_OK, 1, &errorCode);
  
  tlvSend(session, tlv);
}

/** Run the target device 
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void runTarget(Tlv_Session *session)
{
  Tlv *tlv ;
  uint8_t errorCode = TLV_NOT_RUNNING ;
  setCoreMode(CORE_DEBUG_MODE);
  
  if(getCoreMode() == CORE_DEBUG_MODE)
    tlv = tlvCreatePacket(TLV_OK, 0, 0);
  else
    tlv = tlvCreatePacket(TLV_NOT_OK, 1, &errorCode);
  
  tlvSend(session, tlv);
}

/** Step the processor of target device once and send the current PC to host
  *
  * Input     : session contain a element/handler used by tlv protocol
  *
  */
void singleStepTarget(Tlv_Session *session)
{
  Tlv *tlv ;
  uint8_t errorCode = TLV_NOT_STEPPED ;
  uint32_t data = 0 ;
  
  setCoreMode(CORE_SINGLE_STEP);
  
  if(getCoreMode() == CORE_SINGLE_STEP)
  {
    readCoreRegister(CORE_REG_PC, &data);
    tlv = tlvCreatePacket(TLV_STEP,4, (uint8_t *)&data);
  } 
  else
    tlv = tlvCreatePacket(TLV_NOT_OK, 1, &errorCode);
  
  tlvSend(session, tlv);
}

/** Step the processor of target device multiple times and send the current PC to host
  *
  * Input     : session contain a element/handler used by tlv protocol
  *           : nInstructions is the number of instructions to step
  *
  */
void multipleStepTarget(Tlv_Session *session, int nInstructions)
{
  Tlv *tlv ;
  uint8_t errorCode = TLV_NOT_STEPPED ;
  uint32_t data = 0 ;
  
  stepOnly(nInstructions);
  
  if(getCoreMode() == CORE_SINGLE_STEP)
  {
    readCoreRegister(CORE_REG_PC, &data);
    tlv = tlvCreatePacket(TLV_STEP,4, (uint8_t *)&data);
  } 
  else
    tlv = tlvCreatePacket(TLV_NOT_OK, 1, &errorCode);
  
  tlvSend(session, tlv);
}

void setBreakpoint(uint32_t instructionAddress,int matchingMode)
{
  
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
void writeTargetRam(Tlv_Session *session, uint32_t *dataAddress, uint32_t destAddress, int size)  {
  int i;
  
  /* Write to RAM using swd */
  for(i = 0; i < size - 1; i += 4)  {
    /* Data start at position 4 */
    memoryWriteWord(destAddress, *dataAddress++);
    destAddress += 4;
  }
  
  Tlv *tlv = tlvCreatePacket(TLV_OK, 0, 0);
  tlvSend(session, tlv);
}

/** readTargetRam is a function to read target RAM using swd
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           destAddress is the address of the data need to be read
  *           size is the size of the data can be any value
  *
  * return  : NONE
  */
void readTargetRam(Tlv_Session *session, uint32_t destAddress, int size) {
  int i; uint8_t chksum = 0;
  uint32_t readData = 0;
  
  Tlv *tlv = tlvCreatePacket(TLV_OK, size + 4, NULL);
  
  /* store destAddress checksum */
  chksum = tlvPackIntoBuffer(tlv->value, (uint8_t *)&destAddress, 4);
  
  /* Read from RAM using swd */
  for(i = 0; i < size; i += 4)  {
    readData = memoryReadAndReturnWord(destAddress);
    /* Data start at position 4 */
    chksum += tlvPackIntoBuffer(&tlv->value[4 + i], (uint8_t *)&readData, 4);
    destAddress += 4;
  }

  tlv->value[tlv->length - 1] = chksum;
  
  tlvSend(session, tlv);
}

/** selectTask is a function to select instruction 
  * base on tlv->type
  *
  * Input   : tlv is pointer pointing to tlv packet
  *
  * Return  : NONE
  */
void selectTask(Tlv_Session *session, Tlv *tlv)  {
  
  switch(tlv->type) {
    case TLV_WRITE_RAM      : writeTargetRam(session, &get4Byte(&tlv->value[4]), get4Byte(&tlv->value[0]), tlv->length - 4); break;
    case TLV_READ_RAM       : readTargetRam(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4])); break;
    case TLV_WRITE_REGISTER : writeTargetRegister(session, get4Byte(&tlv->value[0]), get4Byte(&tlv->value[4])); break;
    case TLV_READ_REGISTER  : readTargetRegister(session, get4Byte(&tlv->value[0])); break;
    case TLV_HALT_TARGET    : haltTarget(session); break;
    case TLV_RUN_TARGET     : runTarget(session); break;
    case TLV_STEP           : multipleStepTarget(session, get4Byte(&tlv->value[0])); break;
  }
}

/** probeTaskManager
  */
void probeTaskManager(Tlv_Session *session)  {
  static Tlv *packet;
  CEXCEPTION_T err;
  
  switch(session->probeState)  {
    case PROBE_RECEIVE_PACKET :
      Try {
        packet = tlvReceive(session);
        if(verifyTlvPacket(packet)) {
          session->probeState = PROBE_INTERPRET_PACKET;
        }
      }
      Catch(err)  {
        tlvReportError(session, err);
      }
      break;
      
    case PROBE_INTERPRET_PACKET :
      selectTask(session, packet);
      session->probeState = PROBE_RECEIVE_PACKET;
      break;
  }
}
