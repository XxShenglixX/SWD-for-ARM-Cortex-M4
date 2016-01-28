#ifndef Interface_H
#define Interface_H

#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <malloc.h>
#include "Token.h"
#include "Tlv.h"
#include "LoadElf.h"
#include "ErrorCode.h"

#define INPUT_BUFFER_SIZE 2048

typedef struct
{
  int size;
  uint32_t data[INPUT_BUFFER_SIZE];
  uint32_t address;
  Program *program;
  Tlv_Command tlvCommand;
} User_Session;

typedef enum {
  R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	R8,
	R9,
	R10,
	R11,
	R12,
	SP,
	LR,
	PC,
	xPSR,
	MSP,
	PSP,
	SR,
	FPSCR = 33,
	FPREGS0 = 64,
	FPREGS1 ,
	FPREGS2 ,
	FPREGS3 ,
	FPREGS4 ,
	FPREGS5 ,
	FPREGS6 ,
	FPREGS7 ,
	FPREGS8 ,
	FPREGS9 ,
	FPREGS10 ,
	FPREGS11 ,
	FPREGS12 ,
	FPREGS13 ,
	FPREGS14 ,
	FPREGS15 ,
	FPREGS16 ,
	FPREGS17 ,
	FPREGS18 ,
	FPREGS19 ,
	FPREGS20 ,
	FPREGS21 ,
	FPREGS22 ,
	FPREGS23 ,
	FPREGS24 ,
	FPREGS25 ,
	FPREGS26 ,
	FPREGS27 ,
	FPREGS28 ,
	FPREGS29 ,
	FPREGS30 ,
	FPREGS31 = 95
} Register_Address;

typedef enum {
  BANK_1 = 1,
  BANK_2,
  BOTH_BANK,
} Flash_Bank;

typedef enum {
  HELP = 1,
  LOAD,
  WRITE_COMMAND,
  READ_MEMORY,
  WRITE_REGISTER,
  READ_REGISTER,
  HALT,
  RUN,
  STEP,
  STEPOVER,
  BREAKPOINT,
  WATCHPOINT,
  INSTRUCTION_REMAP,
  LITERAL_REMAP,
  REMOVE_BREAKPOINT,
  //REMOVE_ALLBREAKPOINT,
  REMOVE_WATCHPOINT,
  STOP_REMAP,
  //STOP_ALLREMAP,
  ERASE,
  RESET_COMMAND,
  EXIT
} Command_Code;

void displayOptionMenu(void);
User_Session *createNewUserSession(void);
User_Session *waitUserCommand(void);
User_Session *InterpreteCommand(String *userInput);
int getRegisterAddress(char *name);
int getWatchpointMaskValue(char *mask);
int getWatchpointSizevalue(char *size);
int getWatchpointAccessMode(char *mode);
int getFlashBank(char *name);
void displayMemoryMap(uint8_t *data, uint32_t address, int length);
void helpMenu(String *userInput);
void helpCommand(Command_Code ccode);
Command_Code getCommandCode(char *commandName);

/* User Instruction */
void userLoadProgram(User_Session *us, String *userInput);
void userWriteMemory(User_Session *us, String *userInput);
void userReadMemory(User_Session *us, String *userInput);
void userWriteRegister(User_Session *us, String *userInput);
void userReadRegister(User_Session *us, String *userInput);
void userStepTarget(User_Session *us, String *userInput);
void userStepOver(User_Session *us);
void userHaltTarget(User_Session *us);
void userRunTarget(User_Session *us);
void userSetBreakpoint(User_Session *us, String *userInput);
void userSetWatchpoint(User_Session *us, String *userInput);
void userSetInstructionRemapping(User_Session *us, String *userInput);
void userSetLiteralRemapping(User_Session *us, String *userInput);
void userListInformation(User_Session *us, String *userInput);
void userRemoveBreakpoint(User_Session *us,String *userInput);
void userRemoveWatchpoint(User_Session *us);
void userStopFlashPatchRemapping(User_Session *us,String *userInput);
void userErase(User_Session *us, String *userInput);
void userSectionErase(User_Session *us, String *userInput);
void userReset(User_Session *us, String *userInput);
void userExit(User_Session *us);

void delUserSession(User_Session *us);
#endif // Interface_H