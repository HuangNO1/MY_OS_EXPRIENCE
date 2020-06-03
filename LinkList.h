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
    // 在頁表中使用基於 LRU 改進的 Clock 時會用到
    int flag;
    // 頁框號 - 作為 偏移量
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
    struct Page *pagesLinkList;
    // 優先級 - 在時間片輪詢法中用到的優先級
    int priority;
    // 狀態 - 判斷 該進程是否 進入執行(1) 掛起(2) 完成(3)
    int status;
    // 執行長度 - 表示該進程的執行進度
    int length;
    // next
    struct Process *next;
} Process;

typedef struct Page *PagesLinkList;
typedef struct Process *ProcessesLinkList;

// 初始化 Page
int InitPageList(PagesLinkList *PageL)
{
    // 產生頭指針，並將 ProcessL 指向 此頭指針
    *PageL = (PagesLinkList)malloc(sizeof(Page));
    // 如果存儲分配失敗
    if (*PageL == NULL)
    {
        printf("Init process list fail.");
        return ERROR;
    }
    // next 的指針域為空
    (*PageL)->next = NULL;
    printf("Init process list success.");
    return OK;
}

// 銷毀 Page 鏈表
void DestoryPageList(PagesLinkList *PageL)
{
    PagesLinkList p, q;
    // p 指向第一個結點
    p = (*PageL)->next;
    // 沒到表尾
    while (p)
    {
        q = p->next;
        free(p);
        p = q;
    }
    // 釋放頭結點空間
    free(*PageL);
}

// 清除 Page 鏈表
int ClearPageList(PagesLinkList *PageL)
{
    PagesLinkList p, q;
    // 指向第一個結點
    p = (*PageL)->next;
    // 沒到表尾
    while (p)
    {
        q = p->next;
        free(p);
        p = q;
    }
    (*PageL)->next = NULL;
    return OK;
}

// 檢查 Page 鏈表是否為空
int PageListEmpty(PagesLinkList PageL)
{
    return (PageL->next == NULL) ? TRUE : FALSE;
}

// 返回 Page 鏈表長度
int PageListLength(PagesLinkList PageL) {
    int length = 0;
    PagesLinkList p = PageL->next;
    while (p)
    {
        length += 1;
        p = p->next;
    }
    return length;
}

// 初始化 ProcessList
int InitProcessList(ProcessesLinkList *ProcessL)
{
    // 產生頭指針，並將 ProcessL 指向 此頭指針
    *ProcessL = (ProcessesLinkList)malloc(sizeof(Process));
    // 如果存儲分配失敗
    if (*ProcessL == NULL)
    {
        printf("Init process list fail.");
        return ERROR;
    }
    // next 的指針域為空
    (*ProcessL)->next = NULL;
    printf("Init process list success.");
    return OK;
}
