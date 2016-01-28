#include "ProgramLoader.h"

static int programSize = 0;
static Tlv *response;
static User_Session *userSession;
static uint32_t FLASH_BEGIN_ADDRESS = 0x08000000;

/** writeRegister is a function to write data into target register
  *
  * input   : session contain a element/handler used by tlv protocol
  *           registerAddress is the address of the target register
  *           data is the data need to write into the target register
  *
  * return  : 1 successfully write into target register
  *           0 waiting reply from target
  */
Process_Status writeRegister(Tlv_Session *session, uint32_t registerAddress, uint32_t data) {
  uint32_t dataBuffer[] = {registerAddress, data};
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_WRITE_REGISTER, 8, (uint8_t *)dataBuffer);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** readRegister is a function to read target register
  *
  * input   : session contain a element/handler used by tlv protocol
  *           registerAddress is the address of the target register
  *
  * return  : register value if successfully read from probe
  *           0 if waiting reply from probe
  */
uint32_t readRegister(Tlv_Session *session, uint32_t registerAddress) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_READ_REGISTER, 4, (uint8_t *)&registerAddress);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  #ifdef HOST
  printf("value %x\n", get4Byte(&response->value[0]));
  #endif

  returnThis(get4Byte(&response->value[0]));
}

/** halt is a function halt target processor
  *
  * input   : session contain a element/handler used by tlv protocol
  *           registerAddress is the address of the target register
  *
  * return  : 1 if successfully halted
  *           0 if waiting reply from probe
  */
Process_Status halt(Tlv_Session *session) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_HALT_TARGET, 0, NULL);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);


  returnThis(PROCESS_DONE);
}

/** run is a function run target processor
  *
  * input   : session contain a element/handler used by tlv protocol
  *           registerAddress is the address of the target register
  *
  * return  : 1 if successfully run
  *           0 if waiting reply from probe
  */
Process_Status run(Tlv_Session *session) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;
  uint32_t data = 0 ;
  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_RUN_TARGET, 0, NULL);
  previousTime = getSystemTime();
  /* Waiting reply from probe */
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);

  data = get4Byte(&response->value[0]) ;
  #ifdef HOST
  if(data != 0)
    printf("PC stops at %x\n", get4Byte(&response->value[0]));
  #endif
  
  returnThis(PROCESS_DONE);
}

/** multipleStep is a function send tlv request to
  * execute number of instructions
  *
  * input   : session contain a element/handler used by tlv protocol
  *           nInstructions is the number of instruction need to execute
  *
  * return  : Current program counter address if successfully step
  *           0 if waiting reply from probe
  */
uint32_t multipleStep(Tlv_Session *session, int nInstructions) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_STEP, 4, (uint8_t *)&nInstructions);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);
  #ifdef HOST
  printf("value %x\n", get4Byte(&response->value[0]));
  #endif

  returnThis(get4Byte(&response->value[0]));
}

/** stepOver is a function send tlv request to
  * step over an instruction
  *
  * input   : session contain a element/handler used by tlv protocol
  *
  * return  : Current program counter address if successfully step
  *           0 if waiting reply from probe
  */
uint32_t stepOver(Tlv_Session *session) 
{
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_STEPOVER, 0, NULL);

  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND,previousTime), tb);
    yield(tb);
  };

  /* End tlv request task */
  endTask(tb);
  #ifdef HOST
  printf("value %x\n", get4Byte(&response->value[0]));
  #endif
  returnThis(get4Byte(&response->value[0]));
}

/** softReset is a function software reset target
  *
  * Input   : session contain a element/handler used by tlv protocol
  *
  * Return  : 1 if successfully software reset
  *           0 if waiting reply from probe
  */
Process_Status softReset(Tlv_Session *session) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_SOFT_RESET, 0, NULL);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** hardReset is a function hardware reset target
  *
  * Input   : session contain a element/handler used by tlv protocol
  *
  * Return  : 1 if successfully software reset
  *           0 if waiting reply from probe
  */
Process_Status hardReset(Tlv_Session *session) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_HARD_RESET, 0, NULL);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** vectorReset is a function vector reset target
  *
  * Input   : session contain a element/handler used by tlv protocol
  *
  * Return  : 1 if successfully software reset
  *           0 if waiting reply from probe
  */
Process_Status vectorReset(Tlv_Session *session) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_VECT_RESET, 0, NULL);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End tlv request task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** readMemory is a function to read data from target memory
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           address is the address of the data need to be read
  *           size is the size of the data can be any value
  *
  * Return  : s is a Storage structure pointer contain read data
  */
uint8_t *readMemory(Tlv_Session *session, uint32_t address, int size) {
  static TaskBlock taskBlock = {.state = 0};
  static uint8_t *db;
  static uint32_t tAddress = 0, previousTime = 0;
  static int index, tSize = 0;
  TaskBlock *tb = &taskBlock;
  int i;

  if(session == NULL) Throw(TLV_NULL_SESSION);
  /* Start tlv request task */
  startTask(tb);

  /* Create temporary storage to store neccessary data
     to prevent lose track */
  index = 0;
  db = createDataBlock(size);
  tSize = size;
  tAddress = address;

  while(tSize > 0) {
    /* Send tlv request */
    tlvReadDataChunk(session, &tAddress, &tSize);
    /* Get timestamp to verify if the timeout is occur */
    previousTime = getSystemTime();
    while((response = tlvReceive(session)) == NULL) {
      /* Check is maximum timeout is reached */
      isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
      yield(tb);
    };
    /* Waiting reply from probe */
    for(i = 0; i < response->length - 1; i++) {
      db[index] = response->value[i];
      index++;
    }
  }
  /* End task */
  endTask(tb);
  #ifdef HOST
  displayMemoryMap(db, address, size);
  #endif

  returnThis(db);
}

/** writeMemory is a function used to write data into target memory
  * by using tlv protocol
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           dataAddress is the address of the data need to send
  *           destAddress is the address of the data need to be store
  *           size is the size of the data can be any value
  *           memory can be one of the following value :
  *            - TLV_WRITE_RAM
  *            - TLV_WRITE_FLASH
  *
  * return  : NONE
  */
Process_Status writeMemory(Tlv_Session *session, uint8_t *data, uint32_t address, int size, Tlv_Command memory) {
  static TaskBlock taskBlock = {.state = 0};
  static uint8_t *pData = NULL;
  static uint32_t tAddress = 0, previousTime = 0;
  static int tSize = 0;
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* pData is a static pointer to keep track the original pointer
     this pointer address will be shift after write data chunk */
  pData = data;
  tSize = size;
  tAddress = address;

  while(tSize > 0) {
    /* Send tlv request */
    tlvWriteDataChunk(session, &pData, &tAddress, &tSize, memory);
    /* Waiting reply from probe */
    previousTime = getSystemTime();
    while((response = tlvReceive(session)) == NULL) {
      /* Check is maximum timeout is reached */
      isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
      yield(tb);
    };
  }
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** loadProgram is a function to load program into target memory
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           Program is a structure contain all the loadable data, address and size
  *           memorySelect can be one of the following value :
  *            - TLV_WRITE_RAM
  *            - TLV_WRITE_FLASH
  *
  * Return  : NONE
  */
int loadProgram(Tlv_Session *session, Program *p, Tlv_Command memorySelect) {
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;
  static int i;

  if(session == NULL) Throw(TLV_NULL_SESSION);
  assert(p != NULL);

  /* Start tlv request task */
  startTask(tb);
  for(i = 0; i < getProgramSectionSize(p); i++) {
    await(writeMemory(  session,
                        getProgramData(p, i),       /* get program data address */
                        getProgramAddress(p, i),    /* get program destination address */
                        getProgramSize(p, i),       /* get program section size */
                        memorySelect
                      ), tb);
  }
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** loadRam is a function to load elf file and update PC
  * to the program entry addresss
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           Program is a structure contain all the loadable data, address and size
  *
  * Return  : NONE
  */
int loadRam(Tlv_Session *session, Program *p) {
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  assert(session != NULL);
  assert(p != NULL);

  /* Start task */
  startTask(tb);
  /* Load specified program */
  await(loadProgram(session, p, TLV_WRITE_RAM), tb);
  printf("Done loading.....\n");
  /* Update program counter */
  await(writeRegister(session, PC, getEntryAddress(p)), tb);
  printf("Update PC........\n");
  /* Run the program */
  await(run(session), tb);
  printf("Run Program......\n");
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** reactiveProgram is to re-active the existing program in target
  * without reload the whole program
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           Program is a structure contain all the loadable data, address and size
  *
  * return  : return PROCESS_DONE if the process is done
  *           return 0 if process is on-going
  */
Process_Status reactiveProgram(Tlv_Session *session, Program *p) {
  CEXCEPTION_T err;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  /* Set Program Counter to entry address and run the program */
  printf("entry %x\n", getEntryAddress(p));
  await(writeRegister(session, PC, getEntryAddress(p)), tb);
  await(run(session), tb);
  printf("Program Successfully re-activate\n");
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** eraseSection is a function to erase flash section by section
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           file is the file need to be load into target memory
  *
  * Return  : NONE
  */
Process_Status eraseSection(Tlv_Session *session, uint32_t address, int size) {
  CEXCEPTION_T err;
  uint32_t data[] = {address, size};
  static TaskBlock taskBlock = {.state = 0};
  static uint32_t previousTime = 0;
  static Program *p;
  TaskBlock *tb = &taskBlock;
  Verify_Status verifyStatus = 0;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  p = getLoadableSection(FP_PATH);
  /* Load Flash Programmer(FP) into target */
  await(loadRam(session, p), tb);
  /* Send section erase request to flash programmer */
  tlvSendRequest(session, TLV_FLASH_ERASE, 8, (uint8_t *)data);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  delProgram(p);
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** eraseAll is a function used to erase flash by selecting bank
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           banks is flash memory bank that want to be erase
  *
  * Return  : NONE
  */
Process_Status eraseAll(Tlv_Session *session, uint32_t banks) {
  static Program *p;
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  p = getLoadableSection(FP_PATH);
  /* Load Flash Programmer(FP) into target */
  await(loadRam(session, p), tb);
  /* Send mass erase request to flash programmer */
  tlvSendRequest(session, TLV_FLASH_MASS_ERASE, 1, (uint8_t *)&banks);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FORTY_SECOND, previousTime), tb);
    yield(tb);
  };
  delProgram(p);
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** loadFlash is a function to load elf file and update PC
  * to the program entry addresss
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           Program is a structure contain all the loadable data, address and size
  *
  * Return  : NONE
  */
int loadFlash(Tlv_Session *session, Program *p) {
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);
  assert(p != NULL);

  /* Start task */
  startTask(tb);
  /* Erase section */
  await(eraseSection(session, FLASH_BEGIN_ADDRESS, getTotalProgramSize(p)), tb);
  printf("Done Erase.....\n");
  /* Load actual program to flash */
  await(loadProgram(session, p, TLV_WRITE_FLASH), tb);
  /* Update program counter */
  await(writeRegister(session, PC, getEntryAddress(p)), tb);
  printf("Update PC........\n");
  /* Run the program */
  await(run(session), tb);
  printf("Run Program......\n");
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** setBreakpoint is a function to set breakpoint
  * at specified address
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           address is the address want to be stop at
  *
  * Return  : NONE
  */
Process_Status setBreakpoint(Tlv_Session *session, uint32_t address) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_BREAKPOINT, 4, (uint8_t *)&address);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);

  returnThis(PROCESS_DONE);
}

/** setWatchpoint is a function to set watchpoint
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           address is the address want to be stop at
  *
  * Return  : NONE
  */
Process_Status setWatchpoint(Tlv_Session *session, uint32_t address,uint32_t addressMask,uint32_t matchedData,int dataSize,uint32_t accessMode)
{
  static uint32_t previousTime = 0;
  uint32_t data[] = {address, addressMask,matchedData,dataSize,accessMode};
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_WATCHPOINT, 20, (uint8_t *)data);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);
}

/** setInstructionRemapping is a function to remap instruction in Code space to RAM
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           instructionAddress is the address will be remapped
  *           machineCode is the machine code that will remapped from the instructionAddress
  *
  * Return  : NONE
  */
Process_Status setInstructionRemapping(Tlv_Session *session,uint32_t instructionAddress,uint32_t machineCode)
{
  static uint32_t previousTime = 0;
  uint32_t data[] = {instructionAddress, machineCode};
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_INSTRUCTION_REMAP, 8, (uint8_t *)data);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);
}

/** setLiteralRemapping is a function to remap literal data
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           literalAddress is the address will be remapped
  *           literalData is the data that will remapped from the literalAddress
  *
  * Return  : NONE
  */
Process_Status setLiteralRemapping(Tlv_Session *session,uint32_t literalAddress,uint32_t literalData)
{
  static uint32_t previousTime = 0;
  uint32_t data[] = {literalAddress, literalData};
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_LITERAL_REMAP, 8, (uint8_t *)data);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);
}

/
/** removeBreakpoint is a function to remove a single breakpoint
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           instructionAddress is the address set to breakpoint previously and going to be removed
  *
  * Return  : NONE
  */
Process_Status removeBreakpoint(Tlv_Session *session, uint32_t instructionAddress)
{
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_REMOVE_BREAKPOINT, 4, (uint8_t *)&instructionAddress);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);
}

/** removeAllBreakpoint is a function to remove all breakpoint set
  *
  * Input   : session contain a element/handler used by tlv protocol
  *          
  * Return  : NONE
  */
Process_Status removeAllBreakpoint(Tlv_Session *session)
{
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_REMOVE_ALL_BREAKPOINT, 0, NULL);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);  
}

/** removeWatchpoint is a function to remove data watchpoint
  *
  * Input   : session contain a element/handler used by tlv protocol
  *          
  * Return  : NONE
  */
Process_Status removeWatchpoint(Tlv_Session *session)
{
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_REMOVE_WATCHPOINT, 0, NULL);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);     
}

/** stopFlashPatchRemapping is a function to stop single instruction/literal flash patch
  * remapping
  *
  * Input   : session contain a element/handler used by tlv protocol
  *           address is the address going to be stop from remapping
  *          
  * Return  : NONE
  */
Process_Status stopFlashPatchRemapping(Tlv_Session *session,uint32_t address)
{
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_STOP_REMAP, 4, (uint8_t *)&address);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);
}

/** stopAllFlashPatchRemapping is a function to stop all instruction/literal flash patch
  * remapping
  *
  * Input   : session contain a element/handler used by tlv protocol
  *
  * Return  : NONE
  */
Process_Status stopAllFlashPatchRemapping(Tlv_Session *session)
{
  static uint32_t previousTime = 0;  
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  tlvSendRequest(session, TLV_STOP_ALL_REMAP, 0, NULL);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);
  returnThis(PROCESS_DONE);   
}

/** tlvWaitDebugEvents is a function to send expected event
  * to probe and wait for event to happen
  *
  * packet :    type     length   event   chksum
  *          +-----------------------------------+
  *          | 1 byte | 1 byte | 1 byte | 1 byte |
  *          +-----------------------------------+
  *
  * Input   : event is the expected event defined in Tlv_Command enum
  *
  * Return  : NONE
  */
EventType tlvWaitDebugEvents(Tlv_Session *session, EventType event) {
  static uint32_t previousTime = 0;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start tlv request task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_DEBUG_EVENTS, 1, (uint8_t *)&event);
  /* Waiting reply */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  /* End task */
  endTask(tb);

  returnThis(response->value[0]);
}

/**
 *  listAllActiveBreakpoint is a function to list all active breakpoint
 *
 */
Process_Status listAllActiveBreakpoint(Tlv_Session *session);
{
  static uint32_t previousTime = 0;
  static uint32_t data[6] =  {} ;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;
  int i ;
  if(session == NULL) Throw(TLV_NULL_SESSION);

  /* Start task */
  startTask(tb);
  /* Send tlv request */
  tlvSendRequest(session, TLV_LISTBREAKPOINT, 0, NULL);
  /* Waiting reply from probe */
  previousTime = getSystemTime();
  while((response = tlvReceive(session)) == NULL) {
    /* Check is maximum timeout is reached */
    isProbeAlive(isTimeout(FIVE_SECOND, previousTime), tb);
    yield(tb);
  };
  
  printf("Active Breakpoint \n");
  for(i = 0 ; i < response->length -1 ; i ++)
  {
      data[i] = get4Byte(&response->value[i*4]);
      printf("%d. \t%x",i+1,data[i]);
  }   
  
  /* End task */
  endTask(tb);
  
  returnThis(PROCESS_DONE);
  
}

/** listAllActiveWatchpoint is a function to list active watchpoint
  *
  * Input   : session contain a element/handler used by tlv protocol
  *
  * Return  : NONE
  */
Process_Status listAllActiveWatchpoint(Tlv_Session *session)
{
  
}


/** selectCommand is a function to select instruction
  * base on userSession
  *
  * Input   : tlv is pointer pointing to tlv packet
  *
  * Return  : NONE
  */
int selectCommand(Tlv_Session *session, User_Session *us) {

  switch(userSession->tlvCommand) {
    case TLV_WRITE_RAM              : writeRam(session, (uint8_t *)us->data, us->address, us->size);            break;
    case TLV_LOAD_RAM               : loadRam(session, us->program);                                            break;
    case TLV_LOAD_FLASH             : loadFlash(session, us->program);                                          break;
    case TLV_READ_MEMORY            : readMemory(session, us->address, us->size);                               break;
    case TLV_WRITE_REGISTER         : writeRegister(session, us->address, us->data[0]);                         break;
    case TLV_READ_REGISTER          : readRegister(session, us->address);                                       break;
    case TLV_HALT_TARGET            : halt(session);                                                            break;
    case TLV_RUN_TARGET             : run(session);                                                             break;
    case TLV_STEP                   : multipleStep(session, us->data[0]);                                       break;
    case TLV_STEPOVER               : stepOver(session);                                                        break;
    case TLV_FLASH_ERASE            : eraseSection(session, us->address, us->size);                             break;
    case TLV_FLASH_MASS_ERASE       : eraseAll(session, us->address);                                           break;
    case TLV_BREAKPOINT             : setBreakpoint(session, us->address);                                      break;
    case TLV_WATCHPOINT             : setWatchpoint(session, us->address, us->data[0], us->data[1],
                                      us->size, us->data[3]);                                                   break;
    case TLV_INSTRUCTION_REMAP      : setInstructionRemapping(session, us->address, us->data[0]);               break;
    case TLV_LITERAL_REMAP          : setLiteralRemapping(session, us->address, us->data[0]);                   break;
    case TLV_SOFT_RESET             : softReset(session);                                                       break;
    case TLV_HARD_RESET             : hardReset(session);                                                       break;
    case TLV_VECT_RESET             : vectorReset(session);                                                     break;
    case TLV_REMOVE_BREAKPOINT      : removeBreakpoint(session, us->address);                                   break;
    case TLV_REMOVE_ALL_BREAKPOINT  : removeAllBreakpoint(session);                                             break;
    case TLV_REMOVE_WATCHPOINT      : removeWatchpoint(session);                                                break;
    case TLV_STOP_REMAP             : stopFlashPatchRemapping(session,us->address);                             break;
    case TLV_STOP_ALL_REMAP         : stopAllFlashPatchRemapping(session);                                      break;
    case TLV_EXIT                   : systemExit(session);                                                      break;
  }
}

/** hostInterpreter
  */
int hostInterpreter(Tlv_Session *session) {
  CEXCEPTION_T err;
  static TaskBlock taskBlock = {.state = 0};
  TaskBlock *tb = &taskBlock;

  Try {
    startTask(tb);
    while((userSession = waitUserCommand()) == NULL) { yield(tb); }
    await(selectCommand(session, userSession), tb);
    endTask(tb);

    delUserSession(userSession);
    returnThis(PROCESS_DONE);
  }
  Catch(err) {
    resetTask(tb);
    delUserSession(userSession);
    Throw(err);
  }
}

uint8_t *createDataBlock(int size) {
  uint8_t *dataBlock;
  dataBlock = malloc(size * sizeof(uint8_t));
  assert(dataBlock != NULL);

  return dataBlock;
}

void delDataBlock(uint8_t *dataBlock) {
  if(dataBlock != NULL) {
    free(dataBlock);
    dataBlock = NULL;
  }
}