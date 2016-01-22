#ifndef FPB_Unit_H
#define FPB_Unit_H

#include "stm32f429xx.h"
#include "MemoryReadWrite.h"
#include "FPBUnitEx.h"
#include "CoreDebug.h"

#define enableFPBUnit()   memoryWriteWord((uint32_t)&(FPB->FP_CTRL),ENABLE_FPB)
#define disableFPBUnit()  memoryWriteWord((uint32_t)&(FPB->FP_CTRL),DISABLE_FPB)

int checkForValidInstructionComparator(int instructionCOMPno);
int checkForValidLiteralComparator(int literalCOMPno);
int checkForValidFPComparator(uint32_t compNo);

uint32_t swapHalfword(uint32_t data);

int manualSetInstructionBreakpoint(int instructionCOMPno,uint32_t instructionAddress,int matchingMode);
int manualSetInstructionRemapping(int instructionCOMPno,uint32_t instructionAddress, uint32_t remapAddress);
int manualSetLiteralRemapping(int literalCOMPno,uint32_t literalAddress, uint32_t remapAddress);

int autoSetInstructionBreakpoint(uint32_t instructionAddress);
int autoSetInstructionRemapping(uint32_t instructionAddress,uint32_t machineCode);
int autoSetLiteralRemapping(uint32_t literalAddress,uint32_t literalData);

int disableFlashPatchInstructionComparator(int instructionCOMPno);
int disableFlashPatchLiteralComparator(int literalCOMPno);
int disableFlashPatchComparatorLoadedWithAddress(uint32_t address,int comparatorType);
void disableAllFlashPatchComparator();

void readAndUpdateComparatorReadyFlag(int comparatorType);
uint32_t selectNextFreeComparator(int comparatorType);

void initialiseFPBUnit();

void disableAllFlashPatchComparatorSetToBreakpoint();
void disableAllFlashPatchComparatorSetToRemap();

uint32_t retrieveBreakpointAddress(int instructionCOMPno);

uint32_t * getAllActiveBreakpointAddress();
#endif // FPB_Unit_H
