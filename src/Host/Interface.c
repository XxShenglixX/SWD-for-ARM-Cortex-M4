#include "Interface.h"

char InputBuffer[INPUT_BUFFER_SIZE];

void displayOptionMenu(void)  {
  printf("\n");
  printf("=============================================================\n");
  printf("##        ARM CORTEX Serial Wire Debugger Interface        ##\n");
  printf("=============================================================\n");
  printf("\n");
  printf("See 'help' or 'help <command>' to read about a specific user command\n\n");
}

void displayMemoryMap(uint8_t *data, uint32_t address, int length) {
  int i;

  for(i = 0; i < length; i+= 16)  {
    printf("> 0x%08x ", address); address += 16;
    printf( "%08x %08x %08x %08x\n",
            get4Byte(&data[i]), get4Byte(&data[i+4]),
            get4Byte(&data[i+8]), get4Byte(&data[i+12])
          );
  }
  printf("\n");
}

/** getRegisterAddress is a function to get user Input
  * in string and get the register address from the string
  *
  * Input   : name is the string enter by user
  *
  * return  : Register_Address in enum
  */
int getRegisterAddress(char *name)  {

  if(strcmp(name, "R0") == 0)                 return R0;
  else if(strcmp(name, "R1") == 0)            return R1;
  else if(strcmp(name, "R2") == 0)            return R2;
  else if(strcmp(name, "R3") == 0)            return R3;
  else if(strcmp(name, "R4") == 0)            return R4;
  else if(strcmp(name, "R5") == 0)            return R5;
  else if(strcmp(name, "R6") == 0)            return R6;
  else if(strcmp(name, "R7") == 0)            return R7;
  else if(strcmp(name, "R8") == 0)            return R8;
  else if(strcmp(name, "R9") == 0)            return R9;
  else if(strcmp(name, "R10") == 0)           return R10;
  else if(strcmp(name, "R11") == 0)           return R11;
  else if(strcmp(name, "R12") == 0)           return R12;
  else if(strcmp(name, "SP") == 0)            return SP;
  else if(strcmp(name, "LR") == 0)            return LR;
  else if(strcmp(name, "PC") == 0)            return PC;
  else if(strcmp(name, "xPSR") == 0)          return xPSR;
  else if(strcmp(name, "MSP") == 0)           return MSP;
  else if(strcmp(name, "PSP") == 0)           return PSP;
  else if(strcmp(name, "SR") == 0)            return SR;
  else if(strcmp(name, "FPSCR") == 0)         return FPSCR;
  else if(strcmp(name, "FPREGS0") == 0)       return FPREGS0;
  else if(strcmp(name, "FPREGS1") == 0)       return FPREGS1;
  else if(strcmp(name, "FPREGS2") == 0)       return FPREGS2;
  else if(strcmp(name, "FPREGS3") == 0)       return FPREGS3;
  else if(strcmp(name, "FPREGS4") == 0)       return FPREGS4;
  else if(strcmp(name, "FPREGS5") == 0)       return FPREGS5;
  else if(strcmp(name, "FPREGS6") == 0)       return FPREGS6;
  else if(strcmp(name, "FPREGS7") == 0)       return FPREGS7;
  else if(strcmp(name, "FPREGS8") == 0)       return FPREGS8;
  else if(strcmp(name, "FPREGS9") == 0)       return FPREGS9;
  else if(strcmp(name, "FPREGS10") == 0)      return FPREGS10;
  else if(strcmp(name, "FPREGS11") == 0)      return FPREGS11;
  else if(strcmp(name, "FPREGS12") == 0)      return FPREGS12;
  else if(strcmp(name, "FPREGS13") == 0)      return FPREGS13;
  else if(strcmp(name, "FPREGS14") == 0)      return FPREGS14;
  else if(strcmp(name, "FPREGS15") == 0)      return FPREGS15;
  else if(strcmp(name, "FPREGS16") == 0)      return FPREGS16;
  else if(strcmp(name, "FPREGS17") == 0)      return FPREGS17;
  else if(strcmp(name, "FPREGS18") == 0)      return FPREGS18;
  else if(strcmp(name, "FPREGS19") == 0)      return FPREGS19;
  else if(strcmp(name, "FPREGS20") == 0)      return FPREGS20;
  else if(strcmp(name, "FPREGS21") == 0)      return FPREGS21;
  else if(strcmp(name, "FPREGS22") == 0)      return FPREGS22;
  else if(strcmp(name, "FPREGS23") == 0)      return FPREGS23;
  else if(strcmp(name, "FPREGS24") == 0)      return FPREGS24;
  else if(strcmp(name, "FPREGS25") == 0)      return FPREGS25;
  else if(strcmp(name, "FPREGS26") == 0)      return FPREGS26;
  else if(strcmp(name, "FPREGS27") == 0)      return FPREGS27;
  else if(strcmp(name, "FPREGS28") == 0)      return FPREGS28;
  else if(strcmp(name, "FPREGS29") == 0)      return FPREGS29;
  else if(strcmp(name, "FPREGS30") == 0)      return FPREGS30;
  else if(strcmp(name, "FPREGS31") == 0)      return FPREGS31;

  else Throw(ERR_INVALID_REGISTER_ADDRESS);
}

/** getWatchpointMaskValue is a function to get user Input
  * in string and convert to integer value to be sent to probe
  *
  * Input   :mask the string enter by user
  *
  * return  : interger representation of the watchpoint mask
  */
int getWatchpointMaskValue(char *mask)
{
       if(strcmp(mask,"No_Mask" )== 0)            return 0;
  else if(strcmp(mask,"Mask_Bit0") == 0)          return 1;
  else if(strcmp(mask,"Mask_Bit1toBit0") == 0)    return 2;
  else if(strcmp(mask,"Mask_Bit2toBit0") == 0)    return 3;
  else if(strcmp(mask,"Mask_Bit3toBit 0") == 0)   return 4;
  else if(strcmp(mask,"Mask_Bit4toBit 0") == 0)   return 5;
  else if(strcmp(mask,"Mask_Bit5toBit 0") == 0)   return 6;
  else if(strcmp(mask,"Mask_Bit6toBit 0") == 0)   return 7;
  else if(strcmp(mask,"Mask_Bit7toBit 0") == 0)   return 8;
  else if(strcmp(mask,"Mask_Bit8toBit 0") == 0)   return 9;
  else if(strcmp(mask,"Mask_Bit9toBit 0") == 0)   return 10;
  else if(strcmp(mask,"Mask_Bit10toBit 0") == 0)  return 11;
  else if(strcmp(mask,"Mask_Bit11toBit 0") == 0)  return 12;
  else if(strcmp(mask,"Mask_Bit12toBit 0") == 0)  return 13;
  else if(strcmp(mask,"Mask_Bit13toBit 0") == 0)  return 14;
  else if(strcmp(mask,"Mask_Bit14toBit 0") == 0)  return 15;
  
  else Throw(ERR_INVALID_MASK);
}

/** getWatchpointSizevalue is a function to get user Input
  * in string and convert to integer value to be sent to probe
  *
  * Input   :size the string enter by user
  *
  * return  : interger representation of the watchpoint size 
  */
int getWatchpointSizevalue(char *size)
{
       if(strcmp(size,"Byte") == 0)     return 0;
  else if(strcmp(size,"Halfword") == 0) return 1;
  else if(strcmp(size,"Word") == 0)     return 2;
  
  else Throw(ERR_INVALID_DATASIZE);
}

/** getWatchpointAccessMode is a function to get user Input
  * in string and convert to integer value to be sent to probe
  *
  * Input   :mode the string enter by user
  *
  * return  : interger representation of the watchpoint access mode 
  */
int getWatchpointAccessMode(char *mode)
{
       if(strcmp(mode,"Read") == 0)      return 5;
  else if(strcmp(mode,"Write") == 0)     return 6;
  else if(strcmp(mode,"ReadWrite") == 0) return 7;
  
  else Throw(ERR_INVALID_ACCESSMODE);
}

/** getRegisterAddress is a function to get user Input
  * in string and get the register address from the string
  *
  * Input   : name is the string enter by user
  *
  * return  : Register_Address in enum
  */
int getFlashBank(char *name) {

  if(strcmp(name, "bank1") == 0)              return BANK_1;
  else if(strcmp(name, "bank2") == 0)         return BANK_2;
  else if(strcmp(name, "full") == 0)          return BOTH_BANK;

  else Throw(ERR_INVALID_BANK_SELECTION);
}

/** userLoadProgram is a function to get load program instruction
  * and file path need to load into RAM/Flash from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userLoadProgram(User_Session *us, String *userInput)  {
  CEXCEPTION_T err;
  Identifier *iden, *memory;

  Try {
    memory = (Identifier*)getToken(userInput);
    iden = (Identifier*)getToken(userInput);
  } Catch(err) {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(iden->type != FILE_TOKEN)                  Throw(ERR_EXPECT_FILE_PATH);
  else if(memory->type != IDENTIFIER_TOKEN)     Throw(ERR_EXPECT_MEMORY_SELECTION);
  else if(strcmp(memory->name, "ram") == 0)     us->tlvCommand = TLV_LOAD_RAM;
  else if(strcmp(memory->name, "flash") == 0)   us->tlvCommand = TLV_LOAD_FLASH;
  else Throw(ERR_INVALID_MEMORY_SELECTION);

  us->program = getLoadableSection(iden->name);
}

/** userWriteMemory is a function to get write data into
  * memory map RAM/Flash
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userWriteMemory(User_Session *us, String *userInput) {
  CEXCEPTION_T err;
  Identifier *memory; Number* data, *address;
  int i = 0;

  Try {
    memory = (Identifier *)getToken(userInput);
    address = (Number *)getToken(userInput);
    if(address->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);
    while(userInput->length > 1) {
      data = (Number *)getToken(userInput);
      if(data->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);
      us->data[i++] = data->value;
    }
  } Catch(err) {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(memory->type != IDENTIFIER_TOKEN)          Throw(ERR_EXPECT_MEMORY_SELECTION);
  else if(strcmp(memory->name, "ram") == 0)     us->tlvCommand = TLV_WRITE_RAM;
  else if(strcmp(memory->name, "flash") == 0)   us->tlvCommand = TLV_WRITE_FLASH;
  else Throw(ERR_INVALID_MEMORY_SELECTION);

  us->size = i * 4;
  us->address = address->value;
}

/** userReadMemory is a function to get read memory instruction
  * and memory address need to read from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userReadMemory(User_Session *us, String *userInput) {
  CEXCEPTION_T err;
  Number *address, *size;

  Try {
    address = (Number*)getToken(userInput);
    size = (Number*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
  else if(size->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);

  us->tlvCommand = TLV_READ_MEMORY;
  us->address = address->value;
  us->size = size->value;
}

/** userWriteRegister is a function to get register address
  * and value need to write into register
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userWriteRegister(User_Session *us, String *userInput)  {
  int regAddress = 0;
  CEXCEPTION_T err;
  Identifier* iden; Number* data;
  uint32_t data32;

  Try {
    iden = (Identifier*)getToken(userInput);
    data = (Number*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(iden->type != IDENTIFIER_TOKEN)    Throw(ERR_EXPECT_REGISTER_ADDRESS);
  else if(data->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);

  us->tlvCommand = TLV_WRITE_REGISTER;
  us->data[0] = (uint32_t)data->value;
  us->address = getRegisterAddress(iden->name);
}

/** userReadRegister is a function to get register address
  * enter by user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userReadRegister(User_Session *us, String *userInput) {
  int regAddress = 0;
  CEXCEPTION_T err; Identifier* iden;

  Try {
    iden = (Identifier*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(iden->type != IDENTIFIER_TOKEN)  Throw(ERR_EXPECT_REGISTER_ADDRESS);

  us->tlvCommand = TLV_READ_REGISTER;
  us->address = getRegisterAddress(iden->name);
}

/** userHaltTarget is a function to get halt instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userHaltTarget(User_Session *us) {

  us->tlvCommand = TLV_HALT_TARGET;
}

/** userRunTarget is a function to get run instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userRunTarget(User_Session *us) {

  us->tlvCommand = TLV_RUN_TARGET;
}

/** userStepTarget is a function to get step instruction
  * and number of step required from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userStepTarget(User_Session *us, String *userInput) {
  Number *data; CEXCEPTION_T err;
  uint32_t data32;

  Try {
    data = (Number*)getToken(userInput);
  } Catch(err) {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(data->type != NUMBER_TOKEN)  Throw(ERR_EXPECT_NUMBER);

  us->tlvCommand = TLV_STEP;
  us->data[0] = data->value;
}

/** userStepOver is a function to get step over instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userStepOver(User_Session *us)
{
  us->tlvCommand = TLV_STEPOVER ;
}

/** userSetBreakpoint is a function to get address need
  * to set breakpoint from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userSetBreakpoint(User_Session *us, String *userInput) {
  CEXCEPTION_T err; Number *address;

  Try {
    address = (Number*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);

  us->tlvCommand = TLV_BREAKPOINT;
  us->address = address->value;
}

/** userSetBreakpoint is a function to get information need
  * to set watchpoint from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userSetWatchpoint(User_Session *us, String *userInput) {
  CEXCEPTION_T err; 
  Number *address, *data ;
  Identifier *mask, *size, *mode ;
  Try {
    address = (Number*)getToken(userInput);
    mask = (Identifier*)getToken(userInput);
    data = (Number*)getToken(userInput);
    size = (Identifier*)getToken(userInput);
    mode = (Identifier*)getToken(userInput);
    
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)       Throw(ERR_EXPECT_NUMBER);
  else if(data->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
  else if(mask->type != IDENTIFIER_TOKEN) Throw(ERR_EXPECT_WATCHPOINT_MASK);
  else if(size->type != IDENTIFIER_TOKEN) Throw(ERR_EXPECT_WATCHPOINT_SIZE);
  else if(mode->type != IDENTIFIER_TOKEN) Throw(ERR_EXPECT_WATCHPOINT_MODE);
    
  us->tlvCommand = TLV_WATCHPOINT;
  us->address = address->value;
  us->data[0] = getWatchpointMaskValue(mask->name);
  us->data[1] = data->value;
  us->size = getWatchpointSizevalue(size->name);
  us->data[3] = getWatchpointAccessMode(mode->name);
}

/** userSetInstructionRemapping is a function to get information need
  * to set instruction remapping from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userSetInstructionRemapping(User_Session *us, String *userInput)
{
  CEXCEPTION_T err; 
  Number *address, *data ;

  Try {
    address = (Number*)getToken(userInput);
    data = (Number*)getToken(userInput);
    
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
  else if(data->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);
    
  us->tlvCommand = TLV_INSTRUCTION_REMAP;
  us->address = address->value;
  us->data[0] = (uint32_t)data->value;
}

/** userSetLiteralRemapping is a function to get information need
  * to set literal remapping from user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userSetLiteralRemapping(User_Session *us, String *userInput)
{
  CEXCEPTION_T err; 
  Number *address, *data ;

  Try {
    address = (Number*)getToken(userInput);
    data = (Number*)getToken(userInput);
    
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
  else if(data->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);
    
  us->tlvCommand = TLV_LITERAL_REMAP;
  us->address = address->value; 
  us->data[0] = (uint32_t)data->value;
}

/** userListInformation is a function to list all active breakpoint/watchpoint
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userListInformation(User_Session *us, String *userInput)
{
  CEXCEPTION_T err; 
  Identifier *option ;
  
  Try {
    option = (Identifier*)getToken(userInput);
    
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(option->type != NUMBER_TOKEN)     Throw(ERR_INVALID_LIST_OPTION);

  if(strcmp(option->name, "breakpoint") == 0)       us->tlvCommand = TLV_LISTBREAKPOINT;
  else if(strcmp(option->name, "watchpoint") == 0)  us->tlvCommand = TLV_LISTWATCHPOINT;
  else                                              Throw(ERR_INVALID_LIST_OPTION);
}

/** userRemoveBreakpoint is a function to remove breakpoint
  * based on the input
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userRemoveBreakpoint(User_Session *us,String *userInput)
{
  CEXCEPTION_T err; Number *address;
  int mode = 0; //default mode
  Try 
  {
    address = (Number*)getToken(userInput);
  } 
  Catch(err)  
  {
    mode = 1 ; //EMPTY STRING input deteced , switch to remove all breakpoint mode
  }
  
  if(mode == 0)
  {
    if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
    us->tlvCommand = TLV_REMOVE_BREAKPOINT;
    us->address = address->value;
  }
  else
    us->tlvCommand = TLV_REMOVE_ALL_BREAKPOINT;
  
}

/** userRemoveAllBreakpoint is a function to get remove all breakpoint
  * instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userRemoveAllBreakpoint(User_Session *us)
{
  us->tlvCommand = TLV_REMOVE_ALL_BREAKPOINT;
}

/** userRemoveWatchpoint is a function to get remove watchpoint
  * instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userRemoveWatchpoint(User_Session *us)
{
  us->tlvCommand = TLV_REMOVE_WATCHPOINT;
}


/** userStopFlashPatchRemapping is a function to stop flash patch remapping
  * based on the intput by user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userStopFlashPatchRemapping(User_Session *us,String *userInput)
{
  CEXCEPTION_T err; Number *address;
  int mode = 0 ;
  
  Try 
  {
    address = (Number*)getToken(userInput);
  } 
  Catch(err) 
  {
    mode = 1; //EMPTY string input detected, switch to stop all remapping mode
  }
  
  if(mode == 0)
  {
    if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);

    us->tlvCommand = TLV_STOP_REMAP;
    us->address = address->value;
  }
  else
    us->tlvCommand = TLV_STOP_ALL_REMAP;

}

/** userStopAllFlashPatchRemapping is a function to get stop all remapping
  * instruction from user
  *
  * Input   : NONE
  *
  * return  : userSession contain all the information from the user input
  */
void userStopAllFlashPatchRemapping(User_Session *us)
{
  us->tlvCommand = TLV_STOP_ALL_REMAP;
}

void userSectionErase(User_Session *us, String *userInput) {
  CEXCEPTION_T err;
  Number *address, *size;

  Try {
    address = (Number*)getToken(userInput);
    size = (Number*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(address->type != NUMBER_TOKEN)     Throw(ERR_EXPECT_NUMBER);
  else if(size->type != NUMBER_TOKEN)   Throw(ERR_EXPECT_NUMBER);

  us->tlvCommand = TLV_FLASH_ERASE;
  us->address = address->value;
  us->size = size->value;
}

/** userErase is a function to retrieve erase option
  * enter by user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userErase(User_Session *us, String *userInput) {
  CEXCEPTION_T err;
  Identifier* iden;
  int eraseSection = 0;

  Try {
    iden = (Identifier*)getToken(userInput);
  } Catch(err)  {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(iden->type != IDENTIFIER_TOKEN)  Throw(ERR_EXPECT_ERASE_SECTION);

  else {
    if(strcmp(iden->name, "section") == 0) {
      return userSectionErase(us, userInput);
    }
    else {
      us->tlvCommand = TLV_FLASH_MASS_ERASE;
      us->address = (uint32_t)getFlashBank(iden->name);
    }
  }
}

/** userReset is a function to retrieve reset option
  * enter by user
  *
  * Input   : userInput is the string enter by user
  *
  * return  : userSession contain all the information from the user input
  */
void userReset(User_Session *us, String *userInput) {
  CEXCEPTION_T err;
  Identifier *option;

  Try {
    option = (Identifier*)getToken(userInput);
  } Catch(err) {
    Throw(ERR_INCOMPLETE_COMMAND);
  }

  if(option->type != IDENTIFIER_TOKEN)          Throw(ERR_OPTION_NOT_FOUND);
  else if(strcmp(option->name, "soft") == 0)    us->tlvCommand = TLV_SOFT_RESET;
  else if(strcmp(option->name, "hard") == 0)    us->tlvCommand = TLV_HARD_RESET;
  else if(strcmp(option->name, "vector") == 0)  us->tlvCommand = TLV_VECT_RESET;
  else Throw(ERR_INVALID_COMMAND);
}

/** userExit is a function to exit instruction
  *
  * Input   : userInput is the string enter by userExit
  *
  * return  : userSession contain all the information from the user input
  */
void userExit(User_Session *us) {

  us->tlvCommand = TLV_EXIT;
}

Command_Code getCommandCode(char *commandName) {

  if(strcmp(commandName, "help") == 0)                return HELP;
  else if(strcmp(commandName, "load") == 0)           return LOAD;
  else if(strcmp(commandName, "write") == 0)          return WRITE_COMMAND;
  else if(strcmp(commandName, "read") == 0)           return READ_MEMORY;
  else if(strcmp(commandName, "wreg") == 0)           return WRITE_REGISTER;
  else if(strcmp(commandName, "reg") == 0)            return READ_REGISTER;
  else if(strcmp(commandName, "halt") == 0)           return HALT;
  else if(strcmp(commandName, "run") == 0)            return RUN;
  else if(strcmp(commandName, "step") == 0)           return STEP;
  else if(strcmp(commandName, "stepOver") == 0)       return STEPOVER;
  else if(strcmp(commandName, "brkpt") == 0)          return BREAKPOINT;
  else if(strcmp(commandName, "watch") == 0)          return WATCHPOINT;
  else if(strcmp(commandName, "iRemap") == 0)         return INSTRUCTION_REMAP;
  else if(strcmp(commandName, "lRemap") == 0)         return LITERAL_REMAP;
  else if(strcmp(commandName, "listt") == 0)          return LIST_INFO;
  else if(strcmp(commandName, "rmBrkpt") == 0)        return REMOVE_BREAKPOINT;
  else if(strcmp(commandName, "rmWatch") == 0)        return REMOVE_WATCHPOINT;
  else if(strcmp(commandName, "stopRemap") == 0)      return STOP_REMAP;
  else if(strcmp(commandName, "erase") == 0)          return ERASE;
  else if(strcmp(commandName, "reset") == 0)          return RESET_COMMAND;
  else if(strcmp(commandName, "exit") == 0)           return EXIT;


  else Throw(ERR_INVALID_USER_COMMAND);
}

/** helpMenu is a function to display useful information
  * about the user command
  *
  * Input   : NONE
  *
  * return  : NONE
  */
void helpMenu(String *userInput) {
  Identifier *command;
  Command_Code ccode;

  if(userInput->length <= 1) {
    printf("Available commands :\n\n");
    printf(" load         load program into target memory can either ram/flash\n");
    printf(" read         read data from target memory\n");
    printf(" wreg         write data into target register\n");
    printf(" reg          read data from target register\n");
    printf(" halt         halt target\n");
    printf(" run          run target\n");
    printf(" step         executes code one statement at a time\n");
    printf(" stepOver     step over one statement\n");
    printf(" brkpt        stop or pause program at specific address\n");
    printf(" watch        set data watchpoint to monitor data changes\n");
    printf(" iRemap       perform instruction remapping\n");
    printf(" lRemap       perform literal remapping\n");
    printf(" list         list all active breakpoint/watchpoint\n");
    printf(" rmBrkpt      remove breakpoint for specific address or all breakpoints\n");
    printf(" rmWatch      remove data watchpoint\n");
    printf(" stopRemap    stop flash patch remapping for specific address or stop all\n");
    printf(" erase        erase target flash memory\n");
    printf(" reset        reset target process\n");
    printf(" exit         exit current program\n");
  }
  else {
    printf("\n");
    command = (Identifier*)getToken(userInput);
    ccode = getCommandCode(command->name);
    helpCommand(ccode);
  }
}

void helpCommand(Command_Code ccode) {

  switch(ccode) {
    case LOAD                         : printf(" load <memory> <file_path>                                                \n\n");
                                        printf(" <memory>     Target memory location that program will be                   \n");
                                        printf("              load into (ram/flash)                                         \n");
                                        printf(" <file_path>  Program elf file location                                     \n");
                                        break;

    case READ_MEMORY                  : printf(" read <address> <size>                                                      \n\n");
                                        printf(" <address>    Any valid memory address                                      \n");
                                        printf(" <size>       Size of the memory want to read                               \n");
                                        break;

    case WRITE_REGISTER               : printf(" wreg <reg_address> <value>                                               \n\n");
                                        printf(" <reg_address>    Register address can be one of the following value :      \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Core Register 0 - 12 |   R0 - R12   |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Stack Pointer        |      SP      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Link Register        |      LR      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Program Counter      |      PC      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Special-purpose      |              |               \n");
                                        printf("                      | program status       |     xPSR     |               \n");
                                        printf("                      | registers            |              |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Main Stack Pointer   |      MSP     |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Process Stack Pointer|      PSP     |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Special Register     |      SR      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Floating Point       |              |               \n");
                                        printf("                      | Staus and            |     FPSCR    |               \n");
                                        printf("                      | Control Register     |              |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Floating Point       |              |               \n");
                                        printf("                      | Register 0 - 31      | FPSCR 0 - 31 |               \n");
                                        printf("                      =======================================               \n");
                                        printf(" <value>          Data write into register in byte format                   \n");
                                        break;

    case READ_REGISTER                : printf(" reg <reg_address>                                                        \n\n");
                                        printf(" <reg_address>    Register address can be one of the following value :      \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Core Register 0 - 12 |   R0 - R12   |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Stack Pointer        |      SP      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Link Register        |      LR      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Program Counter      |      PC      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Special-purpose      |              |               \n");
                                        printf("                      | program status       |     xPSR     |               \n");
                                        printf("                      | registers            |              |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Main Stack Pointer   |      MSP     |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Process Stack Pointer|      PSP     |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Special Register     |      SR      |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Floating Point       |              |               \n");
                                        printf("                      | Staus and            |     FPSCR    |               \n");
                                        printf("                      | Control Register     |              |               \n");
                                        printf("                      =======================================               \n");
                                        printf("                      | Floating Point       |              |               \n");
                                        printf("                      | Register 0 - 31      | FPSCR 0 - 31 |               \n");
                                        printf("                      =======================================               \n");
                                        break;

    case HALT                         : printf(" halt <empty>                                                             \n\n");
                                        break;

    case RUN                          : printf(" run <empty>                                                              \n\n");
                                        break;

    case STEP                         : printf(" step <number_of_step>                                                    \n\n");
                                        printf(" <number_of_step>   Number of statement can be executes at a time           \n");
                                        break;
    
    case STEPOVER                     : printf(" stepOver <empty>                                                           \n\n");
                                        break;
                                        
    case BREAKPOINT                   : printf(" brkpt <address>                                                          \n\n");
                                        printf(" <address>   Any valid code region address 0x00000000 - 0x1FFFFFFF          \n");
                                        break;
      
    case WATCHPOINT                   : printf(" watch <address> <mask_option> <data> <size_option> <access_option>       \n\n");                                    
                                        printf(" <address>        Address of the variable to be watched                     \n");
                                        printf(" <mask_option>    Address mask to be applied for comparing further range    \n");
                                        printf("                  and can be one of the following value :                   \n");
                                        printf("                  No_Mask          :: No address mask is applied            \n");
                                        printf("                  Mask_Bit0        :: Mask Bit 0 of the address             \n");
                                        printf("                  Mask_Bit1toBit0  :: Mask Bit 1 to Bit 0 of the address    \n");
                                        printf("                  Mask_Bit2toBit0  :: Mask Bit 2 to Bit 0 of the address    \n");
                                        printf("                      ''           ::             ''                        \n");
                                        printf("                  Mask_Bit14toBit0 :: Mask Bit 14 to Bit 0 of the address  \n\n");
                                        printf(" <data>           Value of the data to be matched                           \n");
                                        printf(" <size_option>    Size of the data going to be compared and can be one of   \n");
                                        printf("                  the following value :                                     \n");
                                        printf("                  Byte             :: Compare 1 bytes / 8 bits              \n");
                                        printf("                  Halfword         :: Compare 1 haflword / 16 bits          \n");
                                        printf("                  Word             :: Compare 1 word / 32 bits            \n\n");
                                        printf(" <access_option>  Access mode for the watchpoint and can be one of the      \n");
                                        printf("                  following value :                                         \n");
                                        printf("                  Read             :: Watchpoint on Read access             \n");
                                        printf("                  Write            :: Watchpoint on Write access            \n");
                                        printf("                  ReadWrite        :: Watchpoint on ReadWrite access        \n");
                                        break;
    
    case INSTRUCTION_REMAP            : printf(" iRemap <address> <machineCode>                                           \n\n");
                                        printf(" <address>        Address of the instruction in code region to be remapped  \n");
                                        printf("                  (0x08000000 - 0x1FFF FFFF)                                \n");
                                        printf(" <machineCode>    Machine code of the instruction that will be remapped onto\n");
                                        break;
                                        
    case LITERAL_REMAP                : printf(" lRemap <address> <data>                                                  \n\n");
                                        printf(" <address>        Address of the literal data to be remapped                \n");
                                        printf(" <data>           Value of the literal data to be remapped onto             \n");
                                        break;
                                        
    case LIST_INFO                    : printf(" list <option>                                                           \n\n");
                                        printf(" <option>       Different modes of listing information and can be one of   \n");
                                        printf("                the following value :                                      \n");
                                        printf("                breakpoint        :: list active breakpoint                \n");
                                        printf("                watchpoint        :: list active watchpoint                \n");
                                        break;

    case REMOVE_BREAKPOINT            : printf(" Remove single breakpoint for a specific address                          \n\n");
                                        printf(" rmBrkpt <address>                                                          \n");
                                        printf(" <address>        Address of the breakpoint set in code region to be removed\n");
                                        printf("                  (0x08000000 - 0x1FFFFFFF)                               \n\n");
                                        printf(" Remove all breakpoint                                                    \n\n");
                                        printf(" rmBrkpt <empty>                                                            \n");
                                        break;
                                                                              
    case REMOVE_WATCHPOINT            : printf(" rmWatch <empty>                                                            \n");
                                        break;
                                        
    case STOP_REMAP                   : printf(" Stop single instruction/literal remapping for a specific address         \n\n");
                                        printf(" stopRemap <address>                                                        \n");
                                        printf(" <address>        Address of the remap set in code region to be removed     \n");
                                        printf("                  (0x08000000 - 0x1FFFFFFF)                               \n\n");
                                        printf(" Stop all instruction/literal remapping                                   \n\n");
                                        printf(" stopRemap <empty>                                                          \n");
                                        break;
                                          
    case ERASE                        : printf(" Section Erase                                                              \n");
                                        printf(" erase <section> <address> <size>                                         \n\n");
                                        printf(" <address>        Any valid flash address 0x08000000 - 0x081FFFFF           \n");
                                        printf(" <size>           Size of flash to erase can be any value                 \n\n");
                                        printf(" Mass Erase                                                                 \n");
                                        printf(" erase <bank_option>                                                      \n\n");
                                        printf(" <bank_option>    Flash bank option can be one of the following value :     \n");
                                        printf("                    bank1   :: Flash Bank 1 0x08000000 - 0x080E0000         \n");
                                        printf("                    bank2   :: Flash Bank 2 0x08100000 - 0x081FFFFF         \n");
                                        printf("                    full    :: Both Flash Bank 1 and 2                      \n");
                                        break;

    case RESET_COMMAND                : printf(" reset <reset_option>                                                     \n\n");
                                        printf(" <reset_option>  Type of reset can be one of the following value :          \n");
                                        printf("                    hard    :: hardware reset                               \n");
                                        printf("                    soft    :: software reset                               \n");
                                        printf("                    vector  :: vector reset                                 \n");
                                        break;

    case EXIT                         : printf(" exit <empty>                                                             \n\n");
                                        break;
  }
}

/** InterpreteCommand is a function to interpreter user input to
  * a meaningful information to machine
  *
  * Input   : ccode is a Command code can be one of the following value
  *
  * return  : userSession contain all the information from the user input
  */
User_Session *InterpreteCommand(String *userInput) {
  Identifier *command; Command_Code ccode;
  User_Session *us = createNewUserSession();
  
  command = (Identifier*)getToken(userInput);
  ccode = getCommandCode(command->name);

  if(ccode == HELP)                           {helpMenu(userInput); return NULL;}
  else if(ccode == LOAD)                      userLoadProgram(us, userInput);
  else if(ccode == WRITE_COMMAND)             userWriteMemory(us, userInput);
  else if(ccode == READ_MEMORY)               userReadMemory(us, userInput);
  else if(ccode == WRITE_REGISTER)            userWriteRegister(us, userInput);
  else if(ccode == READ_REGISTER)             userReadRegister(us, userInput);
  else if(ccode == HALT)                      userHaltTarget(us);
  else if(ccode == RUN)                       userRunTarget(us);
  else if(ccode == STEP)                      userStepTarget(us, userInput);
  else if(ccode == STEPOVER)                  userStepOver(us);
  else if(ccode == BREAKPOINT)                userSetBreakpoint(us, userInput);
  else if(ccode == WATCHPOINT)                userSetWatchpoint(us,userInput);
  else if(ccode == INSTRUCTION_REMAP)         userSetInstructionRemapping(us,userInput);
  else if(ccode == LITERAL_REMAP)             userSetLiteralRemapping(us,userInput);
  else if(ccode == REMOVE_BREAKPOINT)         userRemoveBreakpoint(us,userInput);
  else if(ccode == LIST_INFO)                 userListInformation(us,userInput);
  else if(ccode == REMOVE_WATCHPOINT)         userRemoveWatchpoint(us);
  else if(ccode == STOP_REMAP)                userStopFlashPatchRemapping(us,userInput);
  else if(ccode == ERASE)                     userErase(us, userInput);
  else if(ccode == RESET_COMMAND)             userReset(us, userInput);
  else if(ccode == EXIT)                      userExit(us);

  
  else Throw(ERR_INVALID_USER_COMMAND);
  
  return us;
}

/** waitUserCommand is a function to aquire whatever information
  * enter by user
  */
User_Session *waitUserCommand(void) {
  static int display = 0;
  Number *num; String *str;
  
  if(display == 0) {
    display = 1;
    printf("> ");
  }

  if(!kbhit()) return NULL;

  else {
    display = 0;
    fgets(InputBuffer, INPUT_BUFFER_SIZE, stdin);
  }

  str = stringNew(InputBuffer);
  User_Session *us = InterpreteCommand(str);

  stringDel(str);
  return us;
}

User_Session *createNewUserSession(void) {
  User_Session *us = malloc(sizeof(User_Session));

  us->size        = 0;
  us->address     = 0;
  us->program     = NULL;
  us->tlvCommand  = 0;

  memset(us->data, 0, sizeof(us->data));

  return us;
}

void delUserSession(User_Session *us) {
  if(us != NULL) {
    delProgram(us->program);
    free(us);
    us = NULL;
  }
}