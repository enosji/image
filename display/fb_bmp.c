/*
 * @Author: Enos Ji
 * @Date: 2021-08-10 14:35:33
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-10 19:55:13
 * @FilePath: \image\display\fb_bmp.c
 * @Description: 解析BMP格式图片
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
#include <bmp.h>
#include <fb.h>


//规定最大解析图片的大小为1920*1080, BPP最多24
#define BMP_MAX_RESOLUTON       (1920 * 1080)
#define BMP_BUF_SIZE            (BMP_MAX_RESOLUTON * 3)


unsigned char bmp_buf[BMP_BUF_SIZE];

/**
 * @description: 判断这张图片是不是bmp图片
 * @param {const unsigned char} *path ： 图片路径
 * @return {*}：是返回0，不是返回-1
 * @author: Enos Ji
 */
static int is_bmp(char *path)
{
    unsigned char buf[2] = {0};
    int fd = -1;
    int ret;

    //打开图片
    fd = open(path, O_RDONLY);
    if(fd < 0)
    {   
        fprintf(stderr, "open %s error\n", path);
        goto close;
    }

    //读取文件头信息
    ret = read(fd, buf, sizeof(buf));
    if(ret != 2)
    {
        fprintf(stderr, "read file header error\n");
        goto close;
    }

    //判断是否是BMP格式
    if(buf[0] != 'B' || buf[1] != 'M')
    {
        fprintf(stderr, "the image is not a bmp\n");
        goto close;
    }

    debug("check bmp success\n");

    close(fd);
    return 0;

close:
    close(fd);
    return -1;
}


/**
 * @description: 解析bmp图片函数
 * @param {unsigned char} *path:要解析的图片的文件名
 * @return {int}: 错误返回-1，解析正确返回0
 * @author: Enos Ji
 */
int bmp_analyze(pic_info *pPic)
{
    t_bmp_file_header fHeader;
    t_bmp_info_hearder info;
    long len;
    int fd = -1, ret = -1;

    ret = is_bmp(pPic->pathname);
    if(ret < 0)
    {
        fprintf(stderr, "the picture is not a bmp\n");
        return -1;
    }

    //打开图片
    fd = open(pPic->pathname, O_RDONLY);
    if(fd < 0)
    {   
        fprintf(stderr, "open %s error\n", pPic->pathname);
        return -1;
    }

    //读取图片文件头
    read(fd, &fHeader, sizeof(fHeader));
    debug("bfsize = %ld\n", fHeader.bf_size);
    debug("bfoffBits = %ld\n", fHeader.bf_off_Bits);

    //读取图片信息头
    read(fd, &info, sizeof(info));
    debug("bi_width = %ld\n", info.bi_width);
    debug("bi_height = %ld\n", info.bi_height);
    debug("bpp = %d\n", info.bi_bit_count);

    //将读到的信息放到我们自己定义的图片结构体中
    pPic->width = info.bi_width;
    pPic->height = info.bi_height;
    pPic->bpp = info.bi_bit_count;              

    //读取图片有效信息，从第fHeader.bf_off_Bits字节处开始读取,读取info.bi_width * info.bi_height * info.bi_bit_count/3个字节
    lseek(fd, fHeader.bf_off_Bits, SEEK_SET);
    len = info.bi_width * info.bi_height * info.bi_bit_count / 3;
    read(fd, bmp_buf, len);

    pPic->pdata = bmp_buf;

    //把内容放到fb中显示
    fb_draw(0, 0, pPic);
    
    close(fd);
    return 0;
}
