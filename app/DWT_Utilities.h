#ifndef DWT_Utilities_H
#define DWT_Utilities_H

#include <stdint.h>
#include "Misc_Utilities.h"

#define DWT_CTRL		0xE0001000
#define DWT_CYCCNT		0xE0001004
#define DWT_CPICNT		0xE0001008
#define DWT_EXCCNT		0xE000100C
#define DWT_SLEEPCNT	0xE0001010
#define DWT_LSUCNT		0xE0001014
#define DWT_FOLDCNT		0xE0001018
#define DWT_PCSR		0xE000101C

#define DWT_COMP0 	0xE0001020
#define DWT_MASK0 	0xE0001024
#define DWT_FUNC0 	0xE0001028

#define DWT_COMP1 	0xE0001030
#define DWT_MASK1 	0xE0001034
#define DWT_FUNC1	0xE0001038

#define DWT_COMP2 	0xE0001040
#define DWT_MASK2 	0xE0001044
#define DWT_FUNC2	0xE0001048

#define DWT_COMP3 	0xE0001050
#define DWT_MASK3 	0xE0001054
#define DWT_FUNC3	0xE0001058

#define NOT_SUPPORTED 	0
#define SUPPORTED 		1

#define Enable 1 
#define Disable 0

typedef struct DWT_Info DWT_Info ;
typedef struct DWT_ComparatorInfo DWT_ComparatorInfo ;
typedef struct DWT_FunctionInfo DWT_FunctionInfo ;
typedef struct DWT_Control DWT_Control;
typedef struct DWT_32bitCounter DWT_32bitCounter ;
typedef struct DWT_8bitCounter DWT_8bitCounter ;
typedef struct EventStatus EventStatus;

typedef enum
{
	Ignore_Non = 0 ,
	Ignore_Bit0 ,
	Ignore_Bit1_Bit0,
	Ignore_Bit2_Bit0,
	Ignore_Bit3_Bit0,
	Ignore_Bit4_Bit0,
	Ignore_Bit5_Bit0,
	Ignore_Bit6_Bit0,
	Ignore_Bit7_Bit0,
	Ignore_Bit8_Bit0,
	Ignore_Bit9_Bit0,
	Ignore_Bit10_Bit0,
	Ignore_Bit11_Bit0,
	Ignore_Bit12_Bit0,
	Ignore_Bit13_Bit0,
	Ignore_Bit14_Bit0,
}IgnoreMask;

typedef enum
{
	DWTFunction_Disabled = 0 ,
	ITM_SampleEmit_PC = 1 ,
	ITM_Emit_AddressOffset = 100,
	ITM_Emit_DataRW = 2,
	ITM_Emit_Data_AddressOffset_RW = 200 ,
	ITM_Sample_PC_DataValue_RW = 3,
	ITM_Emit_AddressOffset_DataValue_RW = 300,
	Watchpoint_PCMatch  = 4,
	Watchpoint_DataRead = 5,
	Watchpoint_DataWrite = 6,
	Watchpoint_DataReadWrite = 7,
	ETM_Trigger_PCMatch = 8,
	ETM_Trigger_DataRead = 9,
	ETM_Trigger_DataWrite = 10,
	ETM_TriggerDataReadWrite = 11,
	Sample_DataRead =12,
	Sample_DataAddressOffset_Read=1200,
	Sample_DataWrite =13,
	Sample_DataAddressOffset_Write=1300,
	Sample_PC_Data_Read =14,
	Sample_DataAddressOffset_Data_Read=1400,
	Sample_PC_Data_Write =15,
	Sample_DataAddressOffset_Data_Write=1500,
}DWTFunction;

typedef enum
{
	Address_Comparison = 0 ,
	DataValue_Comparaison ,
	CycleCount_Comparaison
}ComparisonMode;

typedef enum
{
	Tap_Disabled = 0,
	Tap_CYCCNTbit24 ,
	Tap_CYCCNTbit26 ,
	Tap_CYCCNTbit28 ,
}SyncTap;

typedef enum
{
	Byte = 0,
	Halfword,
	Word  
}DATAVSIZE;

struct DWT_Info
{
	DWT_Control *dwtControl ;
	
	DWT_32bitCounter *CycleCount ;
	DWT_8bitCounter *CPICount ;
	DWT_8bitCounter *ExceptionOverheadCount ;
	DWT_8bitCounter *SleepCount ;
	DWT_8bitCounter *LSUCount ;
	DWT_8bitCounter *FoldedInstructionCount ;
	DWT_32bitCounter *PCSample ;
	
	DWT_ComparatorInfo *dwtCompInfo[4] ;	
};

struct EventStatus
{
	int CycleCountEvent ;
	int FoldedInstructionCount ;
	int LSUCount ;
	int SleepCount ;
	int ExceptionOverheadCount ;
	int CPICount ;
	int InterruptTracing ;
	int PCSampling ;
};

struct DWT_8bitCounter
{
	int previousValue ;
	int currentValue ;
};


struct DWT_32bitCounter
{
	uint32_t previousValue ;
	uint32_t currentValue ;
};

struct DWT_Control 
{
	int numberComparator;
	int support_Tracing;
	int support_ExternalTrigger;
	int support_CycleCount ;
	int support_ProfilingCount ;
	EventStatus *eventStatus ;
	SyncTap syncTap ;
	int cycTap ;
	int postCnt;
	int postReset;
	int EnableDisable_CycleCountCounter;
};

struct DWT_FunctionInfo
{
	int MATCHED ;
	int DATAVADDR1;
	int DATAVADDR0 ;
	DATAVSIZE dataSize ;
	int LNK1ENA ;
	int DATAVMATCH ;
	int CYCMATCH ;
	int EMITRANGE ;
	DWTFunction dwtFunction ;
};

struct DWT_ComparatorInfo
{
	uint32_t address ;
	IgnoreMask ignoreMask ;
	DWT_FunctionInfo *dwtFunctionInfo ;
};

void init_DWTInfo(DWT_Info *dwtInfo);
void init_DWTControl(DWT_Control *dwtControl);
void init_EventStatus(EventStatus *eventStatus);
void init_DWT8bitCounter(DWT_8bitCounter *counter);
void init_DWT32bitCounter(DWT_32bitCounter *counter);
void init_DWTComparatorInfo(DWT_ComparatorInfo *dwtCompInfo,DWT_FunctionInfo *dwtFunction);
void init_DWTFunctionInfo(DWT_FunctionInfo *dwtFunctionInfo);


void EnableDisable_EventStatus(EventStatus *eventStatus,int CYCEVTENA,int FOLDEVTENA,int LSUEVTENA,int SLEEPEVTENA,int EXCEVTENA,int CPIEVTENA,int EXCTRCENA,int PCSAMPLEENA);

void update_DWTControl(DWT_Control *dwtControl,uint32_t dataRead);
void update_EventStatus(EventStatus *eventStatus,uint32_t dataRead);
void update_8bitCounter(DWT_8bitCounter *counter,uint32_t dataRead);
void update_32bitCounter(DWT_32bitCounter *counter,uint32_t dataRead);
void update_DWTComparatorInfo(DWT_ComparatorInfo *dwtCompInfo,uint32_t dataRead,uint32_t maskRead);
void update_DWTFunctionInfo(DWT_FunctionInfo *dwtFunctionInfo,uint32_t dataRead);



uint32_t get_DWTMask_WriteValue(IgnoreMask mask);
uint32_t get_DWTFunction_WriteValue(int firstLinkComp,int secondLinkComp,ComparisonMode mode,int EnableDisable_DataTrace,DWTFunction function);
uint32_t get_DWTControl_WriteValue(EventStatus eventStatus,int cycTap,int posCnt,int postReset,int EnableDisable_CycleCountCounter);


#endif // DWT_Utilities_H