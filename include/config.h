/*
 * @Author: Enos Ji
 * @Date: 2021-08-10 18:40:24
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-12 10:19:39
 * @FilePath: \image\include\config.h
 * @Description: 公用的头文件
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

//规定最大解析图片的大小为1920*1080, BPP最多24
#define BMP_MAX_RESOLUTON       (1920 * 1080)
#define BMP_BUF_SIZE            (BMP_MAX_RESOLUTON * 3)

//规定最多支持的图片数量
#define MAX_IMAGE_CNT           100
#define DEVICE_TOUCH			"/dev/input/event2"
#define TOUCH_WIDTH             200         //定义触摩翻页宽度


#define DEBUG           //打开调试开关
//#undef DEBUG
//定义debug宏
#ifdef DEBUG
#define debug(...)  do{                                                        \
                        fprintf(stderr, "[debug] [%s : %s : %d] ",         \
                                __FILE__, __func__, __LINE__);            \
                        fprintf(stderr, __VA_ARGS__);                        \
                    } while(0)
#else
#define debug(...)
#endif

//用来封装我们需要的图片信息
typedef struct pic_info
{
    char *pathname;         //图片的路径
    unsigned int width;     //图片分辨率的宽
    unsigned int height;     //图片宽
    unsigned int bpp;       //图片位宽
    unsigned char *pdata;            //指向我们图片有效数据的指针

}pic_info;


#include <bmp.h>
#include <fb.h>

extern unsigned char rgb_buf[BMP_BUF_SIZE];

int display_jpeg(char * pathname);
int display_png(char * pathname);


int is_jpeg(char *path);
int is_bmp(char *path);
int is_png(char *path);

int scan_image(char *path);
void show_images(void);

int ts_updown(void);

#endif