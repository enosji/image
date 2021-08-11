/*
 * @Author: Enos Ji
 * @Date: 2021-08-11 10:57:55
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 19:01:54
 * @FilePath: \image\display\fb_jpeg.c
 * @Description: 解码jpeg格式图片，调用fb.c函数显示图片
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jerror.h>
#include <jpeglib.h>


#include <config.h>

/**
 * @description: 判断是否是jpeg格式
 * @param {char} *path：图片路径
 * @return {*}：判断是返回0，不是返回1，错误返回-1
 * @author: Enos Ji
 */
static int is_jpeg(char *path)
{
    FILE *ffd = NULL;
    char buf[2] = {0};
    
    ffd = fopen(path, "r");
    if(NULL == ffd)
    {
        fprintf(stderr, "open %s error\n", path);
        fclose(ffd);
        return -1;
    }

    //读出图片的前两个字节判断是否是jpeg格式
    fread(buf, sizeof(char), sizeof(buf), ffd);
    debug("read : %x : %x\n", buf[0], buf[1]);
    if(!((buf[0] == 0xff) && (buf[1] == 0xd8)))
    {
        fclose(ffd);
        return 1;
    }
    //文件指针移到最后两个位置判断
    fseek(ffd, -2, SEEK_END);
    fread(buf, sizeof(char), sizeof(buf), ffd);
    debug("read : %x : %x\n", buf[0], buf[1]);
    if(!((buf[0] == 0xff) && (buf[1] == 0xd9)))
    {
        fclose(ffd);
        return 1;
    }
    
    fclose(ffd);

    return 0;

}

struct my_error_mgr {
    struct jpeg_error_mgr pub;	
 
    jmp_buf setjmp_buffer;	
};

typedef struct my_error_mgr * my_error_ptr;

//自己定义的错误处理函数
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
 
    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);
 
    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @description: jpeg格式图片的解码
 * @param {pic_info} *pPic：图片给各类参数的结构体
 * @return {*}：成功解码返回0，错误返回-1
 * @author: Enos Ji
 */
static int jpeg_analyze(pic_info *pPic)
{

    struct jpeg_decompress_struct cinfo;        
    struct my_error_mgr jerr;               //用来做错误处理的
    FILE * infile;                           //文件句柄 

	
    //从源码中可以知道这是一个指向指针的指针 
    JSAMPARRAY buffer = NULL;           //一行图形数据存储的地方
    int row_stride;                     //一行数据的字节数 

 
    //以二进制方式打开图像文件
    if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
    {
        fprintf(stderr, "can't open %s\n", pPic->pathname);
        return -1;
    }
 
    //1.分配和初始化cinfo
 
    //绑定错误处理函数
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
 
    //错误处理
    if (setjmp(jerr.setjmp_buffer)) 
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return -1;
    }
    
    //初始化cinfo，分配内存和数据结构的初始化
    jpeg_create_decompress(&cinfo);
 
    //2.初始化cinfo，将我们打开的二进制图片和解码结构体相关联
    jpeg_stdio_src(&cinfo, infile);
 
    //3.读取文件头
    jpeg_read_header(&cinfo, TRUE);
   

    //4.开始解压
    jpeg_start_decompress(&cinfo);

    debug("image reslution : %d : %d : bpp/8 = %d\n", 
                cinfo.output_width, cinfo.output_height, cinfo.output_components);
 
    //记录下图像的宽度、高度和bpp
    pPic->width = cinfo.output_width;
    pPic->height = cinfo.output_height;
    pPic->bpp = cinfo.output_components * 8;
	/*
	//自己定义的图像存储区
	char *p_image_buf;
    p_image_buf = (unsigned char *)malloc(abs(pPic->width * pPic->height * 3));
    */
    //一行字节数 = 行宽 * 每个像素点字节数
    row_stride = cinfo.output_width * cinfo.output_components; 
 
    //给libjpeg所需要的行缓存分配内存空间
    buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1); 
    if(NULL == buffer)
    {
        fprintf(stderr, "cinfo.mem->alloc_sarray malloc error\n");
    }
 
    //5.一行一行地扫描图像
    while (cinfo.output_scanline < cinfo.output_height) 
    {
        //读取一行图像
        jpeg_read_scanlines(&cinfo, buffer, 1);
 
        //将一行图像数据保存起来
        memcpy(pPic->pdata + ((cinfo.output_scanline - 1) * row_stride), buffer[0], row_stride);    //特别注意buffer[0]
    }

  
    //6.结束解压 
    jpeg_finish_decompress(&cinfo);
 
    //7.释放内存
    jpeg_destroy_decompress(&cinfo);

    

    fclose(infile);

    

    return 0;


}


/**
 * @description: 封装一个对外使用的函数接口
 * @param {char *} pathname：对外的传参只有一个路径
 * @return {*}
 * @author: Enos Ji
 */
int display_jpeg(char * pathname)
{
    int ret = 0;
    pic_info pPic;
    pPic.pathname = pathname;       //指针变量可以直接赋值
    pPic.pdata = rgb_buf;
    
    //检查图片格式
    ret = is_jpeg(pathname);
    if(ret == 1)
    {
        fprintf(stderr, "this picture is not a jpeg\n");
        return -1;
    }
    else if(ret == -1)
    {
        return -1;
    }

    //图片的解码
    ret = jpeg_analyze(&pPic);
    if(ret < 0)
    {
        return -1;
    }

    //图片的显示
    fb_draw2(0, 0, &pPic);


    return 0;

}