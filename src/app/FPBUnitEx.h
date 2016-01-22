#ifndef FPB_Utilities_H
#define FPB_Utilities_H

#include <stdint.h>
#include "FPBUnit.h"
#include "MemoryReadWrite.h"
#include "core_cm4.h"

#define SOFTBKPT_BUFFER_SIZE  100

typedef struct
{
  __IO uint32_t  FP_CTRL;
  __IO uint32_t  FP_REMAP;
  __IO uint32_t  FP_COMP0;
  __IO uint32_t  FP_COMP1;
  __IO uint32_t  FP_COMP2;
  __IO uint32_t  FP_COMP3;
  __IO uint32_t  FP_COMP4;
  __IO uint32_t  FP_COMP5;
  __IO uint32_t  FP_COMP6;
  __IO uint32_t  FP_COMP7;
}FPB_Type;

typedef struct
{
  __IO uint32_t COMP ;
}FP_COMP_Type;

#define FPB_BASE      (0xE0002000UL)
#define FPB           ((FPB_Type*)FPB_BASE)

#define INSTRUCTION_COMP            ((FP_COMP_Type*)&(FPB->FP_COMP0))
#define LITERAL_COMP                ((FP_COMP_Type*)&(FPB->FP_COMP6))

#define INSTRUCTION_COMP_NUM    6
#define LITERAL_COMP_NUM        2

#define INSTRUCTION_COMP0	0
#define INSTRUCTION_COMP1	1
#define INSTRUCTION_COMP2	2
#define INSTRUCTION_COMP3	3
#define INSTRUCTION_COMP4	4
#define INSTRUCTION_COMP5	5

#define LITERAL_COMP0	0
#define LITERAL_COMP1	1

#define MATCH_REMAP					  0x00000000
#define MATCH_LOWERHALFWORD	 	0x40000000
#define MATCH_UPPERHALFWORD		0x80000000
#define MATCH_WORD				    0xC0000000

#define FP_COMP_ENABLE	        0x1
#define FP_COMP_DISABLE         0
      
#define FP_COMP_ADDRESS_MASK		0x1FFFFFFC
#define FP_COMP_MATCH_MASK			0xC0000000
#define FP_REMAP_ADDRESS_MASK		0x1FFFFFE0

#define FPB_ENABLED_MASK 			      0x1
#define FP_COMPARATOR_ENABLED_MASK  0x1

#define DISABLE_FPB 0x2
#define ENABLE_FPB 	0x3

#define COMP_READY  0
#define COMP_BUSY   1
#define COMP_REMAP  2

#define LITERAL_TYPE      0
#define INSTRUCTION_TYPE  1

#define REMAP_BASE        0x30000000

#define LOWERHALFWORD_MASK  0x0000FFFF
#define UPPERHALFWORD_MASK  0xFFFF0000

#define UPPERMATCHINGMODE_MASK  0x2

extern int instructionComparatorReady[INSTRUCTION_COMP_NUM];
extern int literalComparatorReady[LITERAL_COMP_NUM];

#endif // FPB_Utilities_H
