/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include <iostream>
#include <typeinfo>
#include <vector>
#include <stdio.h>
//#include "common.hpp"

using namespace std;

class Test
{
  private:
    int num;
  public:
    Test(int num) : num(num) {}
    
    void display(void)
    {
      cout << "num=" << num << endl;
    }
};

template <typename T>
static void further_test(T a)
{
  cout << "[" << typeid(a).name() << "] a=" << a << endl;
}


static void str_replace(string search, string replace, string &subject)
{
  string::size_type pos = 0;
  
  for (pos = 0; (pos = subject.find(search, pos)) != string::npos; pos += replace.length())
    subject.replace(pos, search.length(), replace);
}

static void further_test(void)
{
  // string
  string str = "this is";
  str.append(" a string");
  str_replace("is", "[IS]", str);
  cout << str << " len=" << str.length() << endl;
  
  // vectorÈÝÆ÷
  vector<string> items;
  vector<string>::iterator iter;
  items.push_back("fooo");
  items.push_back("bar");
  items.push_back("welcome");
  cout << "size=" << items.size() << endl;
  for (iter = items.begin(); iter != items.end(); ++iter)
    cout << *iter << " ";
  cout << endl;
  

  int i;
  int n = items.size();
  int *arr = new int[n];
  for (i = 0; i < n; i++)
    arr[i] = items.at(i).length();
  for (i = 0; i < n; i++)
    cout << arr[i] << " ";
  cout << endl;
  delete[] arr;
  

  Test t = 50;
  t.display();
  

  Test &u = t;
  u.display();
  
  printf("Please input: ");
  scanf("%d", &n);
  printf("2n=%d\n", 2 * n);
  
  cout << "Please input: ";
  cin >> n;
  cout << "3n=" << 3 * n << endl;
}
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*----------------------------------------------------------------------*/
/* Init UART0                                                           */
/*----------------------------------------------------------------------*/
void UART0_Init(void)
{
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}


/**
 * @brief       GPIO PA/PB/PG/PH IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PA/PB/PG/PH default IRQ, declared in startup_M031Series.s.
 */
extern "C" void GPABGH_IRQHandler(void)
{
    volatile uint32_t temp;

    /* To check if PB.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT2))
    {
        GPIO_CLR_INT_FLAG(PB, BIT2);
        printf("PB.2 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PB interrupts */
        temp = PB->INTSRC;
        PB->INTSRC = temp;
        printf("Un-expected interrupts.\n");
    }
}



int32_t main(void)
{


    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();
 
  printf("m031 printf\n");
  printf("SystemCoreClock=%u\n", SystemCoreClock);
  
  cout << "m031 cout" << endl;
  cout << "SystemCoreClock=" << SystemCoreClock << endl;
  
  further_test(15);
  further_test("this");
  further_test();
   /* Configure PB.2 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PB, BIT2, GPIO_MODE_QUASI);
    GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);



    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PB, BIT2);
 

    while(1);
}
