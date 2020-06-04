#include <stdio.h>
#include "LinkList.h"
#include <ctype.h>
// 定義頁表長為 5
#define PageLength 5

// 找到 [最大優先級] 且 [未執行完成] 的進程並返回
ProcessesLinkList findMaxPriority(ProcessesLinkList ProcessL)
{
    ProcessesLinkList p, max;
    // 指向第一個節點
    p = ProcessL->next;

    int maxPriority = 0;
    // 找出優先度最高的進程
    while (p)
    {
        if (p->priority > maxPriority && p->status != FINISHED)
        {
            // 將指針指向最優先級最大的
            max = p;
        }
        p = p->next;
    }
    return max;
}

// 時間片輪詢法 進程調度
void timing_task_scheduler(ProcessesLinkList ProcessL, PagesLinkList PhysicalM, PagesLinkList VirtualM)
{
    ProcessesLinkList p;
    PagesLinkList q;

    // 先得到最大的優先級
    p = findMaxPriority(ProcessL);
    // 利用頁表最大長度將 優先級最大的進程 頁 填入 Memory 執行
    // 限制頁表的大小
    int i = 0;
    q = p->pagesLinkList;

    // 用來記錄頁表
    int record[PageLength] = {};
    // 用來判斷 Clock 演算法調度的檢查位置
    int recordIndex = 0;
    // 開始進行執行的進程
    while (TRUE)
    {
        int sum = PageListLength(PhysicalM) + PageListLength(VirtualM);
        // 頁表是否已被執行，如果沒被執行就填入
        if (q->status == FALSE)
        {
            // 頁表沒裝滿
            if (sum < 5)
            {
                // 填入
                // 放入物理內存
                if (q->valid == TRUE)
                {
                    // flag 輸入 1 表示修改過，status 設成 TRUE 表示執行過
                    PageListInsert(PhysicalM, 0, 1, q->frameNumber, q->externalStorageAddress, q->valid, TRUE);
                    q->status = TRUE;
                }
                // 放入虛擬內存
                else
                {
                    // flag 輸入 1
                    PageListInsert(VirtualM, 0, 1, q->frameNumber, q->externalStorageAddress, q->valid, TRUE);
                    q->status = TRUE;
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
            // 已經裝滿了檢查是否 下一個 頁訪問 已在內存裡面
            else if (sum == 5)
            {
                int isExist = FALSE;

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
                        recordIndex = i;
                        break;
                    }
                }
                // 如果已經在了
                if (flag)
                {
                    // 將頁的 flag 設為 1 表示修改過
                    // 遍歷每個內存
                    while (checkPhysical)
                    {
                        if(checkPhysical->frameNumber == q->frameNumber)
                        {
                            checkPhysical->flag = 1;
                        }
                        checkPhysical = checkPhysical->next;
                    }
                    while (checkVirtual)
                    {
                        if(checkVirtual->frameNumber == q->frameNumber)
                        {
                            checkVirtual->flag = 1;
                        }
                        checkVirtual = checkVirtual->next;
                    }
                    // 表示已經訪問
                    q->status = TRUE;
                }
                // 不存在，產生 [斷頁]
                else
                {
                    // 缺頁次數加一
                    p->res += 1;
                    // 無限直到找到可以移出去的
                    while (TRUE)
                    {
                        // 從 recordIndex 的地方開始找
                        for (int i = recordIndex; i < PageLength; i++)
                        {
                            if (checkPhysical->flag == TRUE)
                            {
                                // 改變狀態
                                checkPhysical->flag == FALSE;
                            }
                        }
                    }
                }
            }
        }

        // next
        q = q->next;
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
            printf("Please enter the process's priority(int).\nInput -1 to stop create a new process.\npriority(int)-> ");

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