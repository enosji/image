/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 20:51:10
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 12:48:11
 * @FilePath: \image\include\fb.h
 * @Description: fb 头文件
 */
#ifndef __FB_H__
#define __FB_H__

// 宏定义
#define FBDEVICE	"/dev/fb0"


// 新开发板
#define WIDTH		1024	
#define HEIGHT		600


#define WHITE		0xffffffff			// test ok
#define BLACK		0x00000000
#define RED			0xffff0000
#define GREEN		0xff00ff00			// test ok
#define BLUE		0xff0000ff			


// 函数声明
int fb_open(void);
void fb_close(void);
#if 0
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color);
void fb_draw_line(unsigned int color);
void fb_draw_pic(void);
void fb_draw_pic1(void);
void fb_draw_pic2(unsigned int x, unsigned int y);
void fb_draw_pic3(unsigned int x, unsigned int y);
#endif
//bmp格式显示
void fb_draw(unsigned int x, unsigned int y,  pic_info *info);
//jpeg格式显示
void fb_draw2(unsigned int x, unsigned int y,  pic_info *info);


#endif
