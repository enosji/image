/*
 * @Author: Enos Ji
 * @Date: 2021-08-11 15:26:08
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 21:21:10
 * @FilePath: \image\display\fb_png.c
 * @Description: 解析png格式的图片
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>

#include <config.h>

#define PNG_BYTES_TO_CHECK 4

/**
 * @description: 判断图片类型
 * @param {char} *path ： 图片路径
 * @return {*} ：判断是返回0，不是返回1，错误返回-1
 * @author: Enos Ji
 */
int is_png(char *path)
{
    FILE *fp = NULL;
    char buf[PNG_BYTES_TO_CHECK];

    if((fp = fopen(path, "rb")) == NULL)
    {
        fprintf(stderr, "open %s error\n", path);
        return -1;
    }

    if(fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
    {
        fprintf(stderr, "read %s error\n", path);
        return -1;
    }

    return (png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

/**
 * @description: png格式图片的解码
 * @param {pic_info} *pPic ： 定义的图像相关数据的结构体
 * @return {*}
 * @author: Enos Ji
 */
static int png_analyze(pic_info *pPic)
{
    int i, j;
    int pos = 0;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;        /* 指针数组，数组中的每一个指针都指向一行图像数据 */
    int color_type = 0; 
    int image_size = 0;
	
    FILE *p_fp;
 
    if ((p_fp = fopen(pPic->pathname, "rb")) == NULL)
    {
        fprintf(stderr, "%s open fail!!!\n", pPic->pathname);
        return -1;
    }
 
    /* 创建一个png_structp结构体，如果不想自定义错误处理，后3个参数可以传NULL */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);	
    if (png_ptr == NULL)
    {
        fclose(p_fp);
        return -1;
    }
 
	/* 创建一个info_ptr，必须的 */	
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(p_fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return -1;
    }
 
    /* 如果上面png_create_read_struct没有自定义错误处理，这里是必须的 ,设置错误处理*/
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(p_fp);
 
        return -1;
    }
 
    /* 初始化文件IO ，将要解码的图片的文件指针与png解码器绑定起来*/
	png_init_io(png_ptr, p_fp);
 
    /* 
     * 函数功能：读取png图片信息 
     * 说明：可以参数3来改变不同的读取方式，这里只读取RGB，抛弃了ALPHA
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);
	if ((png_ptr != NULL) && (info_ptr != NULL))
	{
		/* 获取图像的宽高 */
        #if 1
        pPic->width = info_ptr->width;       
		pPic->height = info_ptr->height; 
        pPic->bpp = info_ptr->pixel_depth;
        #endif
        #if 0       //两种方法都是可以的
		pPic->width = png_get_image_width(png_ptr, info_ptr);       
		pPic->height = png_get_image_height(png_ptr, info_ptr); 
        #endif
		debug("width:%d, height:%d, bpp:%d\n", pPic->width, pPic->height, pPic->bpp);

        /* 获取图像颜色类型 */
	    color_type = info_ptr->color_type; 
        debug("color type:%d\n", color_type);

    }

    /* 获取所有的图像数据 */
    row_pointers = png_get_rows(png_ptr, info_ptr);
 
	/* 只处理RGB24位真彩色格式的图片 */
    if(PNG_COLOR_TYPE_RGB == color_type)
    {
        /* 图像数据的大小 */
        image_size = pPic->width * pPic->height * pPic->bpp / 8;  
        
        for(i = 0; i < pPic->height; i++)
        {
            for(j = 0; j < (pPic->width * 3); j += 3)
            {
                pPic->pdata[pos++] = row_pointers[i][j + 0];
                pPic->pdata[pos++] = row_pointers[i][j + 1];
                pPic->pdata[pos++] = row_pointers[i][j + 2];        
            }
        }
        
    }   

    /* 清理图像，释放内存 */
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    
    fclose(p_fp);    
 
    return 0;
}

/**
 * @description: 封装一个对外使用的函数接口
 * @param {char *} pathname：对外的传参只有一个路径
 * @return {*}
 * @author: Enos Ji
 */
int display_png(char * pathname)
{
    int ret = 0;
    pic_info pPic;
    pPic.pathname = pathname;       //指针变量可以直接赋值
    pPic.pdata = rgb_buf;
    
    //检查图片格式
    ret = is_png(pathname);
    if(ret != 0)
    {
        fprintf(stderr, "this picture is not a png\n");
        return -1;
    }

    ret = png_analyze(&pPic);
    if(ret < 0)
    {
        return -1;
    }

    fb_draw2(0, 0, &pPic);

    return 0;

}
