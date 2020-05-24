#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>	

#include "lvgl/src/lv_hal/lv_hal_indev.h"

#include "evdev.h"

#include "tslib.h"

int evdev_fd;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

struct tsdev *ts;

static pthread_t tslib_thread_id;

void *tslib_thread(void *pArg) {	
	printf("tslib_thread\r\n");

    while(1) {									
		struct ts_sample samp;
		int ret;			
		
		ret = ts_read(ts, &samp, 1);
												
		if (samp.pressure > 0) {				
			evdev_root_x = samp.x;
			evdev_root_y = samp.y;
			evdev_button = LV_INDEV_STATE_PR;
		} else
			evdev_button = LV_INDEV_STATE_REL;				
				
		printf("x:%d,y:%d,value:%d\n", evdev_root_x, evdev_root_y, evdev_button);		
    }

    return NULL;    
}

static int ts_init(void) {			
    char *tsdevice=NULL;	
			
	if ((tsdevice = getenv("TSLIB_TSDEVICE")) == NULL) {
		printf("TSLIB_TSDEVICE Error\n");
        tsdevice = strdup ("/dev/event0");
    }
	ts = ts_open(tsdevice, 0);

	if (!ts) {	
		printf("ts_open error\n");		
		perror (tsdevice);		
		return -1;
	} else {
		printf("ts_open Success\n");
	}

	if (ts_config(ts)) {
		printf("ts_config error\n");
		//perror("ts_config");
		return -1;
	} else {
		printf("ts_config Success\n");
	}

	CreateDetachedTask(&tslib_thread_id, tslib_thread, NULL);							

	return 1;
}

void myts_init(void) {
	if(ts_init() == -1) {
		return;		
	}

    evdev_root_x = 0;	
    evdev_root_y = 0;
    evdev_button = LV_INDEV_STATE_REL;    
}

bool myts_read(lv_indev_drv_t * drv, lv_indev_data_t * data) {											
	data->point.x = evdev_root_x;
	data->point.y = evdev_root_y;
	data->state = evdev_button; 	
			
	if(data->point.x < 0)						
	  data->point.x = 0;
	if(data->point.y < 0)
	  data->point.y = 0;
	if(data->point.x >= LV_HOR_RES_MAX)
	  data->point.x = LV_HOR_RES_MAX - 1;		
	if(data->point.y >= LV_VER_RES_MAX)
	  data->point.y = LV_VER_RES_MAX - 1;											
				
	return false;
}