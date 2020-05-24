#include "lvgl/lvgl.h"
#include "lv_drivers/display/myfb.h"
#include "lv_drivers/indev/myts.h"
#include "lv_drivers/indev/evdev.h"

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define DISP_BUF_SIZE (272 * LV_HOR_RES_MAX)

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    myfb_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, buf2, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer   = &disp_buf;
    disp_drv.flush_cb = my_disp_flush;
    lv_disp_drv_register(&disp_drv);


    /*init touch screen*/
    myts_init();
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = myts_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/
	lv_indev_drv_register(&indev_drv);

    /*Create a Demo*/
    //label_test();
    lv_demo_widgets();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_tick_inc(1);
        lv_task_handler();
        usleep(100);     
    }

    return 0;
}