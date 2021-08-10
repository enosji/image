/*
 * @Author: Enos Ji
 * @Date: 2021-08-10 15:31:45
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-10 19:32:08
 * @FilePath: \image\include\bmp.h
 * @Description: bmp解析的头文件
 */
#ifndef __BMP_H__
#define __BMP_H__
/* 
 * 这句话的意思是地址采用1字节对齐 
 * 由于gcc默认是4字节对齐，而我们这里需要和bmp的头部信息一一对应，所以应该采用1字节对齐方式
 */
#pragma pack (1) 

typedef unsigned long uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

//bmp文件头
typedef struct 
{
    unsigned short bf_byte;
    unsigned long bf_size;
    unsigned short bf_Reserved1;
    unsigned short bf_Reserved2;
    unsigned long bf_off_Bits;
    
}t_bmp_file_header;

//图像信息头
typedef struct {

    uint32_t bi_size; // 4Bytes，INFOHEADER结构体大小，存在其他版本I NFOHEADER，用作区分
    uint32_t bi_width; // 4Bytes，图像宽度（以像素为单位）
    long bi_height; // 4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
    uint16_t bi_planes; // 2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
    uint16_t bi_bit_count; // 2Bytes，图像像素位数
    uint32_t bi_compression; // 4Bytes，0：不压缩，1：RLE8，2：RLE4
    uint32_t bi_image_size; // 4Bytes，4字节对齐的图像数据大小
    uint32_t bi_x_pels_per_meter; // 4Bytes，用象素/米表示的水平分辨率
    uint32_t bi_y_pels_per_meter; // 4Bytes，用象素/米表示的垂直分辨率
    uint32_t bi_clr_used; // 4Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
    uint32_t bi_clr_important; // 4Bytes，重要的调色板索引数，0：所有的调色板索引都重要
    
} t_bmp_info_hearder;


typedef struct {
    uint8_t bp_blue; // 指定蓝色强度
    uint8_t bp_green; // 指定绿色强度
    uint8_t bp_red; // 指定红色强度
    uint8_t bp_reserved; //保留，设置为0

} t_bmp_pallet;
 

typedef struct {
    t_bmp_file_header bmp_file_header;
    t_bmp_info_hearder bmp_info_hearder;
    t_bmp_pallet bmp_pallet;
    
} t_bmp_header;

/* 取消地址对齐 */
#pragma pack ()

int bmp_analyze(pic_info *pPic);

#endif