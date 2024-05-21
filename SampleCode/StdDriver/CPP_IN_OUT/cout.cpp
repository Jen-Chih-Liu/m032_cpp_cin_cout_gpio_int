#include <stdio.h>
#include "NuMicro.h"


#pragma import(__use_no_semihosting) // 禁用半主机模式

// 请不要勾选Use MicroLib
#ifdef __MICROLIB
#error "Please do not use MicroLib!"
#endif
#define DEBUG_PORT   UART0
extern "C"
{
	static void SendChar_ToUART(int ch)
{
    while(DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);

    if(ch == '\n')
    {
        DEBUG_PORT->DAT = '\r';
        while(DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
    }
    DEBUG_PORT->DAT = ch;
}
	
  void _sys_exit(int returncode)
  {
    printf("Exited! returncode=%d\n", returncode);
    while (1);
  }
  
  void _ttywrch(int ch)
  {
    SendChar_ToUART(ch);
  }
}

namespace std
{
  struct __FILE
  {
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
  };
  
  FILE __stdin = {0};
  FILE __stdout = {1};
  FILE __stderr = {2};
  static char GetChar(void)
{
	    while(1)
    {
        if((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0)
        {
            return (DEBUG_PORT->DAT);
        }
    }
	}
	
  int fgetc(FILE *stream)
  {
   return (GetChar());
  }
  
  int fputc(int c, FILE *stream)
  {
    if (stream->handle == 1 || stream->handle == 2)
    {
      _ttywrch(c);
      return c;
    }
    return EOF;
  }
  
  int fclose(FILE * stream)
  {
    return 0;
  }
  
  int fseek(FILE *stream, long int offset, int whence)
  {
    return -1;
  }
  
  int fflush(FILE *stream)
  {
    if (stream->handle == 1 || stream->handle == 2)
		{
		while(DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
		}   
    return 0;
  }
}
