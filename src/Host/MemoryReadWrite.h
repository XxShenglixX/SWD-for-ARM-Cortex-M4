#ifndef MemoryReadWrite_H
#define MemoryReadWrite_H

#include <stdint.h>
#include "Yield.h"
#include "CException.h"
#include "ProgramLoader.h"

extern Tlv_Session *_session;

#define BYTE_SIZE           1
#define HALFWORD_SIZE       2
#define WORD_SIZE           4

#define memoryReadByte(address, dataRead)         memoryRead(address, dataRead, BYTE_SIZE       )
#define memoryReadHalfword(address, dataRead)     memoryRead(address, dataRead, HALFWORD_SIZE   )
#define memoryReadWord(address, dataRead)         memoryRead(address, dataRead, WORD_SIZE       )

#define memoryWriteByte(address, dataWrite)       memoryWrite(address, dataWrite, BYTE_SIZE     )
#define memoryWriteHalfword(address, dataWrite)   memoryWrite(address, dataWrite, HALFWORD_SIZE )
#define memoryWriteWord(address, dataWrite)       memoryWrite(address, dataWrite, WORD_SIZE     )

/* Used for mocking */
uint32_t memoryReadAndReturnWord(uint32_t address);

/* Initialize memory read write by creating tlv session */
void initMemoryReadWrite(void);
int memoryRead(uint32_t address, uint32_t *dataRead, int size);
int memoryWrite(uint32_t address, uint32_t dataWrite, int size);

int _flashWriteWord(uint32_t address, uint32_t writeData);

uint32_t memoryWriteDataAlignment(uint32_t address, uint16_t writeData);

#endif // MemoryReadWrite_H
