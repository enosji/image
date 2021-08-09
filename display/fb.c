/*
 * @Author: Enos Ji
 * @Date: 2021-08-09 15:50:51
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-09 21:32:01
 * @FilePath: \test_project\display\fb.c
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

#include <fb.h>


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
	
	struct fb_fix_screeninfo finfo = {0};
	struct fb_var_screeninfo vinfo = {0};
	
	// 第1步：打开设备
	fd = open(FBDEVICE, O_RDWR);
	if (fd < 0)
	{
		perror("open ");
		return -1;
	}
	printf("open %s success.\n", FBDEVICE);
	
	// 第2步：获取设备的硬件信息
	ret = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
	if (ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	printf("smem_start = 0x%lx, smem_len = %u.\n", finfo.smem_start, finfo.smem_len);
	
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	if (ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	printf("xres = %u, yres = %u.\n", vinfo.xres, vinfo.yres);
	printf("xres_virtual = %u, yres_virtual = %u.\n", vinfo.xres_virtual, vinfo.yres_virtual);
	printf("bpp = %u.\n", vinfo.bits_per_pixel);
	
	// 第3步：进行mmap
	unsigned long len = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
	printf("len = %ld\n", len);
	pfb = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (NULL == pfb)
	{
		perror("mmap");
		return -1;
	}
	printf("pfb = %p.\n", pfb);

	
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
	
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
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