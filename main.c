#include <stdio.h>
#include "LinkList.h"
#include <ctype.h>

void init()
{
}

int main()
{
    // 作為物理記憶體
    Page *PhysicalMemory;
    // 物理記憶體初始化
    InitPageList(&PhysicalMemory);
    // 作為所有需要執行進程
    Process *Process;
    InitProcessList(&Process);

    printf("This is a program to simulate process scheduling and virtual storage management.\n");

    // enterEnd 判斷是否重複進行
    char enterEnd;

    // 如果 enterEnd = "Y" 就無限循環
    do
    {
        printf("First:\n");

        printf("Please define the processeses! If you want to end any action, just enter -1.\n");

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
            Page *processPage;
            InitPageList(&processPage);

            // 創建 Page list 的循環
            int pageIndex = 1;
            while (TRUE)
            {
                // 頁框號 - 作為 偏移量
                int frameNumber;
                // 外存地址
                int externalStorageAddress;
                printf("\nPlease enter a page content in the current process.\n");
                printf("Please enter the frameNumer(int). Input -1 to stop create new page.\nframeNumber(int)-> ");

                scanf("%d", &frameNumber);
                if(frameNumber == -1) {
                    break;
                }
                printf("externalStorageAddress(int)-> ");
                scanf("%d", &externalStorageAddress);
                // 插入鏈表
                PageListInsert(&processPage, pageIndex, 0, frameNumber, externalStorageAddress);
                pageIndex += 1;
            }
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