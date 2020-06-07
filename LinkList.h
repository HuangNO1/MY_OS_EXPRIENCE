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
// process status
#define EXECUTE 1
#define HANG 2
#define FINISHED 3
// struct
// 頁面
typedef struct Page
{
    // 頁號
    int index;
    // flag 用來判斷是否被修改過 如果有修改就是 1 沒有就是 0
    // 在頁表中使用基於 LRU 改進的 Clock 時會用到
    int flag;
    // 頁框號 - 作為 偏移量
    int frameNumber;
    // 外存地址
    int externalStorageAddress;
    // 判斷使是要放到虛擬內存跑
    int valid;
    // 狀態 - 判斷 該頁面是否 未完成(0) 完成(1)
    int status;
    // next
    struct Page *next;
} Page, *PagesLinkList;

typedef struct Process
{
    // 進程號
    int index;
    // 頁表
    struct Page *pagesLinkList;
    // 優先級 - 在時間片輪詢法中用到的優先級
    int priority;
    // 狀態 - 判斷 該進程是否 進入執行(1) 掛起(2) 完成(3)
    int status;
    // 執行長度 - 表示該進程的執行進度，執行
    int length;
    // 缺頁次數
    int res;
    // next
    struct Process *next;
} Process, *ProcessesLinkList;

// 初始化 Page
int InitPageList(PagesLinkList *PageL)
{
    // 產生頭指針，並將 PageL 指向 此頭指針
    *PageL = (PagesLinkList)malloc(sizeof(Page));
    // 如果存儲分配失敗
    if (*PageL == NULL)
    {
        printf("\nInit page list fail.\n");
        return ERROR;
    }
    // next 的指針域為空
    (*PageL)->next = NULL;
    printf("\nInit page list success.\n");
    return OK;
}

// 銷毀 Page 鏈表
void DestoryPageList(PagesLinkList PageL)
{
    PagesLinkList p, q;
    // p 指向第一個結點
    p = (PageL)->next;
    // 沒到表尾
    while (p)
    {
        q = (p)->next;
        free(p);
        p = q;
    }
    // 釋放頭結點空間
    free(PageL);
    printf("\nDestory page success.\n");
}

// 清除 Page 鏈表
int ClearPageList(PagesLinkList PageL)
{
    PagesLinkList p, q;
    // 指向第一個結點
    p = (PageL)->next;
    // 沒到表尾
    while (p)
    {
        q = (p)->next;
        free(p);
        p = q;
    }
    (PageL)->next = NULL;
    printf("\nClear page success.\n");
    return OK;
}

// 檢查 Page 鏈表是否為空
int PageListEmpty(PagesLinkList PageL)
{
    return (PageL->next == NULL) ? TRUE : FALSE;
}

// 返回 Page 鏈表長度
int PageListLength(PagesLinkList PageL)
{
    int length = 0;
    PagesLinkList p = PageL->next;
    while (p)
    {
        length += 1;
        p = p->next;
    }
    return length;
}

// 插入新的 Page
int PageListInsert(PagesLinkList PageL, int index, int flag, int frameNumber, int externalStorageAddress, int valid, int status)
{
    PagesLinkList p, s;
    p = PageL;
    int j = 0;
    // 尋找 index - 1 的結點
    while (p && j < index - 1)
    {
        p = (p)->next;
        j += 1;
    }
    // 第 i 個元素不存在
    if (!p || j > index - 1)
    {
        return ERROR;
    }
    s = (PagesLinkList)malloc(sizeof(Page));
    // 賦值
    s->index = index;
    s->flag = flag;
    s->frameNumber = frameNumber;
    s->externalStorageAddress = externalStorageAddress;
    s->valid = valid;
    s->status = status;
    // next
    s->next = p->next;
    p->next = s;
    printf("\nInsert page success.\n");
    return OK;
}

// -------------------------------------------------------------------

// 初始化 ProcessList
int InitProcessList(ProcessesLinkList *ProcessL)
{
    // 產生頭指針，並將 ProcessL 指向 此頭指針
    *ProcessL = (ProcessesLinkList)malloc(sizeof(Process));
    // 如果存儲分配失敗
    if (*ProcessL == NULL)
    {
        printf("\nInit process list fail.\n");
        return ERROR;
    }
    // next 的指針域為空
    (*ProcessL)->next = NULL;
    printf("\nInit process list success.\n");
    return OK;
}

// 銷毀 Processes 鏈表
void DestoryProcessList(ProcessesLinkList ProcessL)
{
    ProcessesLinkList p, q;
    // p 指向第一個結點
    p = (ProcessL)->next;
    // 沒到表尾
    while (p)
    {
        q = (p)->next;
        // 也釋放 pages
        DestoryPageList(p->pagesLinkList);
        free(p);
        p = q;
    }
    // 釋放頭結點的 Page
    free(ProcessL->pagesLinkList);
    // 釋放頭結點空間
    free(ProcessL);
    printf("\nDestory process list success.\n");
}

// 清除 ProcessL 鏈表
int ClearProcessList(ProcessesLinkList ProcessL)
{
    ProcessesLinkList p, q;
    // 指向第一個結點
    p = (ProcessL)->next;
    // 沒到表尾
    while (p)
    {
        q = (p)->next;
        // 也釋放 pages
        DestoryPageList(p->pagesLinkList);
        free(p);
        p = q;
    }
    (ProcessL)->next = NULL;
    printf("\nClear process list success.\n");
    return OK;
}

// 檢查 ProcessL 鏈表是否為空
int ProcessListEmpty(ProcessesLinkList ProcessL)
{
    return (ProcessL->next == NULL) ? TRUE : FALSE;
}

// 返回 ProcessL 鏈表長度
int ProcessListLength(ProcessesLinkList ProcessL)
{
    int length = 0;
    ProcessesLinkList p = ProcessL->next;
    while (p)
    {
        length += 1;
        p = p->next;
    }
    return length;
}

// 插入新的 進程
int ProcessListInsert(ProcessesLinkList ProcessL, int index, int priority, int status, PagesLinkList PageL)
{
    ProcessesLinkList p, s;
    p = ProcessL;
    int j = 0;
    // 尋找 index - 1 的結點
    while (p && j < index - 1)
    {
        p = (p)->next;
        j += 1;
    }
    // 第 i 個元素不存在
    if (!p || j > index - 1)
    {
        return ERROR;
    }
    s = (ProcessesLinkList)malloc(sizeof(Process));
    // 賦值
    s->index = index;
    s->length = 0;
    s->priority = priority;
    s->status = status;
    s->pagesLinkList = PageL;
    s->res = 0;
    // next
    s->next = p->next;
    p->next = s;
    printf("\nInsert into the process list successfully.\n");
}