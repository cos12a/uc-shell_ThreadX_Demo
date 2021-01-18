
 
 /*
 *********************************************************************************************************
 *                                            INCLUDE FILES
 *********************************************************************************************************
 */
 
 #include  <terminal.h>
 
#include   "tx_api.h"

#include "sample_cfg.h"
 
 
 /*
 *********************************************************************************************************
 *                                            LOCAL DEFINES
 *********************************************************************************************************
 */
 
 
 /*
 *********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

 extern TX_BYTE_POOL            byte_pool_0;
// static  CPU_STK  Terminal_OS_TaskStk[TERMINAL_OS_CFG_TASK_STK_SIZE];
 static  TX_THREAD   Terminal_OS_Thread;
 
 
 /*
 *********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************
 */
 
 static  void  Terminal_OS_Task(ULONG thread_input);
 
 
 /*
 *********************************************************************************************************
 *                                         Terminal_OS_Task()
 *
 * Description : RTOS interface for terminal main loop.
 *
 * Argument(s) : p_arg       Argument to pass to the task.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : RTOS.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */
 static  void  Terminal_OS_Task (ULONG thread_input)
 {
     Terminal_Task(thread_input);
 }
 
 
 /*
 *********************************************************************************************************
 *                                         Terminal_OS_Init()
 *
 * Description : Initialize the terminal task.
 *
 * Argument(s) : p_arg       Argument to pass to the task.
 *
 * Return(s)   : DEF_FAIL    Initialize task failed.
 *               DEF_OK      Initialize task successful.
 *
 * Caller(s)   : Terminal_Init()
 *
 * Note(s)     : The RTOS needs to create Terminal_OS_Task().
 *********************************************************************************************************
 */
 
 CPU_BOOLEAN  Terminal_OS_Init (void *p_arg)
 {
    UINT  os_err;
    CHAR    *pointer = TX_NULL;
    UINT  reslt;

    /* Allocate the stack for thread 1.  */
    reslt = tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, TERMINAL_OS_CFG_TASK_STK_SIZE, TX_NO_WAIT);
    if ( reslt != TX_SUCCESS){        
        Error_Handler();
    }

    reslt = tx_thread_create(&Terminal_OS_Thread, "Terminal", Terminal_OS_Task, 1,  
                        pointer, TERMINAL_OS_CFG_TASK_STK_SIZE, 
                        TERMINAL_OS_CFG_TASK_PRIO, TERMINAL_OS_CFG_TASK_THRESHOLD_PRIO, 4, TX_AUTO_START);

 
     if (reslt != TX_SUCCESS) {
         return (DEF_FAIL);
     }
 
     return (DEF_OK);
 }




