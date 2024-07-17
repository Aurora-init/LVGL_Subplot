
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#include "lv_100ask_teach_demos/lv_100ask_teach_demos.h"
#include "lv_demos/src/lv_demo_widgets/lv_demo_widgets.h"
#include "lv_drivers/win32drv/win32drv.h"

#include <windows.h>


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static int tick_thread(void *data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
    /*Initialize LittlevGL*/
    lv_init();

    /*Initialize the HAL for LittlevGL*/
    lv_win32_init(hInstance, SW_SHOWNORMAL, 1024, 600, NULL);

    /*Output prompt information to the console, you can also use printf() to print directly*/
    LV_LOG_USER("LVGL initialization completed!");
    //printf("www.100ask.net: Lvgl initialization complete!\n");


    /*Run the demo*/
    lv_demo();      // 基础对象(lv_obj)，"Hello, LVGL!"
	//lv_100ask_demo_course_2_2_2();    // 基础对象的大小(size)
	//lv_100ask_demo_course_2_2_3();    // 基础对象的位置(position)
	//lv_100ask_demo_course_2_2_4();    // 基础对象的盒子模型(border-box)
	//lv_100ask_demo_course_2_2_5();    // 基础对象的样式(styles)
	//lv_100ask_demo_course_2_2_6();    // 基础对象的事件(events)、事件冒泡

	//lv_100ask_demo_course_3_1_1();    // 组件(widgets)： 标签(label)的用法
	//lv_100ask_demo_course_3_1_2();    // 组件(widgets)： 标签(label)，显示中文
	//lv_100ask_demo_course_3_2_1();    // 组件(widgets)： 按钮(lv_btn)的用法
	//lv_100ask_demo_course_3_3_1();    // 组件(widgets)： 使用物理按键代替触摸(groups)
	//lv_100ask_demo_course_3_4_1();    // 组件(widgets)： 开关(lv_switch)的用法
	//lv_100ask_demo_course_3_5_1();    // 组件(widgets)： 复选框(lv_checkbox)的用法
	//lv_100ask_demo_course_3_6_1();    // 组件(widgets)： 下拉列表(lv_dropdown))的用法
	//lv_100ask_demo_course_3_7_1();    // 组件(widgets)： 滚轮(lv_roller)的用法
	//lv_100ask_demo_course_3_8_1();    // 组件(widgets)： 进度条(lv_bar)的用法
	//lv_100ask_demo_course_3_9_1();    // 组件(widgets)： 进度条(lv_slider)的用法
	//lv_100ask_demo_course_3_10_1();   // 组件(widgets)： 圆弧(lv_arc)的用法
	//lv_100ask_demo_course_3_10_3();   // 组件(widgets)： 圆弧(lv_arc)-综合实战
    //lv_100ask_demo_course_3_11_1();   // 定时器： 定时器(lv_timer)的用法
    //lv_100ask_demo_course_3_11_2();   // 为什么创建出来的组件获取到的宽高是0？(在学习2_2_2时遇到的问题)
    //lv_100ask_demo_course_3_11_3();   // 为什么创建出来的组件获取到的坐标不正确？(在学习2_2_3时遇到的问题)
    //lv_100ask_demo_course_3_12_1();   // 组件(widgets)： 线条(lv_line)的用法
    //lv_100ask_demo_course_3_12_2();   // 组件(widgets)： 线条(lv_line)实战
    //lv_100ask_demo_course_3_13_1();   // 组件(widgets)： 文本框(lv_textarea)的用法
    //lv_100ask_demo_course_3_14_1();   // 组件(widgets)： 按钮矩阵(lv_btnmatrix)的用法
    //lv_100ask_demo_course_3_14_2();   // 组件(widgets)： 按钮矩阵(lv_btnmatrix)和按钮(lv_btn)的对比(显示多个按钮谁更轻量级？)

    //lv_100ask_demo_course_6_1_1();    // 第三方库(lv_fs)：通过LVGL文件系统接口统一不同的文件系统并读取文件、读取目录内容
    //lv_100ask_demo_course_6_4_1();    // 第三方库(freeType)：通过freeType显示字体 (需要先在 lv_conf.h 中打开  LV_USE_FREETYPE)

    //lv_100ask_demo_course_x_1_1();    // 拓展高级学习：基于FontAwesome 添加新图标SYMBOLS，进入函数查看详细说明 (需要先在 lv_conf.h 中打开  LV_USE_FREETYPE)


    while(!lv_win32_quit_signal) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        usleep(10000);       /*Just to let the system breath*/
    }
    return 0;
}
