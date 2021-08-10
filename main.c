/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 15:32:46
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-10 20:00:44
 * @FilePath: \image\main.c
 * @Description: image_player 主函数
 */
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <bmp.h>
#include <fb.h>



int main(int argc, char const *argv[])
{
    int ret;
    pic_info pPic;

    printf("image decode player......\n");
    
    ret = fb_open();
    if(ret < 0)
    {
        printf("fb_open error\n");
        return -1;
    }

    //fb_draw_back(WIDTH, HEIGHT, RED);
    //fb_draw_pic();
    //fb_draw_pic3(400, 200);

    pPic.pathname = "./bmp/800480.bmp";       //指针变量可以直接赋值
    ret = bmp_analyze(&pPic);
    if(ret < 0)
    {
        fprintf(stderr, "show bmp error\n");
        return -1;
    }


    fb_close();


    return 0;
}
