#include "unity.h"
#include <stdint.h>
#include "FPBUnit.h"
#include "FPBUnitEx.h"
#include "CoreDebug.h"
#include "CoreDebugEx.h"
#include "Emulator.h"
#include "MemoryReadWrite.h"
#include "Swd.h"
#include "IoOperations.h"
#include "mock_configurePort.h"
#include "mock_IoOperationsEx.h"

void setUp(void)
{
}

void tearDown(void)
{
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_READY ;
  instructionComparatorReady[2] = COMP_READY ;
  instructionComparatorReady[3] = COMP_READY ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_READY ;
  
  literalComparatorReady[0] = COMP_READY ;
  literalComparatorReady[1] = COMP_READY ;
}

/*-------------------------enableFPBUnit-----------------------*/
void test_enableFPBUnit_should_write_ENABLE_FPB_to_FP_CTRL()
{
  //Faking CSW to Word Size
  cswDataSize = CSW_WORD_SIZE ;
  
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_CTRL));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,ENABLE_FPB);

  enableFPBUnit();
}

/*-------------------------disableFPBUnit-----------------------*/
void test_disablePBUnit_should_write_DISABLE_FPB_to_FP_CTRL()
{
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_CTRL));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,DISABLE_FPB);
  
  disableFPBUnit();
}

/*-------------------------checkForValidInstructionComparator-----------------------*/
void test_checkForValidInstructionComparator_given_INSTRUCTION_COMP0_to_INSTRUCTION_COMP5_should_return_1()
{
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP0));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP1));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP2));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP3));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP4));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(INSTRUCTION_COMP5));

}

void test_checkForValidInstructionComparator_given_0_to_5_should_return_1()
{
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(0));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(1));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(2));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(3));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(4));
  TEST_ASSERT_EQUAL(1,checkForValidInstructionComparator(5));

}


void test_checkForValidInstructionComparator_given_unkown_value_should_return_minus_1()
{
  TEST_ASSERT_EQUAL(-1,checkForValidInstructionComparator(0x100));
  TEST_ASSERT_EQUAL(-1,checkForValidInstructionComparator(-1));
  TEST_ASSERT_EQUAL(-1,checkForValidInstructionComparator(-3));
  TEST_ASSERT_EQUAL(-1,checkForValidInstructionComparator(6));
}

/*-------------------------checkForValidLiteralComparator-----------------------*/
void test_checkForValidLiteralComparator_given_LITERAL_COMP0_to_LITERAL_COMP1_should_return_1()
{
  TEST_ASSERT_EQUAL(1,checkForValidLiteralComparator(LITERAL_COMP0));
  TEST_ASSERT_EQUAL(1,checkForValidLiteralComparator(LITERAL_COMP1));
}

void test_checkForValidLiteralComparator_given_0_to_1_should_return_1()
{
  TEST_ASSERT_EQUAL(1,checkForValidLiteralComparator(0));
  TEST_ASSERT_EQUAL(1,checkForValidLiteralComparator(1));
}

void test_checkForValidLiteralComparator_given_unkown_value_should_return_minus_1()
{
  TEST_ASSERT_EQUAL(-1,checkForValidLiteralComparator(0x100));
  TEST_ASSERT_EQUAL(-1,checkForValidLiteralComparator(-1));
  TEST_ASSERT_EQUAL(-1,checkForValidLiteralComparator(-3));
  TEST_ASSERT_EQUAL(-1,checkForValidLiteralComparator(3));
  
}

/*--------------------------swapHalfword-----------------------*/
void test_swapHalfword_given_0xAABBCCDD_should_return_0xCCDDAABB()
{
  TEST_ASSERT_EQUAL(0xCCDDAABB,swapHalfword(0xAABBCCDD));
}

void test_swapHalfword_given_0x1111AAAA_should_return_0xAAAA1111()
{
  TEST_ASSERT_EQUAL(0xAAAA1111,swapHalfword(0x1111AAAA));
}


/*-------------------------manualSetInstructionBreakpoint-----------------------*/
//MATCH_LOWERHALFWORD
void test_manualSetInstructionBreakpoint_givenINSTRUCTION_COMP1_address_0xFFFFFFFF_MATCH_LOWERHALFWORD()
{
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x5FFFFFFD);
  
  TEST_ASSERT_EQUAL(0,manualSetInstructionBreakpoint(INSTRUCTION_COMP1,0xFFFFFFFF,MATCH_LOWERHALFWORD));
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[1]);
}

//MATCH_UPPERHALFWORD
void test_manualSetInstructionBreakpoint_givenINSTRUCTION_COMP2_address_0xA55AA55A_MATCH_UPPERHALFWORD()
{
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x855AA559);
  
  TEST_ASSERT_EQUAL(0,manualSetInstructionBreakpoint(INSTRUCTION_COMP2,0xA55AA55A,MATCH_UPPERHALFWORD));
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[2]);
}

//MATCH_WORD
void test_manualSetInstructionBreakpoint_givenINSTRUCTION_COMP3_address_0x02345670_MATCH_WORD()
{
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0xC2345671);
  
  TEST_ASSERT_EQUAL(0,manualSetInstructionBreakpoint(INSTRUCTION_COMP3,0x02345670,MATCH_WORD));
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[3]);
}

void test_manualSetInstructionBreakpoint_given_out_of_range_instruction_comp_should_return_negative_1()
{
  TEST_ASSERT_EQUAL(-1,manualSetInstructionBreakpoint(10,0x02345670,MATCH_WORD));
}

/*-------------------------manualSetInstructionRemapping-----------------------*/
void test_manualSetInstructionRemapping_should_program_FP_REMAP_and_comparator()
{
  //Program FP_REMAP
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_REMAP));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x02222220);
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x02345671);
   
  TEST_ASSERT_EQUAL(0,manualSetInstructionRemapping(INSTRUCTION_COMP0,0x02345670,0x22222222));
  TEST_ASSERT_EQUAL(COMP_REMAP,instructionComparatorReady[0]);
}
/*-------------------------manualSetLiteralRemapping-----------------------*/
void test_manualSetLiteralRemapping_should_program_FP_REMAP_and_literal_Comparator()
{
  //Program FP_REMAP
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_REMAP));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x14567880);

  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x12345679);
  
  TEST_ASSERT_EQUAL(0,manualSetLiteralRemapping(LITERAL_COMP1,0x12345678,0x3456789A));
  TEST_ASSERT_EQUAL(COMP_REMAP,literalComparatorReady[1]);
}

/*-------------------------autoSetInstructionBreakpoint-----------------------*/
void test_autoSetInstructionBreakpoint_given_INSTRUCTION_COMP5_free_should_use_INSTRUCTION_COMP5()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x9FFFFFFD);
  
  TEST_ASSERT_EQUAL(5,autoSetInstructionBreakpoint(0xFFFFFFFF));
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[5]);
  
}

/*-------------------------autoSetInstructionRemapping-----------------------*/
void test_autoSetInstructionRemapping_should_program_machineCode_to_REMAP_BASE_given_COMP0_free()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Program machine code
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,REMAP_BASE);
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x56781234);
  
  //Program FP_REMAP
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_REMAP));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,REMAP_BASE & FP_REMAP_ADDRESS_MASK);
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x08000001);

  TEST_ASSERT_EQUAL(INSTRUCTION_COMP0,autoSetInstructionRemapping(0x08000000,0x12345678));
  TEST_ASSERT_EQUAL(COMP_REMAP,instructionComparatorReady[0]);
}

void test_autoSetInstructionRemapping_16bit_machineCode_case()
{
  cswDataSize = CSW_WORD_SIZE;
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Set CSW to Halfword Size
	emulateSwdRegisterWrite(SELECT_REG, SWD_DP, OK, SELECT_BANK0);
	emulateSwdRegisterWrite(CSW_REG, SWD_AP, OK, (CSW_DEFAULT_MASK | CSW_HALFWORD_SIZE | CSW_ENABLE_ADDR_INC_PACKED));
  
  //Read Instruction Address + 2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, 0x08000002);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0xBBBB1234));

  //Set CSW to Word Size
	emulateSwdRegisterWrite(SELECT_REG, SWD_DP, OK, SELECT_BANK0);
	emulateSwdRegisterWrite(CSW_REG, SWD_AP, OK, (CSW_DEFAULT_MASK | CSW_WORD_SIZE));

  //Program machine code
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,REMAP_BASE);
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0xBBBBAAAA);
  
  //Program FP_REMAP
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_REMAP));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,REMAP_BASE & FP_REMAP_ADDRESS_MASK);
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x08000001);

  TEST_ASSERT_EQUAL(INSTRUCTION_COMP0,autoSetInstructionRemapping(0x08000000,0xAAAA));
  TEST_ASSERT_EQUAL(COMP_REMAP,instructionComparatorReady[0]);
}

/*-------------------------autoSetLiteralRemapping-----------------------*/
void test_autoSetLiteral_should_return_negative_1_if_all_comp_are_busy()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  TEST_ASSERT_EQUAL(-1,autoSetLiteralRemapping(0x08000000,0x12345678));
}

void test_autoSetLiteralRemapping_given_LITERAL_COMP1_free_should_program_literalData_to_REMAP_BASE()
{
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0));
  
  //Program Literal Data
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,REMAP_BASE + 28);
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x12345678);
 
  //Program FP_REMAP
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_REMAP));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,REMAP_BASE & FP_REMAP_ADDRESS_MASK);
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,0x08000001);
  
 
  TEST_ASSERT_EQUAL(LITERAL_COMP1,autoSetLiteralRemapping(0x08000000,0x12345678));
  TEST_ASSERT_EQUAL(COMP_REMAP,literalComparatorReady[1]);
}

/*-------------------------disableFlashPatchInstructionComparator-----------------------*/
void test_disableInstructionComparator_should_write_FP_COMP_DISABLE_to_the_selected_instruction_comparator()
{
  cswDataSize = CSW_WORD_SIZE ;
  
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  TEST_ASSERT_EQUAL(0,disableFlashPatchInstructionComparator(INSTRUCTION_COMP0));
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[0]);
}

void test_disableInstructionComparator_should_return_negative_1_given_unkwown_comp()
{
  TEST_ASSERT_EQUAL(-1,disableFlashPatchInstructionComparator(100));
}


/*-------------------------disableFlashPatchLiteralComparator-----------------------*/
void test_disableLiteralComparator_should_write_FP_COMP_DISABLE_to_the_selected_literal_comparator()
{
  //Program comparator
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  TEST_ASSERT_EQUAL(0,disableFlashPatchLiteralComparator(LITERAL_COMP1));
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[1]);
}

void test_disableLiteralComparator_should_return_negative_1_given_unkwown_comp()
{
  TEST_ASSERT_EQUAL(-1,disableFlashPatchLiteralComparator(100));
}

/*-------------------------disableFlashPatchComparatorLoadedWithAddress-----------------------*/
//INSTRUCTION_TYPE
void test_disableFPComparatorLoadedWithAddress_given_found_comp3_should_disable_comp3()
{
  instructionComparatorReady[0] = COMP_BUSY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_BUSY ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_BUSY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x88884444));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0xBEEFBEEF));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x23456789));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0xBEEFBEEF));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0xBEEFBEEF));
  
  disableFlashPatchComparatorLoadedWithAddress(0x12345678,INSTRUCTION_TYPE);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[3]);
  
}

void test_disableFPComparatorLoadedWithAddress_given_all_loaded_with_same_address_should_disable_all()
{
  instructionComparatorReady[0] = COMP_BUSY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_BUSY ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_BUSY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  disableFlashPatchComparatorLoadedWithAddress(0x12345678,INSTRUCTION_TYPE);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[1]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[2]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[3]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[4]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[5]);
}

void test_disableFPComparatorLoadedWithAddress_given_all_comparator_free_should_do_nothing()
{
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_READY ;
  instructionComparatorReady[2] = COMP_READY ;
  instructionComparatorReady[3] = COMP_READY ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_READY ;
  
  disableFlashPatchComparatorLoadedWithAddress(0,INSTRUCTION_TYPE);
}

void test_disableFPComparatorLoadedWithAddress_given_non_match_should_do_nothing()
{
  instructionComparatorReady[0] = COMP_BUSY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_BUSY ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_BUSY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  disableFlashPatchComparatorLoadedWithAddress(0xFFFFFFFF,INSTRUCTION_TYPE);
  
}

//LITERAL_TYPE
void test_disableFPComparatorLoadedWithAddress_given_found_literal_comp1_should_disable_comp1()
{

  literalComparatorReady[0] = COMP_BUSY ;
  literalComparatorReady[1] = COMP_BUSY ;
  
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x88884444));
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x12345678));
  
  //Disable LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  disableFlashPatchComparatorLoadedWithAddress(0x12345678,LITERAL_TYPE);
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[1]);
  
}

/*-------------------------disableAllFPBComparator-----------------------*/
void test_disableAllFPBComparator_should_disable_all_comparator_and_set_all_ready_flag_to_COMP_READY()
{
  instructionComparatorReady[0] = COMP_BUSY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_BUSY ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_BUSY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  literalComparatorReady[0] = COMP_BUSY ;
  literalComparatorReady[1] = COMP_BUSY ;
  
  //Disable INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  disableAllFlashPatchComparator();
  
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[1]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[2]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[3]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[4]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[5]);
  
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[1]);
}

/*-------------------------readAndUpdateComparatorReadyFlag-----------------------*/
//INSTRUCTION_TYPE
void test_readAndUpdateComparatorReadyFlag_should_read_all_Instruction_Comparator_and_update_readyFlag()
{
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_READY ;
  instructionComparatorReady[2] = COMP_READY ;
  instructionComparatorReady[3] = COMP_READY ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_READY ;
  
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x10000001));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0xF0000001));

  readAndUpdateComparatorReadyFlag(INSTRUCTION_TYPE);
  
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[1]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[2]);
  TEST_ASSERT_EQUAL(COMP_REMAP,instructionComparatorReady[3]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[4]);
  TEST_ASSERT_EQUAL(COMP_BUSY,instructionComparatorReady[5]);
}

//Literal_Type
void test_readAndUpdateComparatorReadyFlag_should_read_all_Literal_Comparator_and_update_readyFlag()
{
  literalComparatorReady[0] = COMP_READY ;
  literalComparatorReady[1] = COMP_READY ;
  
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x20000001));
  
  readAndUpdateComparatorReadyFlag(LITERAL_TYPE);
  
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_BUSY,literalComparatorReady[1]);
}

/*--------------------------selectNextFreeComparator-----------------------*/
//INSTRUCTION_TYPE
void test_selectNextFreeInstructionComparator_given_all_free_should_return_INSTRUCTION_COMP0()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);

  
  TEST_ASSERT_EQUAL(INSTRUCTION_COMP0,selectNextFreeComparator(INSTRUCTION_TYPE));
}

void test_selectNextFreeInstructionComparator_given_COMP0_busy_should_return_INSTRUCTION_COMP1()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  TEST_ASSERT_EQUAL(INSTRUCTION_COMP1,selectNextFreeComparator(INSTRUCTION_TYPE));
}

void test_selectNextFreeInstructionComparator_given_all_busy_should_return_negative_1()
{
  //Read INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  //Read INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x1));
  
  TEST_ASSERT_EQUAL(-1,selectNextFreeComparator(INSTRUCTION_TYPE));
}

//LITERAL_TYPE
void test_selectNextFreeInstructionComparator_given_LiteralComp0_ready_should_return_LiteralComp0()
{
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x20000001));
  
  
  TEST_ASSERT_EQUAL(LITERAL_COMP0,selectNextFreeComparator(LITERAL_TYPE));
}

void test_selectNextFreeInstructionComparator_given_LiteralComp1_ready_should_return_LiteralComp1()
{
  literalComparatorReady[0] = COMP_BUSY;
  literalComparatorReady[1] = COMP_READY;
  
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x20000001));
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
  
  TEST_ASSERT_EQUAL(LITERAL_COMP1,selectNextFreeComparator(LITERAL_TYPE));
}

void test_selectNextFreeInstructionComparator_given_all_literal_COMP_busy_should_return_neg_1()
{
  literalComparatorReady[0] = COMP_BUSY;
  literalComparatorReady[1] = COMP_BUSY;
  
  //Read LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x20000001));
  
  //Read LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x20000001));
  
  TEST_ASSERT_EQUAL(-1,selectNextFreeComparator(LITERAL_TYPE));
}

/*-------------------------initialiseFPBUnit-----------------------*/
void test_initialiseFPBUnit_should_disable_all_comparator_and_enable_FPB_unit()
{
  //Disable INSTRUCTION_COMP0
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP4
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[4]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable LITERAL_COMP0
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[0]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Enable FPB Unit
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(FPB->FP_CTRL));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,ENABLE_FPB);
  
  initialiseFPBUnit();
  
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[1]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[2]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[3]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[4]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[5]);
  
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[0]);
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[1]);
}

/*-------------------------disableAllFlashPatchComparatorSetToBreakpoint-----------------------*/
void test_removeAllFPComparatorSetToBreakpoint_should_only_disable_comparator_set_to_COMP_BUSY()
{
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_REMAP ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  //Disable INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable INSTRUCTION_COMP3
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[3]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
 
  //Disable INSTRUCTION_COMP5
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[5]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  disableAllFlashPatchComparatorSetToBreakpoint();
  
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[1]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[3]);
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[5]);
}

/*-------------------------disableAllFlashPatchComparatorSetToRemap-----------------------*/
void test_stopAllFlashPatchRemapping_should_only_disable_comparator_set_to_COMP_REMAP()
{
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_REMAP ;
  instructionComparatorReady[3] = COMP_BUSY ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_BUSY ;
  
  literalComparatorReady[0] = COMP_READY ;
  literalComparatorReady[1] = COMP_REMAP ;
  
  //Disable INSTRUCTION_COMP2
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  //Disable LITERAL_COMP1
  emulateSwdRegisterWrite(TAR_REG,SWD_AP,4,(uint32_t)&(LITERAL_COMP[1]));
	emulateSwdRegisterWrite(DRW_REG,SWD_AP,4,FP_COMP_DISABLE);
  
  disableAllFlashPatchComparatorSetToRemap();
  
  TEST_ASSERT_EQUAL(COMP_READY,instructionComparatorReady[2]);
  TEST_ASSERT_EQUAL(COMP_READY,literalComparatorReady[1]);
}

/*-------------------------retrieveBreakpointAddress-----------------------*/
void test_retrieveBreakpointAddress_given_instructionCOMP1_contains_0x9ABC883D_should_return_0x1ABC883E()
{
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x9ABC883D));
  
  TEST_ASSERT_EQUAL(0x1ABC883E,retrieveBreakpointAddress(INSTRUCTION_COMP1));
}

void test_retrieveBreakpointAddress_given_instructionCOMP1_contains_0x5ABC883D_should_return_0x1ABC883C()
{
  //Read INSTRUCTION_COMP1
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x5ABC883D));
  
  TEST_ASSERT_EQUAL(0x1ABC883C,retrieveBreakpointAddress(INSTRUCTION_COMP1));
}

/*-------------------------getAllActiveBreakpointAddress-----------------------*/
void test_getAllActiveBreakpointAddress_given_comp1_and_comp2_busy_should_read_comp1_and_comp2()
{
  uint32_t *address;
  
  instructionComparatorReady[0] = COMP_READY ;
  instructionComparatorReady[1] = COMP_BUSY ;
  instructionComparatorReady[2] = COMP_BUSY ;
  instructionComparatorReady[3] = COMP_REMAP ;
  instructionComparatorReady[4] = COMP_READY ;
  instructionComparatorReady[5] = COMP_READY ;
  
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[1]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x9ABC883D));
  
  emulateSwdRegisterWrite(TAR_REG, SWD_AP, OK, (uint32_t)&(INSTRUCTION_COMP[2]));
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, 0);
	emulateSwdRegisterRead(DRW_REG, SWD_AP, OK, 1, interconvertMSBandLSB(0x5ABC883D));
  
  
  address = getAllActiveBreakpointAddress();
  TEST_ASSERT_EQUAL(0,address[0]);
  TEST_ASSERT_EQUAL(0x1ABC883E,address[1]);
  TEST_ASSERT_EQUAL(0x1ABC883C,address[2]);
}