#ifndef DWT_Utilities_H
#define DWT_Utilities_H

#include <stdint.h>
#include "CoreDebug.h"
#include "CoreDebugEx.h"
#include "MemoryReadWrite.h"
#include "core_cm4.h"

typedef struct 
{
  __IO uint32_t COMP;                  
  __IO uint32_t MASK;              
  __IO uint32_t FUNCTION;      
       uint32_t RESERVED0[1];
}DWT_COMP_Type;
  
#define DWT_COMP            ((DWT_COMP_Type*)&(DWT->COMP0))
#define DWT_MATCHED_MASK		0x1000000

#define DISABLE_DWTCOMPARATOR 0

typedef enum 
{
	WATCHPOINT_READ = 5 ,
	WATCHPOINT_WRITE = 6 ,
	WATCHPOINT_READWRITE 
}Watchpoint_AccessMode ;


typedef enum
{
	WATCHPOINT_MASK_NOTHING = 0 ,
	WATCHPOINT_MASK_BIT0 ,
	WATCHPOINT_MASK_BIT1_BIT0,
	WATCHPOINT_MASK_BIT2_BIT0,
	WATCHPOINT_MASK_BIT3_BIT0,
	WATCHPOINT_MASK_BIT4_BIT0,
	WATCHPOINT_MASK_BIT5_BIT0,
	WATCHPOINT_MASK_BIT6_BIT0,
	WATCHPOINT_MASK_BIT7_BIT0,
	WATCHPOINT_MASK_BIT8_BIT0,
	WATCHPOINT_MASK_BIT9_BIT0,
	WATCHPOINT_MASK_BIT10_BIT0,
	WATCHPOINT_MASK_BIT11_BIT0,
	WATCHPOINT_MASK_BIT12_BIT0,
	WATCHPOINT_MASK_BIT13_BIT0,
	WATCHPOINT_MASK_BIT14_BIT0,
}Watchpoint_AddressMask;

typedef enum
{
	ADDRESS_COMPARISON = 0 ,
	DATA_COMPARISON ,
	CYCLECOUNT_COMPARISON
}Watchpoint_ComparisonMode;

typedef enum
{
	WATCHPOINT_BYTE = 0,
	WATCHPOINT_HALFWORD,
	WATCHPOINT_WORD
}Watchpoint_DataSize;


#endif // DWT_Utilities_H
