#include <stdio.h>
#include "LinkList.h"
#include <ctype.h>
// 定義頁表長為 4
#define PageLength 4

// 找到 [最大優先級] 且 [未執行完成] 的進程並返回
ProcessesLinkList findMaxPriority(ProcessesLinkList ProcessL, ProcessesLinkList currentP)
{
    ProcessesLinkList p, max;
    // 指向第一個節點
    p = ProcessL->next;

    int maxPriority = 0;
    // 如果 current P 有指向有分配空間的 進程
    if (currentP)
    {
        maxPriority = currentP->priority;
        max = currentP;
        // 找出優先度最高的進程
        while (p)
        {
            if (p->priority > currentP->priority && p->status != FINISHED)
            {
                // 將指針指向最優先級最大的
                max = p;
                maxPriority = p->priority;
            }
            p = p->next;
        }
    }
    else
    {
        // 找出優先度最高的進程
        while (p)
        {
            if (p->priority > maxPriority && p->status != FINISHED)
            {
                // 將指針指向最優先級最大的
                max = p;
                maxPriority = p->priority;
            }
            p = p->next;
        }
    }

    return max;
}

// 用來檢查頁訪問是否已經在頁表了
int check_is_already_in_page(PagesLinkList PhysicalM, PagesLinkList VirtualM, int *record, int *recordIndex, PagesLinkList q, ProcessesLinkList p)
{
    PagesLinkList checkPhysical, checkVirtual;

    checkPhysical = PhysicalM->next;
    checkVirtual = VirtualM->next;

    // 判斷該頁訪問是否已經在頁表裡面
    int flag = FALSE;
    for (int i = 0; i < PageLength; i++)
    {
        if (q->frameNumber == record[i])
        {
            // 有存在
            flag = TRUE;
            // 保存當前尋找位置
            *recordIndex = i;
            break;
        }
    }

    // init
    checkPhysical = PhysicalM->next;
    checkVirtual = VirtualM->next;
    // 如果已經在了
    if (flag)
    {
        // 將頁的 flag 設為 1 表示修改過
        // 遍歷每個內存
        while (checkPhysical)
        {
            if (checkPhysical->frameNumber == q->frameNumber)
            {
                checkPhysical->flag = 1;
                // 完成度 +1
                p->length += 1;
            }
            checkPhysical = checkPhysical->next;
        }
        while (checkVirtual)
        {
            if (checkVirtual->frameNumber == q->frameNumber)
            {
                checkVirtual->flag = 1;
                // 完成度 +1
                p->length += 1;
            }
            checkVirtual = checkVirtual->next;
        }
        // 表示已經訪問
        q->status = TRUE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// 改變優先度
void change_priority(ProcessesLinkList ProcessL, ProcessesLinkList currentPL)
{
    // 指向頭節點
    ProcessesLinkList p = ProcessL->next;
    // 將目前的優先度減一
    if (currentPL->status != FINISHED)
    {
        currentPL->priority -= 1;
    }
    // 將每個 HANG 的 Process 增加 1
    while (p)
    {
        if (p->status == HANG)
        {
            p->priority += 1;
        }
        p = p->next;
    }
}

// 判斷所有進程是否都已經完成了
int check_is_all_processes_finished(ProcessesLinkList ProcessL)
{
    ProcessesLinkList checkFinish;
    checkFinish = ProcessL->next;
    int flag = TRUE;
    while (checkFinish)
    {
        // 如果找到任何沒有完成的
        if (checkFinish->status != FINISHED)
        {
            flag = FALSE;
            break;
        }
        checkFinish = checkFinish->next;
    }
    return (flag == TRUE) ? TRUE : FALSE;
}

// CLOCK
void clock_page_mamager(ProcessesLinkList p, PagesLinkList q, PagesLinkList PhysicalM, PagesLinkList VirtualM, int *record, int *recordIndex)
{
    // 利用頁表最大長度將 優先級最大的進程 頁 填入 Memory 執行
    // 限制頁表的大小
    // clock 調頁算法
    int sum = PageListLength(PhysicalM) + PageListLength(VirtualM);
    // 頁表是否已被執行，如果沒被執行就填入
    if (q->status == FALSE)
    {
        // 頁表沒裝滿
        if (sum < PageLength)
        {
            // 先判斷是否有重複的
            int flag = check_is_already_in_page(PhysicalM, VirtualM, record, &recordIndex, q, p);

            if (flag == FALSE)
            {
                // 填入
                // 放入物理內存
                if (q->valid == TRUE)
                {
                    // flag 輸入 1 表示修改過，status 設成 TRUE 表示執行過
                    PageListInsert(PhysicalM, 1, 1, q->frameNumber, q->externalStorageAddress, q->valid, TRUE);
                    q->status = TRUE;
                    // 完成度 +1
                    p->length += 1;
                }
                // 放入虛擬內存
                else
                {
                    // flag 輸入 1
                    PageListInsert(VirtualM, 1, 1, q->frameNumber, q->externalStorageAddress, q->valid, TRUE);
                    q->status = TRUE;
                    // 完成度 +1
                    p->length += 1;
                }
                // 填入紀錄
                for (int i = 0; i < PageLength; i++)
                {
                    if (record[i] == 0)
                    {
                        // 紀錄 frameNumber
                        record[i] = q->frameNumber;
                        break;
                    }
                }
            }
        }
        // 已經裝滿了檢查是否 下一個 頁訪問 已在內存裡面
        else if (sum == PageLength)
        {
            int isExist = FALSE;
            PagesLinkList checkPhysical, checkVirtual;
            // 不存在，產生 [斷頁]
            int flag = check_is_already_in_page(PhysicalM, VirtualM, record, &recordIndex, q, p);

            if (flag == FALSE)
            {
                // 缺頁次數加一
                p->res += 1;
                // 無限直到找到可以移出去的頁
                int findRemove = FALSE;
                // 從 recordIndex 的地方開始找
                for (int i = *recordIndex; i < PageLength; i++)
                {
                    // init
                    checkPhysical = PhysicalM->next;
                    checkVirtual = VirtualM->next;
                    // 先找到 對應頁表的頁
                    while (checkPhysical)
                    {
                        // printf("\n Debug checkPhysical\n");
                        // for (int i = 0; i < PageLength; i++)
                        // {
                        //     printf("%d  ", record[i]);
                        // }
                        // printf("\n\n");
                        // printf("record[%d]: %d, frameNumber: %d\n",i , record[i], checkPhysical->frameNumber);
                        // printf("adjust : %d", record[i] == checkPhysical->frameNumber);
                        if (record[i] == checkPhysical->frameNumber)
                        {
                            // 修改位 TRUE -> FALSE
                            if (checkPhysical->flag == TRUE)
                            {
                                // 改變狀態
                                checkPhysical->flag = FALSE;
                                checkPhysical = checkPhysical->next;
                            }
                            else
                            {
                                // 有找到 flag 位為 0 的頁
                                // 產生頁置換
                                // 修改資料
                                checkPhysical->flag = 1;
                                checkPhysical->frameNumber = q->frameNumber;
                                checkPhysical->externalStorageAddress = q->externalStorageAddress;

                                *recordIndex = i;
                                record[i] = checkPhysical->frameNumber;
                                // 將 find Remove 設為 TRUE 結束循環
                                findRemove = TRUE;
                                // 完成度 +1
                                p->length += 1;
                                break;
                            }
                        }
                        else if (record[i] != checkPhysical->frameNumber)
                        {
                            checkPhysical = checkPhysical->next;
                        }
                    }
                    while (checkVirtual)
                    {
                        // 修改位 TRUE -> FALSE
                        if (record[i] == checkVirtual->frameNumber)
                        {
                            if (checkVirtual->flag == TRUE)
                            {
                                // 改變狀態
                                checkVirtual->flag = FALSE;
                                checkVirtual = checkVirtual->next;
                            }
                            else
                            {
                                // 有找到 flag 位為 0 的頁
                                // 產生頁置換
                                // 修改資料
                                checkVirtual->flag = 1;
                                checkVirtual->frameNumber = q->frameNumber;
                                checkVirtual->externalStorageAddress = q->externalStorageAddress;

                                *recordIndex = i;
                                record[i] = checkPhysical->frameNumber;
                                // 將 find Remove 設為 TRUE 結束循環
                                findRemove = TRUE;
                                // 完成度 +1
                                p->length += 1;
                                break;
                            }
                        }
                        else if (record[i] != checkVirtual->frameNumber)
                        {
                            checkVirtual = checkVirtual->next;
                        }
                    }
                    // 如果有置換成功
                    if (findRemove == TRUE)
                    {
                        break;
                    }
                    // 如果沒有找到置換點
                    if (i == PageLength - 1)
                    {
                        i = -1;
                    }
                }
            }
        }
    }
}

// 輸出 狀態
void print_process_page(int *record, ProcessesLinkList p)
{
    printf("\ncurrent process: %d, priority: %d\n\npage list: ", p->index, p->priority);
    for (int i = 0; i < PageLength; i++)
    {
        printf("%d  ", record[i]);
    }
    printf("\n\n");
}

// 時間片輪詢法 進程調度
void timing_task_scheduler(ProcessesLinkList ProcessL, PagesLinkList PhysicalM, PagesLinkList VirtualM)
{
    ProcessesLinkList p;
    PagesLinkList q;

    // 先得到最大的優先級
    p = findMaxPriority(ProcessL);
    // 設定 status 為 運行中
    p->status = EXECUTE;

    // 用來記錄頁表
    int record[PageLength] = {};
    // 用來判斷 Clock 演算法調度的檢查位置
    int recordIndex = 0;
    q = p->pagesLinkList->next;
    // 開始進行執行的進程
    while (TRUE)
    {
        printf("\nBefore");
        print_process_page(record, p);

        // 都完成了，全部中止
        if (check_is_all_processes_finished(ProcessL) == TRUE)
        {
            // 刪除所有進程
            ClearProcessList(ProcessL);
            break;
        }

        // clock 調度頁
        clock_page_mamager(p, q, PhysicalM, VirtualM, record, &recordIndex);

        printf("\nAfter");
        print_process_page(record, p);

        // 先判斷是不是已經完成了
        // 如果變量相同
        if (p->length == PageListLength(p->pagesLinkList))
        {
            // 表示該進程已經完成了
            // 改變 status
            p->status = FINISHED;
            printf("\nThe %d process has finished.\n", p->index);
            printf("Page break times: %d, rate: %.2f %%\n\n", p->res, (1 - p->res * 1.0 / p->length) * 100);
        }

        // 先改變優先度
        change_priority(ProcessL, p);

        // 都完成了，全部中止
        if (check_is_all_processes_finished(ProcessL) == TRUE)
        {
            // 刪除所有進程
            ClearProcessList(ProcessL);
            break;
        }

        // 獲取當前最大優先度進程
        ProcessesLinkList pSwap;
        pSwap = findMaxPriority(ProcessL);
        // 如果獲取的新的最大優先度進程與當前進程不一樣
        if (pSwap && pSwap->index != p->index)
        {
            // 掛起
            if (p->status != FINISHED)
            {
                p->status == HANG;
            }
            p = pSwap;
            p->status = EXECUTE;
            q = p->pagesLinkList->next;
            // 將內存所有的 pages 退出
            ClearPageList(PhysicalM);
            ClearPageList(VirtualM);
            // record 初始化
            for (int i = 0; i < PageLength; i++)
            {
                record[i] = 0;
            }

            recordIndex = 0;
        }
        // 獲得的優先度進程與當前的一樣
        else
        {
            // next
            q = q->next;
        }
    }

    // 調整 優先度
}

int main()
{
    // 作為物理記憶體
    PagesLinkList PhysicalMemory;
    // 物理記憶體初始化
    InitPageList(&PhysicalMemory);
    // 作為虛擬記憶體
    PagesLinkList VirtualMemory;
    InitPageList(&VirtualMemory);
    // 作為所有需要執行進程
    ProcessesLinkList Process;
    InitProcessList(&Process);

    printf("This is a program to simulate process scheduling and virtual storage management.\n");

    // enterEnd 判斷是否重複進行
    char enterEnd;

    // 如果 enterEnd = "Y" 就無限循環
    do
    {
        printf("First:\n");

        printf("Please define the processeses! If you want to end any action, just enter -1.\n");

        int processIndex = 1;
        // 創建進程的循環
        while (TRUE)
        {
            int priority = 0;
            int status = 0;
            printf("Please enter the process's priority(int).\nInput -1 to stop create a new process.\n");

            printf("Index : %d\npriority(int)-> ", processIndex);

            scanf("%d", &priority);
            // 如果輸入 -1 終止 while
            if (priority == -1)
            {
                break;
            }
            // 進程的 頁面鏈表
            PagesLinkList processPage;
            InitPageList(&processPage);

            // 創建 Page list 的循環
            int pageIndex = 1;
            while (TRUE)
            {
                // 頁框號 - 作為 偏移量
                int frameNumber;
                // 外存地址
                int externalStorageAddress;
                // 有效位
                int valid;
                printf("\nPlease enter a page content in the current process.\n");
                printf("Please enter the frameNumer(int). Input -1 to stop create new page.\nframeNumber(int)-> ");

                scanf("%d", &frameNumber);
                if (frameNumber == -1)
                {
                    break;
                }
                printf("externalStorageAddress(int)-> ");
                scanf("%d", &externalStorageAddress);
                printf("valid(0 or 1)(decide whether to Physical Memory)-> ");
                scanf("%d", &valid);
                // 插入鏈表
                PageListInsert(processPage, pageIndex, 0, frameNumber, externalStorageAddress, valid, FALSE);
                pageIndex += 1;
            }
            ProcessListInsert(Process, processIndex, priority, HANG, processPage);
            processIndex += 1;
        }

        // 進行進程執行 時間片輪詢法調度
        timing_task_scheduler(Process, PhysicalMemory, VirtualMemory);

        // clear 所有
        ClearPageList(PhysicalMemory);
        ClearPageList(VirtualMemory);

        // 結尾
        printf("Do you want to try again? (y/n) -> ");
        // 清空緩衝區
        fflush(stdin);
        scanf("%c", &enterEnd);
        // 一律轉大寫
        enterEnd = (char)toupper(enterEnd);
        printf("enterEnd : %c\n", enterEnd);
        while (!((int)enterEnd == 'Y' || (int)enterEnd == 'N'))
        {
            printf("invalid commend. Please Enter again.\n");
            printf("Do you want to try again? (y/n) -> ");
            // 清空緩衝區
            fflush(stdin);
            scanf("%c", &enterEnd);
            // 一律轉大寫
            enterEnd = (char)toupper(enterEnd);
            printf("enterEnd : %c\n", enterEnd);
        }
    } while (enterEnd == 'Y');
}