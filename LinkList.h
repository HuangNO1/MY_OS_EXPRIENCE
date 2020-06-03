#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
// define global constant number
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2
// struct
// 頁面
typedef struct Page
{
    // 頁號
    int index;
    // flag 用來判斷是否被修改過 如果有修改就是 1 沒有就是 0
    int flag;
    // 頁框號 作為 偏移量
    int frameNumber;
    // 外存地址
    int externalStorageAddress;
    // next
    struct Page *next;
} Page;

typedef struct Process
{
    // 進程號
    int index;
    // 頁表
    typedef struct Page *pagesLinkList;
    // 優先級
    int priority;
    // next
    struct Process *next;
} Process;

typedef struct Process *ProcessesLinkList;

int InitProcessList(ProcessesLinkList &ProcessL)
{
    // 產生頭指針，並將 ProcessL 指向 此頭指針
    ProcessL = (ProcessesLinkList)malloc(sizeof(Process));
    // 如果存儲分配失敗
    if (ProcessL == NULL)
    {
        return ERROR;
    }
    // next 的指針域為空
    (ProcessL)->next = NULL;
    return OK;
}
