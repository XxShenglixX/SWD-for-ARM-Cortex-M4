#include "Tpiu.h"

/** configTpiu is a function to configure TPIU
 */
void tpiuConfigPort(void) {
  GPIO_Init init;
  
  init.mode = ALTFUNC_MODE;
  init.pin = TRACE_CLOCK_PIN;
  init.af = AF0;
  
  /* Configure Trace Port Clock */
  gpioConfigPort(TRACE_PORT, &init);
  
  /* Configure Trace Port Data0 */
  init.pin = TRACE_DATA0_PIN;
  gpioConfigPort(TRACE_PORT, &init);
}

/** configTpiu is a function to configure TPIU
  */
void tpiuInit(void) {
  /* Configure TraceClock and TraceData0 */
  tpiuConfigPort();
  /* Current trace port size is 2 */
  tpiuPortSize(TPIU_PORT_SIZE_2);
  
  /* Bit [1:0]  : the formatter is enabled 
     Bit [8]    : set to 1 to indicate that triggers are indicated */
  tpiuFormatter(0x102);
  
  /* TPIU select Synchronous mode protocol by default*/
  tpiuProtocol(SYNC_MODE);
  
  /* Enable trace and debug blocks :
      - Data Watchpoint and Trace       (DWT)
      - Instrumentation Trace Macrocell (ITM)
      - Embedded Trace Macrocell        (ETM)
      - Trace Port Interface Unit       (TPIU). */
  enableDWTandITM();
  
  /* Enable trace I/O and select Synchronous Mode with a
     TRACEDATA size of 1 */
  enableAndSelectTraceMode(DBGMCU_CR_TRACE_MODE_0);
}

void tpiuConfigPortx(void) {
  GPIO_Init init;

  init.mode = ALTFUNC_MODE;
  init.pin = SWO_TRACE_PIN;
  init.af = AF0;
  
  /* Configure Trace Port Clock */
  gpioConfigPort(SWO_TRACE_PORT, &init);
}

/** configTpiu is a function to configure TPIU
  */
void tpiuInitx(void) {
  uint32_t dataRead = 0;
  
  /* Configure SWO */
  tpiuConfigPortx();
  
  /* Current trace port size is 1 (SWO) */
  tpiuPortSize(TPIU_PORT_SIZE_1);
  
  /* Bit [1:0]  : the formatter is enabled 
     Bit [8]    : set to 1 to indicate that triggers are indicated */
  tpiuFormatter(0x102);
  
  /* TPIU select Synchronous mode protocol by default*/
  tpiuProtocol(NRZ_MODE);
  
  /* Enable trace and debug blocks :
      - Data Watchpoint and Trace       (DWT)
      - Instrumentation Trace Macrocell (ITM)
      - Embedded Trace Macrocell        (ETM)
      - Trace Port Interface Unit       (TPIU). */
  enableDWTandITM();
  
  /* Enable trace I/O and select Synchronous Mode with a
     TRACEDATA size of 1 */
  // enableAndSelectTraceMode(DBGMCU_CR_TRACE_MODE_0);
  /* Enable trace I/O and select Asynchronous Mode with a
     TRACEDATA size of 1 */
  memoryWriteWord((uint32_t)&DBGMCU->CR, DBGMCU_CR_TRACE_IOEN);
  memoryReadWord((uint32_t)&DBGMCU->CR, &dataRead);
  printf("DBGMCU->CR %x\n", dataRead);
}