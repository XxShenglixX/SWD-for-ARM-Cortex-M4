#ifndef Uart_H
#define Uart_H

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>
#include "Tlv.h"
#include "ErrorCode.h"
#include "CException.h"

typedef enum 
{
  UART_ERROR  = -1,
  UART_OK     = 1,
  UART_BUSY   = 2,
} Uart_Status;

#define UART_PORT                     "COM3"//"COM7"
#define UART_BAUD_RATE                128000//9600
#define closePort(__SESSION__)        CloseHandle(((HANDLE *)((__SESSION__)->handler)))

/* Init/Config USB-to-Serial Converter */
void uartInit(void **handler);
void uartConfig(HANDLE handler);

/* Uart Transmit Function */
uint8_t sendBytes(void *handler, uint8_t *txBuffer, int length);

/* Uart Receive Function */
uint8_t getBytes(void *handler, uint8_t *rxBuffer, int length);
uint8_t getByte(void *handler, uint8_t *rxBuffer);

HANDLE findProbe(void);
HANDLE openComPort(LPCSTR portname, DWORD baudrate);
int isComPortAlive(HANDLE handler);

int isRxBusy(void);
int isTxBusy(void);
void cancelRx(void);
#endif // Uart_H
