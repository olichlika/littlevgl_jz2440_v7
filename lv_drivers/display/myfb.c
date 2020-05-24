#include "fbdev.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>


#define  FILE  "/dev/fb0"      // 设备文件

int fd = 0;
struct fb_fix_screeninfo finfo = {0};       // 不可变信息结构体
struct fb_var_screeninfo vinfo = {0};       // 可变信息结构体
static volatile unsigned int *pMap = NULL;  // 用来指向mmap映射得到的虚拟地址

int myfb_init(void) {
    /* 打开文件得到文件描述符 */
    fd = open(FILE, O_RDWR);
    if (0 > fd) {
        perror("open error");
        return -1;
    }
    printf("%s 打开成功\n", FILE);

    /* 操作文件 */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("ioctl error");
        close(fd);
        return -1;
    }    

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("ioctl error");
        close(fd);
        return -1;
    }

    // 打印信息
    printf("size lv_color_t = %d\n", sizeof(lv_color_t));
    printf("size unsigned int = %d\n", sizeof(unsigned int));  
    printf("不可变信息smem_start = 0x%x\n", finfo.smem_start);    
    printf("不可变信息smem_len = %ld\n", finfo.smem_len);    
    printf("可变信息xres = %d, yres = %d\n", vinfo.xres, vinfo.yres);//480 272
    printf("可变信息xres_virtual = %d, yres_virtual = %d\n", vinfo.xres_virtual, vinfo.yres_virtual);
    printf("可变信息xoffset = %d, yoffset = %d\n", vinfo.xoffset, vinfo.yoffset);

    /* 进行mmap映射 */
    pMap = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (NULL == pMap) {
        perror("mmap error");
        return -1;
    }
    memset(pMap, 0, finfo.smem_len);//清屏

    return 0;
}

/*填充像素点*/
static inline void lcd_draw_pixel(unsigned int x, unsigned int y, unsigned int color) {
    *(unsigned int *)((unsigned int)pMap + (vinfo.xres*y + x)*4) = color;
}

void my_disp_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p) {
    // int32_t x, y;
    // unsigned int c;
    
    // for(y = area->y1; y <= area->y2; y++) {
    //     for(x = area->x1; x <= area->x2; x++) {
    //         c = color_p->full;
    //         lcd_draw_pixel(x, y, c);  /* Put a pixel to the display.*/
    //         color_p = color_p+1;
    //     }
    // }
    // lv_disp_flush_ready(drv);         /* Indicate you are ready with the flushing*/

    if(pMap == NULL ||
            area->x2 < 0 ||
            area->y2 < 0 ||
            area->x1 > (int32_t)vinfo.xres - 1 ||
            area->y1 > (int32_t)vinfo.yres - 1) {
        lv_disp_flush_ready(drv);
        return;
    }

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;


    lv_coord_t w = lv_area_get_width(area);
    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
        uint32_t * fbp32 = (uint32_t *)pMap;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
            memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
            color_p += w;
        }
    }

    lv_disp_flush_ready(drv);
}