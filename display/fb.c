/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 15:50:51
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 13:19:07
 * @FilePath: \image\display\fb.c
 * @Description: framebuffer的基本实现，包括fb的打开、ioctl的获取信息，测试
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <config.h>


//#include "1024600.h"
//#include "3232.h"


// 全局变量
unsigned int *pfb = NULL;
int fd = -1;

/**
 * @description: 打开fb
 * @param {*}
 * @return {*}
 * @author: Enos Ji
 */
int fb_open(void)
{
	int  ret = -1;
	
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	
	// 第1步：打开设备
	fd = open(FBDEVICE, O_RDWR);
	if (fd < 0)
	{
		perror("open ");
		return -1;
	}
	debug("open %s success.\n", FBDEVICE);
	
	// 第2步：获取设备的硬件信息
	ret = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
	if (ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	debug("smem_start = 0x%lx, smem_len = %u.\n", finfo.smem_start, finfo.smem_len);
	
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	if (ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	debug("xres = %u, yres = %u.\n", vinfo.xres, vinfo.yres);
	debug("xres_virtual = %u, yres_virtual = %u.\n", vinfo.xres_virtual, vinfo.yres_virtual);
	debug("bpp = %u.\n", vinfo.bits_per_pixel);
	
	// 第3步：进行mmap
	unsigned long len = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
	debug("len = %ld\n", len);
	pfb = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (NULL == pfb)
	{
		perror("mmap");
		return -1;
	}
	debug("pfb = %p.\n", pfb);

	
	return 0;
}

/**
 * @description: 关闭fb
 * @param {int} fd
 * @return {*}
 * @author: Enos Ji
 */
void fb_close(void)
{
	close(fd);
}

#if 0
/**
 * @description: 绘制背景的测试函数
 * @param {unsigned int} width：显示屏的长度
 * @param {unsigned int} height：显示屏的宽度
 * @param {unsigned int} color：颜色
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color)
{
	unsigned int x, y;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(pfb + y * WIDTH + x) = color;
		}
	}
}

void fb_draw_line(unsigned int color)
{
	unsigned int x;
	
	for (x=50; x<600; x++)
	{
		*(pfb + 200 * WIDTH + x) = color;
	}
}

/**
 * @description: 测试显示器显示图片
 * @param {*}
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw_pic(void)
{
	const unsigned char *pdata = gImage_1024600;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = 0;
	unsigned int cnt;
	int i, j;

	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			cnt = WIDTH * i + j;		//当前像素的编号
			
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			*(p + cnt) = ((pdata[a] << 16) | (pdata[a + 1] << 8) | (pdata[a + 2] << 0));
			
			a += 3;

		}
	}
}

/**
 * @description: 测试比显示屏小的图片
 * @param {*}
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw_pic1(void)
{
	const unsigned char *pdata = gImage_3232;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = 0;
	unsigned int cnt;
	int i, j;

	for(i = 0; i < 32; i++)
	{
		for(j = 0; j < 32; j++)
		{
			cnt = WIDTH * i + j;		//当前像素的编号
			
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			*(p + cnt) = ((pdata[a] << 16) | (pdata[a + 1] << 8) | (pdata[a + 2] << 0));
			
			a += 3;

		}
	}
}

/**
 * @description: 将图片显示到屏幕的任意位置
 * @param {unsigned int} x：显示位置的起始行坐标
 * @param {unsigned int} y：显示位置的起始列坐标
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw_pic2(unsigned int x, unsigned int y)
{
	const unsigned char *pdata = gImage_3232;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = 0;
	unsigned int cnt1;
	int i, j;

	for(i = y; i < y + 32; i++)
	{
		for(j = x; j < x + 32; j++)
		{
			cnt1 = WIDTH * i + j;		//当前像素的编号
	
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			//左值考虑的是当前像素点在fb内存中的偏移量
			//右值考虑的是当前像素点在图像数据中的数组下标
			*(p + cnt1) = ((pdata[a] << 16) | (pdata[a + 1] << 8) | (pdata[a + 2] << 0));
			
			a += 3;

		}
	}
}

/**
 * @description: 显示任意坐标的图片，超出显示屏部分去掉
 * @param {unsigned int} x
 * @param {unsigned int} y
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw_pic3(unsigned int x, unsigned int y)
{
	const unsigned char *pdata = gImage_1024600;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = 0;
	unsigned int cnt;
	int i, j;

	for(i = y; i < y + HEIGHT; i++)
	{
		//当y方向超出之后剩下的部分就不用显示了
		if(i > HEIGHT)
		{
			break;
		}
		
		for(j = x; j < x + WIDTH; j++)
		{
			cnt = WIDTH * i + j;		//当前像素的编号
			if(j > WIDTH)
			{
				a += 3;
				continue;
			}
			
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			*(p + cnt) = ((pdata[a] << 16) | (pdata[a + 1] << 8) | (pdata[a + 2] << 0));
			
			a += 3;

		}
	}
}

#endif


/**
 * @description: bmp图片解析后显示调用的函数
 * @param {unsigned int} x：行偏移
 * @param {unsigned int} y：列偏移
 * @param {unsigned char *} pPic：像素数据
 * @param {t_bmp_info_hearder} info：图片信息结构体
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw(unsigned int x, unsigned int y,  pic_info *info)
{
	unsigned char *pdata = info->pdata;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = info->width * info->height * 3;
	unsigned int cnt;
	int i, j;
	a -= 3;

	if((info->bpp != 32) && (info->bpp != 24))
	{
		fprintf(stderr, "bpp %d is not support\n", info->bpp);
		return;
	}

	for(i = y; i < y + info->height; i++)
	{
		//当y方向超出之后剩下的部分就不用显示了
		if(i > HEIGHT)
		{
			break;
		}
		
		for(j = x; j < x + info->width; j++)
		{
			cnt = WIDTH * i + j;		//当前像素的编号
			if(j > WIDTH)
			{
				a -= 3;
				continue;
			}
			
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			*(p + cnt) = ((pdata[a] << 0) | (pdata[a + 1] << 8) | (pdata[a + 2] << 16));
			
			a -= 3;

		}
	}
}

/**
 * @description: jpeg图片解析后显示调用的函数
 * @param {unsigned int} x：行偏移
 * @param {unsigned int} y：列偏移
 * @param {unsigned char *} pPic：像素数据
 * @param {t_bmp_info_hearder} info：图片信息结构体
 * @return {*}
 * @author: Enos Ji
 */
void fb_draw2(unsigned int x, unsigned int y,  pic_info *info)
{
	unsigned char *pdata = info->pdata;	//定义一个指向这个数组的制作
	unsigned int *p = pfb;
	unsigned int a = 0;
	unsigned int cnt;
	int i, j;


	if((info->bpp != 32) && (info->bpp != 24))
	{
		fprintf(stderr, "bpp %d is not support\n", info->bpp);
		return;
	}

	for(i = y; i < y + info->height; i++)
	{
		//当y方向超出之后剩下的部分就不用显示了
		if(i > HEIGHT)
		{
			break;
		}
		
		for(j = x; j < x + info->width; j++)
		{
			cnt = WIDTH * i + j;		//当前像素的编号
			if(j > WIDTH)
			{
				a += 3;
				continue;
			}
			
			//一个像素是包含三个颜色将三个颜色移位或上之后复制给显存
			*(p + cnt) = ((pdata[a] << 16) | (pdata[a + 1] << 8) | (pdata[a + 2] << 0));
			
			a += 3;

		}
	}
}