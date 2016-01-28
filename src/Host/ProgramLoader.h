#ifndef ProgramLoader_H
#define ProgramLoader_H

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "Tlv.h"
#include "Yield.h"
#include "ErrorCode.h"
#include "CException.h"
#include "Interface.h"
#include "SystemTime.h"
#include "ProgramVerifier.h"

typedef enum {
  PROCESS_BUSY = 0,
  PROCESS_DONE
} Process_Status;

#if defined(TEST)
/* Flash Programmer File Path */
#define FP_PATH    "test/ElfFiles/FlashProgrammer.elf"
#else
#define FP_PATH    "target/FlashProgrammer.elf"
#endif

/* ##### Host Interpreter Macros ##### */
#define isProbeAlive(timeout, x)      do {  if(timeout)                     \
                                            {                               \
                                              resetTask(x);                 \
                                              Throw(PROBE_NOT_RESPONDING);  \
                                            }                               \
                                         } while(0)                         \
                                           
#define systemExit(__SESSION__)       ((__SESSION__)->exit = 1)
#define isExit(__SESSION__)           (((__SESSION__)->exit == 1) ? 1 : 0  )

#define writeRam(session, data, address, size)     writeMemory(session, data, address, size, TLV_WRITE_RAM)
#define writeFlash(session, data, address, size)   writeMemory(session, data, address, size, TLV_WRITE_FLASH)

/* Read/Write target register */
Process_Status writeRegister(Tlv_Session *session, uint32_t registerAddress, uint32_t data);
uint32_t readRegister(Tlv_Session *session, uint32_t registerAddress);

/* Halt target */
Process_Status halt(Tlv_Session *session);

/* Run target */
Process_Status run(Tlv_Session *session);

/* Step target */
uint32_t multipleStep(Tlv_Session *session, int nInstructions);
uint32_t stepOver(Tlv_Session *session);

/* Reset */
Process_Status softReset(Tlv_Session *session);
Process_Status hardReset(Tlv_Session *session);
Process_Status vectorReset(Tlv_Session *session);

/* Read/Write target memory */
Process_Status writeMemory(Tlv_Session *session, uint8_t *data, uint32_t address, int size, Tlv_Command memory);
uint8_t *readMemory(Tlv_Session *session, uint32_t address, int size);

/* Write/Load Flash and RAM */
int loadProgram(Tlv_Session *session, Program *p, Tlv_Command memorySelect);
int loadRam(Tlv_Session *session, Program *p);
int loadFlash(Tlv_Session *session, Program *p);
Process_Status reactiveProgram(Tlv_Session *session, Program *p);
Process_Status _writeFlash(Tlv_Session *session, uint8_t *dataAddress, uint32_t destAddress, int size);
/* Flash Erase */
Process_Status eraseSection(Tlv_Session *session, uint32_t address, int size);
Process_Status eraseAll(Tlv_Session *session, uint32_t banks);

/* Set Breakpoint */
Process_Status setBreakpoint(Tlv_Session *session, uint32_t address);

/* Set Watchpoint */
Process_Status setWatchpoint(Tlv_Session *session, uint32_t address,uint32_t addressMask,uint32_t matchedData,int dataSize,uint32_t accessMode);

/* Set Remapping */
Process_Status setInstructionRemapping(Tlv_Session *session,uint32_t instructionAddress,uint32_t machineCode);
Process_Status setLiteralRemapping(Tlv_Session *session,uint32_t literalAddress,uint32_t literalData);

/* List Breakpoint*/
Process_Status listAllActiveBreakpoint(Tlv_Session *session);

/* List Watchpoint*/
Process_Status listAllActiveWatchpoint(Tlv_Session *session);

/* Remove Breakpoint */
Process_Status removeBreakpoint(Tlv_Session *session, uint32_t instructionAddress);
Process_Status removeAllBreakpoint(Tlv_Session *session);

/* Remove Watchpoint*/
Process_Status removeWatchpoint(Tlv_Session *session);

/* Stop Flash Patch Remap */
Process_Status stopFlashPatchRemapping(Tlv_Session *session,uint32_t address);
Process_Status stopAllFlashPatchRemapping(Tlv_Session *session);

/* Wait Debug Events */
EventType tlvWaitDebugEvents(Tlv_Session *session, EventType event);

int selectCommand(Tlv_Session *session, User_Session *us);
int hostInterpreter(Tlv_Session *session);

uint8_t *createDataBlock(int size);
void delDataBlock(uint8_t *s);

#endif // ProgramLoader_H