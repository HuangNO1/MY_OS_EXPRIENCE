#include <stdio.h>
#include "LinkList.h"
#include <ctype.h>

int main()
{
    // 作為物理記憶體
    Page *PhysicalMemory;
    // 物理記憶體初始化
    InitPageList(&PhysicalMemory);
    // 作為所有需要執行進程
    Process *process;
    InitProcessList(&process);

    printf("This is a program to simulate process scheduling and virtual storage management.\n");

    // enterEnd 判斷是否重複進行
    char enterEnd;

    // 如果 enterEnd = "Y" 就無限循環
    do
    {
        printf("First:\n");

        printf("Please define the processes! If you want to end any action, just enter -1.\n");

        while(TRUE) {
            
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