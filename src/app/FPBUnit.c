#include "FPBUnit.h"

int instructionComparatorReady[INSTRUCTION_COMP_NUM] = {};
int literalComparatorReady[LITERAL_COMP_NUM] = {};

/**
 *  Use to check for valid Flash Patch Instruction Comparator
 *
 *  Input   : instructionCOMPno contains the comparator number to be checked
 *  Output  : return 1 for valid comparator
 *            return -1 for invalid comparator
 *
 */
int checkForValidInstructionComparator(int instructionCOMPno)
{
  int result = - 1;
  
  if(instructionCOMPno >= 0 && instructionCOMPno <= 5)
    result = 1;
  
  return result ;
}

/**
 *  Use to check for valid Flash Patch Literal Comparator
 *
 *  Input   : literalCOMPno contains the comparator number to be checked
 *  Output  : return 1 for valid comparator
 *            return -1 for invalid comparator
 *
 */
int checkForValidLiteralComparator(int literalCOMPno)
{
  int result = - 1;
  
  if(literalCOMPno == 0 || literalCOMPno == 1)
    result = 1 ;
  
  return result ;
}

/**
 *  Swap the halfword of a 32 bit data
 *  
 *  Input   : data contains the 32 bit data to be swapped
 *  Output  : returns the swapped 32 bit data
 */
uint32_t swapHalfword(uint32_t data)
{
  uint32_t lowerHalfword = (data & LOWERHALFWORD_MASK) << 16 ;
  uint32_t upperHalfword = (data & UPPERHALFWORD_MASK) >> 16 ;

  return (upperHalfword + lowerHalfword);
}


/**
 *  Use to set for instruction address breakpoint
 *
 *  Input : instructionCOMPno is the instruction comparator going to be used
 *				  Possible values : 
 *					  INSTRUCTION_COMP0			Instruction comparator number 0	
 *					  INSTRUCTION_COMP1			Instruction comparator number 1	
 *					  INSTRUCTION_COMP2     Instruction comparator number 2
 *					  INSTRUCTION_COMP3     Instruction comparator number 3
 *					  INSTRUCTION_COMP4     Instruction comparator number 4
 *					  INSTRUCTION_COMP5     Instruction comparator number 5
 *
 *          instructionAddress is the address that will be breakpointed
 *          matchingMode defines the behaviour when the comparator is matched
 *          Possible value :
 *					  MATCH_LOWERHALFWORD	    Set breakpoint on lower halfword (Bits[1:0] are 0b00)			
 *					  MATCH_UPPERHALFWORD	    Set breakpoint on upper halfword (Bits[1:0] are 0b10)			
 *					  MATCH_WORD		          Set breakpoint on both upper and lower halfword						
 *
 *  Output :  return 0 if instruction breakpoint is set
 *            return -1 if invalid comparator is chosen
 */
int manualSetInstructionBreakpoint(int instructionCOMPno,uint32_t instructionAddress,int matchingMode)
{
  uint32_t configData = 0 ;
  
  if(checkForValidInstructionComparator(instructionCOMPno) == -1)
    return -1 ;
  
  configData = (instructionAddress & FP_COMP_ADDRESS_MASK) +matchingMode + FP_COMP_ENABLE ;
   
  memoryWriteWord((uint32_t)&(INSTRUCTION_COMP[instructionCOMPno]),configData);
  
  instructionComparatorReady[instructionCOMPno] = COMP_BUSY;
  return 0 ;
}

/**
 *  Use to set for instruction address remapping
 *
 *  Input : instructionCOMPno is the instruction comparator going to be used
 *				  Possible values : 
 *					  INSTRUCTION_COMP0			Instruction comparator number 0	
 *					  INSTRUCTION_COMP1			Instruction comparator number 1	
 *					  INSTRUCTION_COMP2     Instruction comparator number 2
 *					  INSTRUCTION_COMP3     Instruction comparator number 3
 *					  INSTRUCTION_COMP4     Instruction comparator number 4
 *					  INSTRUCTION_COMP5     Instruction comparator number 5
 *
 *          instructionAddress is the address that will be remapped to the remap addresss + 4n (where n = 0,1,2,3,4,5 of INSTRUCTION_COMPn)
 *          remap address is the base address for remapping
 *
 *  Output :  return 0 if instruction remapping is set
 *            return -1 if invalid comparator is chosen
 */
int manualSetInstructionRemapping(int instructionCOMPno,uint32_t instructionAddress, uint32_t remapAddress)
{
  uint32_t configData = 0 ;
  
  if(checkForValidInstructionComparator(instructionCOMPno) == -1)
    return -1 ;
  
  configData = (instructionAddress & FP_COMP_ADDRESS_MASK) + FP_COMP_ENABLE ;
  
  memoryWriteWord((uint32_t)&(FPB->FP_REMAP),(remapAddress & FP_REMAP_ADDRESS_MASK));
  memoryWriteWord((uint32_t)&(INSTRUCTION_COMP[instructionCOMPno]),configData);

  instructionComparatorReady[instructionCOMPno] = COMP_REMAP;
  
  return 0 ;
}

/**
 *  Use to set for literal address remapping
 *
 *  Input : literalCOMPno is the literal comparator going to be used
 *				  Possible values : 
 *					  LITERAL_COMP0			Literal comparator number 0	
 *					  LITERAL_COMP1			Litreal comparator number 1	
 *
 *          literalAddress is the address that will be remapped to the remap addresss + 0x24 + 4n (where n = 0,1 of LITERAL_COMPn)
 *          remap address is the base address for remapping
 *
 *  Output :  return 0 if literal remapping is set
 *            return -1 if invalid comparator is chosen
 */
int manualSetLiteralRemapping(int literalCOMPno,uint32_t literalAddress, uint32_t remapAddress)
{
  uint32_t configData = 0 ;
  
  if(checkForValidLiteralComparator(literalCOMPno) == -1)
    return -1 ;
  
  configData = (literalAddress & FP_COMP_ADDRESS_MASK) + FP_COMP_ENABLE ;
  
  memoryWriteWord((uint32_t)&(FPB->FP_REMAP),(remapAddress & FP_REMAP_ADDRESS_MASK));
  memoryWriteWord((uint32_t)&(LITERAL_COMP[literalCOMPno]),configData);
  
  literalComparatorReady[literalCOMPno] = COMP_REMAP ;
  
  return 0 ;
}

/**
 *  Use to set for instruction address breakpoint ()
 *
 *  Input : instructionAddress is the address that will be breakpointed			
 *
 *  Output :  return INSTRUCTION_COMP0 - INSTRUCTION_COMP5 for valid comparator used
 *            return -1 if invalid comparator is chosen
 */
int autoSetInstructionBreakpoint(uint32_t instructionAddress)
{
  int comparatorToUse = 0 ;
  int matchingMode = 0 ;
  
  comparatorToUse = selectNextFreeComparator(INSTRUCTION_TYPE);
  if(comparatorToUse == -1)
    return -1 ;

  if((instructionAddress & UPPERMATCHINGMODE_MASK))
    matchingMode = MATCH_UPPERHALFWORD ;
  else
    matchingMode = MATCH_LOWERHALFWORD ;
  
  manualSetInstructionBreakpoint(comparatorToUse,instructionAddress,matchingMode);

  return comparatorToUse;
}

/**
 *  Use to set for instruction address remapping
 *
 *  Input :  instructionAddress is the address that will be remapped
 *           machineCode is the machine code that will remapped from the instructionAddress
 *
 *  Output :  return INSTRUCTION_COMP0 - INSTRUCTION_COMP5 for valid comparator used
 *            return -1 if invalid comparator is chosen
 */
int autoSetInstructionRemapping(uint32_t instructionAddress,uint32_t machineCode)
{
  int comparatorToUse = 0 ;
  uint32_t dataRead = 0 ;
  
  comparatorToUse = selectNextFreeComparator(INSTRUCTION_TYPE);
  if(comparatorToUse == -1)
    return -1 ;
  
  if(machineCode > 0xFFFF)
    machineCode = swapHalfword(machineCode);
  else
  {
    memoryReadHalfword(instructionAddress+2,&dataRead);
    machineCode = (machineCode << 16) + dataRead;
    machineCode = swapHalfword(machineCode);
  }
  memoryWriteWord((REMAP_BASE + (4*comparatorToUse)),machineCode);
  manualSetInstructionRemapping(comparatorToUse,instructionAddress,REMAP_BASE);
  
  return comparatorToUse;
}

/**
 *  Use to set for literal data remapping
 *
 *  Input :  literalAddress is the address that will be remapped
 *           literalData is the machine code that will remapped from the literalAddress
 *
 *  Output :  return LITERAL_COMP0 - LITERAL_COMP1 for valid comparator used
 *            return -1 if invalid comparator is chosen
 */
int autoSetLiteralRemapping(uint32_t literalAddress,uint32_t literalData)
{
  int comparatorToUse = 0 ;
  
  comparatorToUse = selectNextFreeComparator(LITERAL_TYPE);
  if(comparatorToUse == -1)
    return -1 ;
  
  memoryWriteWord((REMAP_BASE + (4*(comparatorToUse + INSTRUCTION_COMP_NUM))),literalData);
  manualSetLiteralRemapping(comparatorToUse,literalAddress,REMAP_BASE);
  
  return comparatorToUse;
}

/**
 *  Disable the selected Instruction Comparator
 * 
 *  Input : instructionCOMPno is the comparator number going to be disabled		
 *				  Possible values : 
 *					  INSTRUCTION_COMP0			Instruction comparator number 0	
 *					  INSTRUCTION_COMP1			Instruction comparator number 1	
 *					  INSTRUCTION_COMP2     Instruction comparator number 2
 *					  INSTRUCTION_COMP3     Instruction comparator number 3
 *					  INSTRUCTION_COMP4     Instruction comparator number 4
 *					  INSTRUCTION_COMP5     Instruction comparator number 5
 *
 *  Output :  return 0 if comparator is disabled
 *            return -1 if invalid comparator is chosen
 */
int disableFlashPatchInstructionComparator(int instructionCOMPno)
{
  if(checkForValidInstructionComparator(instructionCOMPno) == -1)
    return -1 ;
  
  memoryWriteWord((uint32_t)&(INSTRUCTION_COMP[instructionCOMPno]),FP_COMP_DISABLE);
  
  instructionComparatorReady[instructionCOMPno] = COMP_READY ;
  
  return 0 ;
}

/**
 *  Disable the selected Literal Comparator
 * 
 *  Input : literalCOMPno is the comparator number going to be disabled		
 *				  Possible values : 
 *					  LITERAL_COMP0			    Literal comparator number 0	
 *					  LITERAL_COMP1			    Litreal comparator number 1	
 *
 *  Output :  return 0 if comparator is disabled
 *            return -1 if invalid comparator is chosen
 */
int disableFlashPatchLiteralComparator(int literalCOMPno)
{
  if(checkForValidLiteralComparator(literalCOMPno) == -1)
    return -1 ;
  
  memoryWriteWord((uint32_t)&(LITERAL_COMP[literalCOMPno]),FP_COMP_DISABLE);
  
  literalComparatorReady[literalCOMPno] = COMP_READY ;
  
  return 0;
}

/**
 *  Disable the instruction/literal comparator loaded with the selected address
 *   
 *  Input   : address is the address going to be checked inside the instruction/literal comparator
 *            comparatorType is used to select between instruction and literal comparator
 *				    Possible values : 
 *					    INSTRUCTION_TYPE			Instruction Comparator
 *					    LITERAL_TYPE			    Literal Comparator    
 *
 * Output  : return 1 if found
 *           return -1 if not found
 */
int disableFlashPatchComparatorLoadedWithAddress(uint32_t address,int comparatorType)
{
  uint32_t mask = 0x1FFFFFFC ;
  uint32_t dataRead = 0 ;
  uint32_t *compPtr ;
  int *compFlagPtr ;
  int i = 0 , max = 0 ;
  int (*funcPtr)(int) ;
  int found  =  -1;
  
  address = address & mask ;//mask off bit [31:29] and [1:0]
  
  if(comparatorType == INSTRUCTION_TYPE)
  {
    max = INSTRUCTION_COMP_NUM ;
    compPtr = (uint32_t *)&(INSTRUCTION_COMP[0]);
    compFlagPtr = &(instructionComparatorReady[0]);
    funcPtr = &(disableFlashPatchInstructionComparator);
  }
  else
  {
    max = LITERAL_COMP_NUM;
    compPtr = (uint32_t *)&(LITERAL_COMP[0]);
    compFlagPtr = &(literalComparatorReady[0]);
    funcPtr = &(disableFlashPatchLiteralComparator);
  }
  
  for(i = 0 ; i < max ; i ++ )
  {
    if(compFlagPtr[i] != COMP_READY)
    { 
      memoryReadWord((uint32_t)&(compPtr[i]),&dataRead);
      if((dataRead & mask) == address)
      {
        funcPtr(i);
        found = 1 ;
      }
    }
  }
  return found ;
}

/**
 *  Read all instruction/literal comparator and update their respective ready flag
 *   
 *  Input : comparatorType is used to select between instruction and literal comparator
 *				  Possible values : 
 *					  INSTRUCTION_TYPE			Instruction Comparator
 *					  LITERAL_TYPE			    Literal Comparator   
 */
void readAndUpdateComparatorReadyFlag(int comparatorType)
{
  uint32_t remapMask = 0xF0000000;
  uint32_t data = 0 ;
  uint32_t *compPtr ;
  int *compFlagPtr  ;
  int i = 0,max = 0 ;
  
  if(comparatorType == INSTRUCTION_TYPE)
  {
    max = INSTRUCTION_COMP_NUM;
    compPtr = (uint32_t *)&(INSTRUCTION_COMP[0]);
    compFlagPtr = &(instructionComparatorReady[0]);
  }
  else
  {
    max = LITERAL_COMP_NUM;
    compPtr = (uint32_t *)&(LITERAL_COMP[0]);
    compFlagPtr = &(literalComparatorReady[0]);
  }
 
  for(i = 0 ; i < max ; i ++)
  { 
      memoryReadWord((uint32_t)&(compPtr[i]),&data);
      if((data & FP_COMPARATOR_ENABLED_MASK))
      {
        if(!(data & remapMask))
          compFlagPtr[i] = COMP_REMAP ;
        else
          compFlagPtr[i] = COMP_BUSY ;
      }
      else
        compFlagPtr[i] = COMP_READY ;
  }
}

/**
 *  Use to select the next free/available instruction comparator to use
 *
 *  Output : return INSTRUCTION_COMP0 - INSTRUCTION_COMP5 if any one of them is free and INSTRUCTION_TYPE is chosen
 *           return LITERAL_COMP0     - LITERAL_COMP1 if any one of them is free and LITERAL_TYPE is chosen
 *           return -1 if all the comparators are busy
 *
 */
uint32_t selectNextFreeComparator(int comparatorType)
{
  int *compFlagPtr ;
  int nextComparator = 0 ;
  int max = 0 ;
  
  readAndUpdateComparatorReadyFlag(comparatorType);
  
  if(comparatorType == INSTRUCTION_TYPE)
  {
    max = INSTRUCTION_COMP_NUM;
    compFlagPtr = &(instructionComparatorReady[0]);
  }
  else
  {
    max = LITERAL_COMP_NUM;
    compFlagPtr = &(literalComparatorReady[0]);
  }
  
  for(nextComparator = 0 ; nextComparator < max ; nextComparator++)
  {
    if(compFlagPtr[nextComparator] == COMP_READY)
      return nextComparator;
  }

  return -1 ;
}

/**
 *  Disable all instruction and literal comparator in FPB unit
 *  
 */
void disableAllFlashPatchComparator()
{
  int i = 0 ;
  
  for (i=0 ; i < INSTRUCTION_COMP_NUM ; i++)
  {
    disableFlashPatchInstructionComparator(i);
    instructionComparatorReady[i] = COMP_READY;
  }
  
  for (i=0 ; i < LITERAL_COMP_NUM ; i++)
  {
    disableFlashPatchLiteralComparator(i);
    literalComparatorReady[i] = COMP_READY ;
  }
}

/**
 *  Initialise FPB unit by enabling FPB unit and disable all FP Comparator
 */
void initialiseFPBUnit()
{
  disableAllFlashPatchComparator();
  enableFPBUnit();
}

/**
 *  Disable all flash patch comparator set to breakpoint , flash patch remapping will not be affected
 *
 */
void disableAllFlashPatchComparatorSetToBreakpoint()
{
  int i = 0 ;
  for (i = 0 ; i < INSTRUCTION_COMP_NUM ; i ++)
  {
    if(instructionComparatorReady[i] == COMP_BUSY)
      disableFlashPatchInstructionComparator(i);
  }
}

/**
 * Stop all flash patch remapping , breakpoint will not be affected
 *
 */
void disableAllFlashPatchComparatorSetToRemap()
{
  int i = 0 ;
  
  for(i=0 ; i < INSTRUCTION_COMP_NUM ; i ++)
  {
    if(instructionComparatorReady[i] == COMP_REMAP)
      disableFlashPatchInstructionComparator(i);
  }
  
  for (i=0 ; i < LITERAL_COMP_NUM ; i++)
  {
    if(literalComparatorReady[i] == COMP_REMAP)
    disableFlashPatchLiteralComparator(i);
  }
}

/**
 *  Retrieve the breakpoint address set inside the selected comparator
 *
 *  Input : instructionCOMPno is the selected comparator which the address set will be retrieved
 *				  Possible values : 
 *					  INSTRUCTION_COMP0			Instruction comparator number 0	
 *					  INSTRUCTION_COMP1			Instruction comparator number 1	
 *					  INSTRUCTION_COMP2     Instruction comparator number 2
 *					  INSTRUCTION_COMP3     Instruction comparator number 3
 *					  INSTRUCTION_COMP4     Instruction comparator number 4
 *					  INSTRUCTION_COMP5     Instruction comparator number 5
 */
uint32_t retrieveBreakpointAddress(int instructionCOMPno)
{
  uint32_t dataRead = 0 ; 
  
  memoryReadWord((uint32_t)&(INSTRUCTION_COMP[instructionCOMPno]), &dataRead);

  if ((dataRead & FP_COMP_MATCH_MASK) == MATCH_UPPERHALFWORD)
    return (dataRead & FP_COMP_ADDRESS_MASK )+ 2;
  
  return dataRead & FP_COMP_ADDRESS_MASK;
}

/**
 *  Get all the address of the active breakpoint
 *  and rearrange the data in order to reduce the size to be send across later
 */
uint32_t * getAllActiveBreakpointAddress()
{
  uint32_t temp[INSTRUCTION_COMP_NUM] = {};
  static uint32_t address[INSTRUCTION_COMP_NUM] = {};
  int i = 0 , j = 0;
  
  for(i=0; i < INSTRUCTION_COMP_NUM ; i++)
  {
    if(instructionComparatorReady[i] == COMP_BUSY)
      temp[i] = retrieveBreakpointAddress(i);
  }
  
   for(i=0; i < INSTRUCTION_COMP_NUM ; i++)
  {
    if(temp[i] != 0)
    {
      address[j] = temp[i] ;
      j++ ;
    }
  }
  
  return address;
}