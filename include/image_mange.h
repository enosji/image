/*
 * @Author: Enos Ji
 * @Date: 2021-08-11 19:48:36
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 21:52:37
 * @FilePath: \image\include\image_mange.h
 * @Description: 图片关联相关头文件
 */
#ifndef __IMAGE_MANGE_H__
#define __IMAGE_MANGE_H__

#define PATHNAME_LEN        256

//对于4种图片格式
typedef enum image_type
{
    IMAGE_TYPE_BMP,
    IMAGE_TYPE_JPEG,
    IMAGE_TYPE_PNG,
    IMAGE_TYPE_UNKNOW,

}image_type_e;


//用来封装一个图片的信息
typedef struct image_info
{
    char pathname[PATHNAME_LEN];        //图片文件的路径
    image_type_e type;                  //图片文件的格式

    
}image_info_t;

//images数组本来是空的
extern image_info_t images[MAX_IMAGE_CNT];
extern unsigned int image_index;




#endif