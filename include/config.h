/*
 * @Author: Enos Ji
 * @Date: 2021-08-10 18:40:24
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-10 19:34:53
 * @FilePath: \image\include\config.h
 * @Description: 公用的头文件
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

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


#endif