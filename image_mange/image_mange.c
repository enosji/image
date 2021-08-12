/*
 * @Author: Enos Ji
 * @Date: 2021-08-11 19:56:31
 * @LastEditors: Enos Ji
 * @LastEditTime: 2021-08-12 11:11:36
 * @FilePath: \image\image_mange\image_mange.c
 * @Description: 图片文件管理
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>
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

static void show_up_image(int i)
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
}

/**
 * @description: 实现通过触摸屏来对图片进行翻页
 * @param {*}
 * @return {*}
 * @author: Enos Ji
 */
int ts_updown(void)
{
    int i = 0;
	int fd = -1, ret = -1;
	struct input_event ev;
	
	// 第1步：打开设备文件，触摸屏检测
	fd = open(DEVICE_TOUCH, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	debug("image_index = %d\n", image_index);
	while (1)
	{
		// 第2步：读取一个event事件包
		memset(&ev, 0, sizeof(struct input_event));
		ret = read(fd, &ev, sizeof(struct input_event));
		if (ret != sizeof(struct input_event))
		{
			perror("read");
			close(fd);
			return -1;
		}
		
        /*
		// 第3步：解析event包，才知道发生了什么样的输入事件
		debug("type = %hd.\n", ev.type);	
        debug("code = %hd.\n", ev.code);	
        debug("value = %d.\n", ev.value);	
        debug("\n");
        */
        if((ev.type == EV_ABS) && (ev.code == ABS_X))
        {  
            //确定这个是x坐标
            if((ev.value < TOUCH_WIDTH) && (ev.value >= 0))
            {
                //上翻页
                i--;
                if(i < 0)
                {
                    i = image_index - 1;
                }
                debug("i = %d\n", i);
            }
            else if((ev.value > (WIDTH - TOUCH_WIDTH)) && (ev.value <= WIDTH))
            {
                //下翻页
                i++;
                if(i >= image_index)
                {
                    i = 0;
                }
                debug("i = %d\n", i);
            }
            show_up_image(i);
        }
	}
	 
	
	// 第4步：关闭设备
	close(fd);
	
	return 0;
}