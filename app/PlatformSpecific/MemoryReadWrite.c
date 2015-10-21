#include "MemoryReadWrite.h"

int cswDataSize = -1;

/* Use for mocking purpose in test_TLV_Probe */
uint32_t memoryReadAndReturnWord(uint32_t address) {
  uint32_t dataRead = 0;
  
  memoryReadWord(address, &dataRead);
  return dataRead;
}

int memoryReadWord(uint32_t address, uint32_t *dataRead)
{
	int parity = 0 , status = 0;
	
	if(cswDataSize != CSW_WORD_SIZE) // used to prevent setting same size again and again
  {
    swdSelectMemorySize((CSW_DEFAULT_MASK | CSW_WORD_SIZE));
    cswDataSize = CSW_WORD_SIZE;
  }

	swdWriteAP(TAR_REG, address);
	swdReadAP(DRW_REG, dataRead);
	
	status = compare_ParityWithData(*dataRead,parity);
	
	return status;
}

int memoryReadHalfword(uint32_t address,uint32_t *dataRead)
{
  int parity = 0 , status = 0;
	
	if(cswDataSize != CSW_HALFWORD_SIZE) // used to prevent setting same size again and again
	  {
	    swdSelectMemorySize((CSW_DEFAULT_MASK | CSW_HALFWORD_SIZE));
	    cswDataSize = CSW_HALFWORD_SIZE;
	  }

	swdWriteAP(TAR_REG, address);
	swdReadAP(DRW_REG, dataRead);
	
	status = compare_ParityWithData(*dataRead,parity);
	
	return status;
}

SwdError memoryWriteByte(uint32_t address, uint8_t writeData)
{
  int error = 0;
  uint32_t alignedData = 0;
  
  if(cswDataSize != CSW_BYTE_SIZE) // used to prevent setting same size again and again
  {  
    swdSelectMemorySize((CSW_DEFAULT_MASK | CSW_BYTE_SIZE));
    cswDataSize = CSW_BYTE_SIZE;
  }
  alignedData = memoryWriteDataAlignment(address,writeData);
  
  error = swdWriteAP(TAR_REG, address);
  if(error != NO_ERROR) return error;
  
  error = swdWriteAP(DRW_REG, alignedData);
  return error;
}

SwdError memoryWriteHalfword(uint32_t address, uint16_t writeData)
{
  int error = 0;
  uint32_t alignedData = 0;
  
  if(cswDataSize != CSW_HALFWORD_SIZE) // used to prevent setting same size again and again
  {  
    swdSelectMemorySize((CSW_DEFAULT_MASK | CSW_HALFWORD_SIZE));
    cswDataSize = CSW_HALFWORD_SIZE;
  }
  
  alignedData = memoryWriteDataAlignment(address,writeData);
  
  error = swdWriteAP(TAR_REG, address);
  if(error != NO_ERROR) return error;
  
  error = swdWriteAP(DRW_REG, alignedData);
  return error;
}

SwdError memoryWriteWord(uint32_t address, uint32_t writeData)
{
  int error = 0;
  
  if(cswDataSize != CSW_WORD_SIZE) // used to prevent setting same size again and again
  {  
    swdSelectMemorySize((CSW_DEFAULT_MASK | CSW_WORD_SIZE));
    cswDataSize = CSW_WORD_SIZE;
  }
 
  error = swdWriteAP(TAR_REG, address);
  if(error != NO_ERROR) return error;
  
  error = swdWriteAP(DRW_REG, writeData);
  return error;
}

/**
 * Perform data alignment when performing byte or halfword memory write access
 *
 *  Input : address is the address where the data is going to be written into
 *          writeData is the data going to be written into the address
 *
 *  Output : return 32bits of aligned data
 */
uint32_t memoryWriteDataAlignment(uint32_t address,uint16_t writeData)
{ 
  if(address % 4 == 0x1)
    return writeData << 8 ;
  if(address % 4 == 0x2)
    return writeData << 16 ;
  if(address % 4 == 0x3)
    return writeData << 24 ;
  
  return writeData ;
}