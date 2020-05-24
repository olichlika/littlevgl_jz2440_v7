#ifndef MYFB_H
#define MYFB_H

#include "lv_drv_conf.h"

#include "lvgl/lvgl.h"

int myfb_init(void);
void my_disp_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

#endif /*FBDEV_H*/
