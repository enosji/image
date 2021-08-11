/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 15:32:46
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 19:02:57
 * @FilePath: \image\main.c
 * @Description: image_player 主函数
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <config.h>


unsigned char rgb_buf[BMP_BUF_SIZE];



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
/*
    //bmp 测试程序
    ret = display_bmp("./bmp/1.bmp");
    if(ret < 0)
    {
        fprintf(stderr, "show bmp error\n");
        return -1;
    }
    sleep(3);
    ret = display_jpeg("./jpeg/1.jpg");
    if(ret < 0)
    {
        fprintf(stderr, "show jpeg error\n");
        return -1;
    }
*/
    ret = display_png("./png/1.png");
    if(ret < 0)
    {
        fprintf(stderr, "show png error\n");
        return -1;
    }



    fb_close();


    return 0;
}
