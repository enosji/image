/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 15:32:46
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-09 21:31:56
 * @FilePath: \test_project\main.c
 * @Description: image_player 主函数
 */
#include <stdio.h>

#include <fb.h>

int main(int argc, char const *argv[])
{
    int ret;

    printf("image decode player......\n");
    
    ret = fb_open();
    if(ret < 0)
    {
        printf("fb_open error\n");
        return -1;
    }

    fb_draw_back(WIDTH, HEIGHT, RED);

    fb_close();


    return 0;
}
