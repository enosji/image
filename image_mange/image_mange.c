/*
 * @Author: Enos Ji
 * @Date: 2021-08-11 19:56:31
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-11 21:57:46
 * @FilePath: \image\image_mange\image_mange.c
 * @Description: 图片文件管理
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <config.h>

#include <image_mange.h>


image_info_t images[MAX_IMAGE_CNT];
unsigned int image_index = 0;

/**
 * @description: 程序初始化时将会去指点目录下递归检索所有文件和文件夹，并将所有的图片格式收集并且填充的images数组中
 * @param {char} *pathname：指定的图片存储文件夹
 * @return {*}
 * @author: Enos Ji
 */
int scan_image(char *path)
{
    DIR *dir;
    struct  dirent *ptr;
    struct stat sta;
    char base[1000];

    if((dir = opendir(path)) == NULL)
    {
        perror("open dir error");
        exit(1);
    }

    //readdir每调用一次就会返回opendir打开的path目录下的一个文件，知道目录下所有文件被读完之后返回NULL
    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }

        memset(base, '\0', sizeof(base));
        strcpy(base, path);
        strcat(base, "/");
        strcat(base, ptr->d_name);
        lstat(base, &sta);

        if(S_ISREG(sta.st_mode))
        {
            //debug("d_name : %s : %s\n", path, ptr->d_name);
            //如果时普通文件，就在这里进行处理
            //先判断是否属于我们已知的图片格式如果是就放入数组，不是就跳过
            if(!is_bmp(base))
            {
                strcpy(images[image_index].pathname, base);
                images[image_index].type = IMAGE_TYPE_BMP;
            }
            if(!is_jpeg(base))
            {
                strcpy(images[image_index].pathname, base);
                images[image_index].type = IMAGE_TYPE_JPEG;
            }
            if(!is_png(base))
            {
                strcpy(images[image_index].pathname, base);
                images[image_index].type = IMAGE_TYPE_PNG;
            }
            image_index++;
        }

        if(S_ISDIR(sta.st_mode))
        {
            //debug("directory\n");
            //debug("d_name : %s : %s\n", path, ptr->d_name);
            scan_image(base);
        }
        /*
        直接使用readdir返回的结构体来读取文件信息时不行的，不支持nfs文件系统
        switch (ptr->d_type)
        {
            case DT_REG:                              //普通文件
                debug("d_name : %s : %s\n", path, ptr->d_name);
                break;
            case DT_DIR:                                 //文件夹
                memset(base, '\0', sizeof(base));
                strcpy(base, path);
                strcat(base, "/");
                strcat(base, ptr->d_name);
                scan_image(base);
                break;
            case DT_UNKNOWN:             //未知格式文件
                debug("unknow file type\n");
                break;
            default:
                debug("aaa\n");
                break;
        }
        */
    }

    return 0;
    
}

/**
 * @description: 将系统添加到图片管理数组内的图片显示出来
 * @param {*}
 * @return {*}
 * @author: Enos Ji
 */
void show_images(void)
{
    int i;

    for(i = 0; i < image_index; i++)
    {
        switch (images[i].type)
        {
        case IMAGE_TYPE_BMP:
            display_bmp(images[i].pathname);
            break;
        case IMAGE_TYPE_JPEG:
            display_jpeg(images[i].pathname);
            break;
        case IMAGE_TYPE_PNG:
            display_png(images[i].pathname);
            break;
        default:
            break;
        }
        sleep(3);
    }
}