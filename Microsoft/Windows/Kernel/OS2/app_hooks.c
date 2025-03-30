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
*                                          Application Hooks
*
* Filename : app_hooks.c
* Version  : V2.92.13
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>


/*
*********************************************************************************************************
*                                      EXTERN  GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


void OutFileInit() {
    /*Clear the file*/
    if ((Output_err = fopen_s(&Output_fp, OUTPUT_FILE_NAME, "w")) == 0)
        fclose(Output_fp);
    else
        printf("Error to clear output file");
}

void InputFile() {
    errno_t err;
    if ((err = fopen_s(&fp, INPUT_FILE_NAME, "r")) != 0) {
        //printf("The file '%s' was opened\n", INPUT_FILE_NAME);
        printf("The file '%s' was not opened\n", INPUT_FILE_NAME);
    }
    
    char str[MAX];
    char* ptr;
    char* pTmp = NULL;
    int TaskInfo[INFO], i, j = 0;
    TASK_NUMBER = 0;
    while (!feof(fp)) {
        i = 0;
        memset(str, 0, sizeof(str));
        fgets(str, sizeof(str) - 1, fp);
        ptr = strtok_s(str, " ", &pTmp);
        while (ptr != NULL) {
            TaskInfo[i] = atoi(ptr);
            ptr = strtok_s(NULL, " ", &pTmp);
            if (i == 0) {
                TASK_NUMBER++;
                TaskParameter[j].TaskID = TaskInfo[i]; //task編號
            }
            else if (i == 1) {
				TaskParameter[j].TaskArriveTime = TaskInfo[i]; //到達時間
            }
            else if (i == 2) {
                TaskParameter[j].TaskExecutionTime = TaskInfo[i]; //執行時間
            }
            else if (i == 3) {
				TaskParameter[j].TaskPeriodic = TaskInfo[i]; //週期:幾個tick執行一次
            }
            i++;
        }
        TaskParameter[j].TaskNumber = 0;  //TaskNumber save the task do how many times
		TaskParameter[j].TaskPriority = j + 1;  //task priority
		TaskParameter[j].Task_need_ExecutionTime = TaskParameter[j].TaskExecutionTime;  //task need execution time
        j++;
    }
    fclose(fp);

    
}
/*
*********************************************************************************************************
*********************************************************************************************************
**                                        uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/*
*********************************************************************************************************
*                                  TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}


/*
*********************************************************************************************************
*                                  TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}


/*
*********************************************************************************************************
*                                    IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


/*
*********************************************************************************************************
*                                  STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}


/*
*********************************************************************************************************
*                                   TASK RETURN HOOK (APPLICATION)
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/


#if OS_VERSION >= 289
void  App_TaskReturnHook (OS_TCB  *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                   TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
    if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
        if (OSTime) {
            printf("%d", OSTime);
            fprintf(Output_fp, "%d", OSTime);
            if (TaskParameter[OSPrioCur - 1].Task_need_ExecutionTime == 0 && OSPrioCur != OS_TASK_IDLE_PRIO) { //如果要被切換進來的task是idle task，就print這行
                printf("\tCompletion\t");  //if the task will be done, show this line
                fprintf(Output_fp,"\tCompletion\t");
            }
            else {
                printf("\tPreemption\t");
                fprintf(Output_fp, "\tPreemption\t");
            }

            if (OSPrioCur == OS_TASK_IDLE_PRIO) { //如果要被切換出去的task是idle task，就print這行
                printf("task(%2d)\t\ttask(%2d)(%2d)", OSPrioCur, TaskParameter[OSPrioHighRdy - 1].TaskID, TaskParameter[OSPrioHighRdy - 1].TaskNumber);  //if the task being switched out is idle task, show this line
                fprintf(Output_fp, "task(%2d)\ttask(%2d)(%2d)", OSPrioCur, TaskParameter[OSPrioHighRdy - 1].TaskID, TaskParameter[OSPrioHighRdy - 1].TaskNumber);
            }
            else if (OSPrioHighRdy == OS_TASK_IDLE_PRIO) { //如果要被切換進來的task是idle task，就print這行
                printf("task(%2d)(%2d)\t\ttask(%2d)", TaskParameter[OSPrioCur - 1].TaskID, TaskParameter[OSPrioCur - 1].TaskNumber, OSPrioHighRdy);  //if the task will be switched in is idle task, show this line
                fprintf(Output_fp, "task(%2d)(%2d)\ttask(%2d)", TaskParameter[OSPrioCur - 1].TaskID, TaskParameter[OSPrioCur - 1].TaskNumber, OSPrioHighRdy);
            }
            else { //如果兩個task在做context switch，就print這行  
                printf("task(%2d)(%2d)\t\ttask(%2d)(%2d)", TaskParameter[OSPrioCur - 1].TaskID, TaskParameter[OSPrioCur - 1].TaskNumber, TaskParameter[OSPrioHighRdy - 1].TaskID, TaskParameter[OSPrioHighRdy - 1].TaskNumber);  //if the two task doing context switch, show this line
                fprintf(Output_fp, "task(%2d)(%2d)\ttask(%2d)(%2d)", TaskParameter[OSPrioCur - 1].TaskID, TaskParameter[OSPrioCur - 1].TaskNumber, TaskParameter[OSPrioHighRdy - 1].TaskID, TaskParameter[OSPrioHighRdy - 1].TaskNumber);
            }
        
            if (TaskParameter[OSPrioCur -1].Task_need_ExecutionTime == 0 && OSPrioCur != OS_TASK_IDLE_PRIO) { //done
                int response_time   = OSTime - TaskParameter[OSPrioCur - 1].TaskNumber * TaskParameter[OSPrioCur - 1].TaskPeriodic - TaskParameter[OSPrioCur - 1].TaskArriveTime;
				int preemption_time = response_time - TaskParameter[OSPrioCur - 1].TaskExecutionTime;
				int OSTimeDly       = TaskParameter[OSPrioCur - 1].TaskPeriodic - response_time;
                printf("\t%d\t\t%d\t\t%d\n", response_time, preemption_time, OSTimeDly);
                fprintf(Output_fp, "\t%d\t\t%d\t\t\t%d\n", response_time, preemption_time, OSTimeDly);
                
                //記錄這個task做幾次
                TaskParameter[OSPrioCur - 1].TaskNumber++;
                TaskParameter[OSPrioCur - 1].Task_need_ExecutionTime = TaskParameter[OSPrioCur - 1].TaskExecutionTime;
            
                //檢查Missdeadline
                for (int i = 0; i < TASK_NUMBER; i++) {
                    int response_time_i = OSTime - TaskParameter[i].TaskNumber * TaskParameter[i].TaskPeriodic - TaskParameter[i].TaskArriveTime;
                    int OSTimeDly_i = TaskParameter[i].TaskPeriodic - response_time_i;
                    if (OSTimeDly_i <= 0 && TaskParameter[OSPrioCur - 1].Task_need_ExecutionTime != 0) {

                        printf("%d\tMissDeadline\ttask( %d)( %d)\t\t----------------- \n", OSTime, TaskParameter[i].TaskID, TaskParameter[i].TaskNumber);
                        fprintf(Output_fp, "%d\tMissDeadline\ttask( %d)( %d)\t\t----------------- \n", OSTime, TaskParameter[i].TaskID, TaskParameter[i].TaskNumber);
                        OSRunning = OS_FALSE;
                        fclose(Output_fp);
                        exit(0);
                    }
                }


            
            }
            else {
                printf("\n");
				fprintf(Output_fp, "\n");
            }
        
        
        }
    }
	fclose(Output_fp);
    

}
#endif


/*
*********************************************************************************************************
*                                   OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                       TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
