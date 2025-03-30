/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"


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

#define TASK_STACKSIZE              2048
static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);
static  void  task(void* p_arg);
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/
int count1 = 0;
int count2 = 0;
int count = 0;
int idle = 0;

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif



    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();
    OutFileInit();


    InputFile();


    Task_STK = malloc(TASK_NUMBER * sizeof(int*));


    int n;
    for (n = 0; n < TASK_NUMBER; n++) {
        Task_STK[n] = malloc(TASK_STACKSIZE * sizeof(int));
    }
    
    //                                                   /* Initialize uC/OS-II                                  */
    /*if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
        fprintf(Output_fp, "Tick: %2d, Hello from task%2d\n", OSTime);
        fclose(Output_fp);
    }*/

    /*int number = 1;
    count = 0;
    printf("Tick\t\tCurrentTask ID\t\tNextTask ID\t\tNumber of ctx switch\n");
    printf("%*d      ***********             task(%2d)(%2d)            %2d\n",2,OSTime, number, count1, count1);
    if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
        fprintf(Output_fp, "%*d      ***********             task(%2d)(%2d)            %2d\n", 2, OSTime, number, count1, count1);
        fclose(Output_fp);
    }*/
    OSTimeSet(0);
	//PA1-3 RM sheduling
	/*for (int i = 0; i < TASK_NUMBER; i++) {
		printf("Task %d: %d PRIO: %d \n", TaskParameter[i].TaskID, TaskParameter[i].TaskPeriodic, TaskParameter[i].TaskPriority);
	}*/

    //PA1-2 read task file
    for (auto i = 0;i < TASK_NUMBER;i++) {
            OSTaskCreateExt(task,                               /* Create the startup task                              */
                &TaskParameter[i],
                &Task_STK[i][TASK_STACKSIZE - 1],
                TaskParameter[i].TaskPriority,
                TaskParameter[i].TaskID,
                &Task_STK[i][0],
                TASK_STACKSIZE,
                &TaskParameter[i],
                (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));  
    }
    
    //PA1-1 TCB linked list
    printf("================TCB linked list================\n");
    printf("Task\tPrev_TCB_addr\tTCB_addr\tNext_TCB_addr\n");
    OS_TCB* current = OSTCBList;
    while (current) {
        if (current->OSTCBPrio == OS_TASK_IDLE_PRIO) {
            printf("%d\t%x\t\t%x\t\t%x\n", current->OSTCBPrio, current->OSTCBPrev, current, current->OSTCBNext);
        }
        else {
            printf("%d\t%x\t\t%x\t\t%x\n", current->OSTCBId, current->OSTCBPrev, current, current->OSTCBNext);
        }
        current = current->OSTCBNext;
    }

    printf("Tick\tEvent\t\tCurrentTaskID\t\tNextTask ID\tResponse Time\tPreemption Time\tOSTimeDly\n");
    
	fclose(Output_fp);


    OSStart();
                                               /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}


/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

//static  void  StartupTask (void *p_arg)
//{
//   (void)p_arg;
//
//    OS_TRACE_INIT();                                            /* Initialize the uC/OS-II Trace recorder               */
//
//#if OS_CFG_STAT_TASK_EN > 0u
//    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
//#endif
//
//#ifdef CPU_CFG_INT_DIS_MEAS_EN
//    CPU_IntDisMeasMaxCurReset();
//#endif
//    
//    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));
//
//    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
//        OSTimeDlyHMSM(0u, 0u, 0u, 500u);
//		APP_TRACE_DBG(("Time: %2d\n\r", OSTimeGet()));
//    }
//}


void task(void* p_arg) {
    task_para_set* task_data;
    task_data = (task_para_set*)p_arg;
    //printf("TICK %d task_data: %d\n",OSTime, task_data->TaskID);
    // 初始到達時間延遲
    if (OSTime < task_data->TaskArriveTime) {
        //printf("TICK %d task_data: %d delay %d\n", OSTime, task_data->TaskID , task_data->TaskArriveTime - OSTime);
        OSTimeDly(task_data->TaskArriveTime - OSTime);
    }

    INT32U next_period = 0;
    while (1) {
		next_period = task_data->TaskPeriodic * (task_data->TaskNumber+1) + task_data->TaskArriveTime;
        //printf("delay: %d\n", task_data->TaskPeriodic - time_since_start - delay_start);
        if (task_data->Task_need_ExecutionTime==0) {
            if (next_period - OSTime > 0u) {
                OSTimeDly(next_period - OSTime);
            }
            else {
                OSTaskSwHook();
            }
        }
        
    }
}

