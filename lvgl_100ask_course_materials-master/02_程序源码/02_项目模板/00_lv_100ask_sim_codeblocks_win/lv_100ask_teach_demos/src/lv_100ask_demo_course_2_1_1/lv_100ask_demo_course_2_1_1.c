#include "../../lv_100ask_teach_demos.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

#define app_btn_num 21                       /**< 应用列表按键数 */
#define menu_btn_num 19                     /**< 下拉菜单按键数 */
#define menu_amin_speed_delay 360           /**< 下拉菜单动画持续时间 */
#define Covered_tiles_amin_speed_delay 300  /**< 瓷砖横杠下拉动画持续时间 */

/**< 时间字样字符串 */
char str[64];

/**< 播放器播放运行标志位 */
uint8_t player_run_flag = 0;

/**< 播放器播放模式标志位（repeat_mode、unrepeat_mode） */
uint8_t player_run_mode_set_flag = 0;

/**< 视频媒体流索引 */
uint8_t player_run_video_num = 0;

/**<用户操作回调函数变量*/
static uint16_t gpress_x = 0;   /**< 按下屏幕后x方向坐标 */
static uint16_t gmove_x = 0;    /**< 释放屏幕后x方向坐标 */
static uint16_t gpress_y = 0;   /**< 按下屏幕后y方向坐标 */
static uint16_t gmove_y = 0;    /**< 释放屏幕后y方向坐标 */
static uint8_t long_press = 0;  /**< 长按检测次数 */
static uint32_t last_time = 0;  /**< 用于计算dt */
static float x_speed = 0;       /**< x方向的线速度 */
static float y_speed = 0;       /**< y方向的线速度 */

 /** \brief
  * 用户操作回调函数，处理大多数对象方向上的滑动以及长按点击等事件。
  * \param  lv_event_t * e  对象触发回调函数的事件
  * \return void
  * main_obj_event_cb(lv_event_t * e);
  */
static void main_obj_event_cb(lv_event_t * e);

/** \brief
 * 位置类型的动画效果处理
 * \param lv_obj_t * obj            被操作的对象，指向需要被操作的对象的对象也可以
 * \param uint32_t delay            动画持续时长
 * \param lv_coord_t target         需要移动到的位置
 * \return void
 * auto_move(lv_obj_t * obj, uint32_t delay, lv_coord_t target);
 */
static void auto_move(lv_obj_t * obj, uint32_t delay, lv_coord_t target);

/** \brief
 * 大小类型的动画效果处理
 * \param lv_obj_t * obj            被操作的对象，指向需要被操作的对象的对象也可以
 * \param uint32_t delay            动画持续时长
 * \param lv_coord_t start_width    起始宽度
 * \param lv_coord_t target_width   目标宽度
 * \param lv_coord_t start_hight    起始高度
 * \param lv_coord_t target_hight   目标高度
 * \return void
 * auto_size_set(lv_obj_t * obj, uint32_t delay, lv_coord_t start_width, lv_coord_t target_width, lv_coord_t start_hight, lv_coord_t target_hight);
 */
static void auto_size_set(lv_obj_t * obj, uint32_t delay, lv_coord_t start_width, lv_coord_t target_width, lv_coord_t start_hight, lv_coord_t target_hight);

/** \brief
 *  日期更新定时器回调函数
 * \param lv_timer_t * timer        定时器对象
 * \return void
 *  timeset_timer_cb(lv_timer_t * timer);
 */
static void timeset_timer_cb(lv_timer_t * timer);

/** \brief
 *  视频进度条更新定时器回调函数
 * \param lv_timer_t * timer        定时器对象
 * \return void
 *  video_detect_timer_cb(lv_timer_t * timer);
 */
static void video_detect_timer_cb(lv_timer_t * timer);

/**< 视频切换功能依赖的变量及函数 */
const char *video_paths[] = {
    "C:/MOV/1.mov",
    "C:/MOV/2.mov",
    "C:/MOV/3.mov",
    "C:/MOV/4.mov",
    "C:/MOV/5.mov",
    "C:/MOV/6.mov",
    "C:/MOV/7.mov",
    "C:/MOV/8.mov"
};
const int video_count = sizeof(video_paths) / sizeof(video_paths[0]);

void play_video(lv_obj_t *player, lv_obj_t *img, int video_num) {
    if (video_num < 0 || video_num >= video_count) return;

    lv_ffmpeg_player_set_src(player, video_paths[video_num]);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_img_set_src(img, "C:/IMG/pause.png");
}

/**< 1.主界面 */
void lv_demo(void)
{
    //主页面背景
    lv_obj_t * main_page = lv_obj_create(lv_scr_act());
    //状态栏
    lv_obj_t * Status_Bar = lv_obj_create(main_page);
    //应用列表瓷砖背景
    lv_obj_t * appliction_bg = lv_tabview_create(main_page, LV_DIR_TOP, 0);
    lv_obj_t * appliction_tile1 = lv_tabview_add_tab(appliction_bg, "First");
    lv_obj_t * appliction_tile2 = lv_tabview_add_tab(appliction_bg, "Second");
    //应用列表按键
    lv_obj_t * app_btn_objs[app_btn_num];
    for (int i = 1; i <= app_btn_num; i++){
        app_btn_objs[i] = lv_btn_create(appliction_tile1);
    }
     lv_obj_clear_flag(appliction_tile1, LV_OBJ_FLAG_SCROLLABLE);
    //下拉菜单背景
    lv_obj_t * menu = lv_obj_create(main_page);
    //下拉菜单盒子1
    lv_obj_t * menu_box1 = lv_obj_create(menu);
    //下拉菜单盒子1按键区域2瓷砖背景
    lv_obj_t * menu_box1_btn2_bg = lv_tabview_create(menu_box1, LV_DIR_TOP, 0);
    lv_obj_t * tile1 = lv_tabview_add_tab(menu_box1_btn2_bg, "First");
    lv_obj_t * tile2 = lv_tabview_add_tab(menu_box1_btn2_bg, "Second");
    //下拉菜单盒子1按键
    lv_obj_t * menu_btn_objs[menu_btn_num];
    for (int i = 1; i <= menu_btn_num; i++){
        if(i <= 2){
            menu_btn_objs[i] = lv_btn_create(menu_box1);
        }
        else if(i <= 15){
            menu_btn_objs[i] = lv_btn_create(tile1);
        }
        else{
            menu_btn_objs[i] = lv_btn_create(tile2);
        }
    }
    //独属于亮度滑动条与亮度自动调节按钮的下拉状态栏、调光进度条，自动调节亮度按键、我的设备按键
    lv_obj_t * menu_covered_tiles = lv_obj_create(menu_box1);
    lv_obj_t * menu_box1_slider = lv_slider_create(menu_covered_tiles);
    lv_obj_t * slider_btn_obj = lv_btn_create(menu_covered_tiles);
    lv_obj_t * Covered_tiles_menu_set = lv_img_create(menu_covered_tiles);
    lv_obj_t * mydev_btn_obj = lv_btn_create(menu_covered_tiles);
    //下拉菜单盒子2
    lv_obj_t * menu_box2 = lv_obj_create(menu);
    lv_obj_t * player_btn_obj = lv_btn_create(menu_box2);
    lv_obj_t * GITHUB_qrcode_btn_obj = lv_btn_create(menu_box2);
    lv_obj_t * menu_box2_tabview = lv_tabview_create(menu_box2, LV_DIR_TOP, 0);
    //选项卡1播放器
    lv_obj_t * menu_box2_tabview_tab1 = lv_tabview_add_tab(menu_box2_tabview, "First");
    lv_obj_t * player = lv_ffmpeg_player_create(menu_box2_tabview_tab1);
    lv_obj_t * menu_box2_tabview_tab2= lv_tabview_add_tab(menu_box2_tabview, "Second");
    //二维码页面背景
    lv_obj_t * menu_box2_qr_bg = lv_obj_create(menu_box2);

    //1.主页面背景，也是应用列表界面//设置主界面的边框子对象间距离、边框透明度、边框宽度、主界面无圆角、主界面的位置位于屏幕中心
    lv_obj_set_size(main_page, LV_HOR_RES , LV_VER_RES);
    lv_obj_set_style_pad_top(main_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(main_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(main_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(main_page, 0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(main_page, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(main_page, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(main_page, 0, LV_PART_MAIN);
    lv_obj_align(main_page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(main_page, LV_OBJ_FLAG_SCROLLABLE);

    //2.状态栏，用于放置时间、电量、流量信息、信号强度等（）
    lv_obj_set_size(Status_Bar, LV_HOR_RES , LV_VER_RES/10);
    lv_obj_align_to(Status_Bar, main_page, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_border_width(Status_Bar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(Status_Bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(Status_Bar, LV_OBJ_FLAG_SCROLLABLE);
    Status_Bar->user_data = menu;
    Status_Bar->num = Status_Bar_num;
    //状态栏的日期字样
    lv_obj_t * label_time = lv_label_create(Status_Bar);
    lv_obj_set_size(label_time, LV_HOR_RES/10, LV_VER_RES/10);
    lv_obj_align_to(label_time, Status_Bar, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(label_time, str);
    lv_obj_set_style_text_color(label_time, lv_color_make(24, 24, 24), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label_time, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //3.主页面应用列表
    lv_obj_set_size(appliction_bg, LV_HOR_RES, LV_VER_RES*3/4);
    lv_obj_align_to(appliction_bg, main_page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(appliction_bg, lv_color_make(0XFF, 0XFF, 0XFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(appliction_bg, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(appliction_bg, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);              //隐藏滚轮
    lv_obj_set_scroll_dir(appliction_bg, LV_DIR_HOR);                                                     //设置滚动方向,LV_DIR_HOR水平滚动,LV_DIR_VER垂直滚动
    //主页面应用按键控件样式
    for(int i = 1; i <= app_btn_num; i++){
        lv_obj_set_size(app_btn_objs[i], LV_HOR_RES/10, LV_HOR_RES/10);
        lv_obj_set_style_bg_color(app_btn_objs[i], lv_color_make(230, 230, 230), LV_PART_MAIN);
        lv_obj_set_style_border_width(app_btn_objs[i], 1, LV_PART_MAIN);
        lv_obj_set_style_border_opa(app_btn_objs[i], 128, LV_PART_MAIN);
        lv_obj_set_style_radius(app_btn_objs[i], 35, LV_PART_MAIN);
        if(i <= 7){
            lv_obj_align_to(app_btn_objs[i], appliction_bg, LV_ALIGN_TOP_LEFT, (i-1)*LV_HOR_RES/8+76, LV_VER_RES*1/20);
        }
        else if(i <= 14){
            lv_obj_align_to(app_btn_objs[i], appliction_bg, LV_ALIGN_TOP_LEFT, (i-8)*LV_HOR_RES/8+76, LV_VER_RES*6/20);
        }
        else if(i <= 21){
            lv_obj_align_to(app_btn_objs[i], appliction_bg, LV_ALIGN_TOP_LEFT, (i-15)*LV_HOR_RES/8+76, LV_VER_RES*11/20);
        }
        app_btn_objs[i]->num = i*10;
    }

    //4.下拉生成的菜单栏,完全隐藏情况下就一条边，完全显现情况下可以依稀看到一点背景的图片，大小是一个屏幕大小
    lv_obj_set_size(menu, LV_HOR_RES , LV_VER_RES);
    lv_obj_align_to(menu, main_page, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(menu, lv_color_make(64, 64, 64), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu, 250, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu, 0, LV_PART_MAIN);
    lv_obj_clear_flag(menu, LV_OBJ_FLAG_SCROLLABLE);
    menu->num = menu_num;
    //4.1下拉菜单的时间及日期字样对象
    lv_obj_t * menu_label_time = lv_label_create(menu);
    lv_obj_set_size(menu_label_time, LV_HOR_RES/10, LV_VER_RES/10);
    lv_obj_align_to(menu_label_time, menu, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(menu_label_time, str);
    lv_obj_t * menu_label_data = lv_label_create(menu);
    lv_obj_set_size(menu_label_data, LV_HOR_RES/5, LV_VER_RES/5);
    lv_obj_align_to(menu_label_data, menu, LV_ALIGN_TOP_LEFT, LV_HOR_RES/13, 2);
    lv_label_set_text(menu_label_data, "July 5th  Friday");
    lv_obj_set_style_text_color(menu_label_time, lv_color_make(200, 200, 200), LV_PART_MAIN);
    lv_obj_set_style_text_font(menu_label_time, &lv_font_montserrat_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(menu_label_data, lv_color_make(200, 200, 200), LV_PART_MAIN);
    lv_obj_set_style_text_font(menu_label_data, &lv_font_montserrat_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    //4.2设置小齿轮图片按键
    lv_obj_t * set_img = lv_img_create(menu);
    lv_img_set_src(set_img, "C:/IMG/set.png");
    lv_obj_align_to(set_img, menu, LV_ALIGN_TOP_LEFT, LV_HOR_RES*3/12, -2);
    //4.3下拉菜单的下拉栏目图片
    lv_obj_t * menu_set_img = lv_img_create(menu);
    lv_img_set_src(menu_set_img, "C:/IMG/menu_set.png");
    lv_obj_align_to(menu_set_img, menu, LV_ALIGN_BOTTOM_MID, 0, LV_VER_RES/10);

    //5.下拉菜单盒子1
    lv_obj_set_size(menu_box1, LV_HOR_RES*5/17 , LV_VER_RES*11/13);
    lv_obj_align_to(menu_box1, menu, LV_ALIGN_TOP_LEFT, 0, LV_VER_RES*1/16);
    lv_obj_set_style_bg_color(menu_box1, lv_color_make(240, 240, 240), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_box1, 0xff, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_box1, 3, LV_PART_MAIN);
    lv_obj_set_style_border_opa(menu_box1,0xff,LV_PART_MAIN);
    lv_obj_set_style_border_color(menu_box1, lv_color_make(128, 128, 128), LV_PART_MAIN);
    lv_obj_set_style_pad_top(menu_box1, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_box1, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu_box1, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_right(menu_box1, 12, LV_PART_MAIN);
    lv_obj_set_style_radius(menu_box1, 25, LV_PART_MAIN);
    lv_obj_clear_flag(menu_box1, LV_OBJ_FLAG_SCROLLABLE);
    //5.1.下拉菜单盒子内的按键区域2背景瓷砖
    lv_obj_set_size(menu_box1_btn2_bg, LV_HOR_RES*6/24 , LV_VER_RES*5/21);
    lv_obj_align_to(menu_box1_btn2_bg, menu_box1, LV_ALIGN_TOP_LEFT, 0, LV_VER_RES*2/17);
    lv_obj_set_style_bg_color(menu_box1_btn2_bg, lv_color_make(240, 240, 240), LV_PART_MAIN);
    lv_obj_clear_flag(tile1, LV_OBJ_FLAG_SCROLLABLE);
    //菜单栏按键
    for(int i = 1; i <= menu_btn_num; i++)
    {
        lv_obj_set_size(menu_btn_objs[i], 38, 38);
        lv_obj_set_style_radius(menu_btn_objs[i], 25, LV_PART_MAIN);
        lv_obj_set_style_bg_color(menu_btn_objs[i], lv_color_make(210,210,210), LV_PART_MAIN);
        lv_obj_set_style_bg_color(menu_btn_objs[i], lv_color_make(173, 216, 230), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(menu_btn_objs[i], lv_palette_main(LV_PALETTE_BLUE_GREY), LV_STATE_CHECKED);
        lv_obj_add_flag(menu_btn_objs[i], LV_OBJ_FLAG_CHECKABLE);
        if(i <= 2)
        {
            lv_obj_set_size(menu_btn_objs[i], 128, 64);
            lv_obj_align_to(menu_btn_objs[i], menu_box1, LV_ALIGN_TOP_LEFT, (i-1)*LV_HOR_RES*3/23, 0);
            lv_obj_set_style_bg_color(menu_btn_objs[i], lv_color_make(220, 220, 220), LV_PART_MAIN);
            lv_obj_set_style_bg_color(menu_btn_objs[i], lv_color_make(173, 216, 230), LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(menu_btn_objs[i], lv_palette_main(LV_PALETTE_BLUE_GREY), LV_STATE_CHECKED);
            lv_obj_set_style_border_width(menu_btn_objs[i], 1, LV_PART_MAIN);
            lv_obj_set_style_border_opa(menu_btn_objs[i], 128, LV_PART_MAIN);
            lv_obj_set_style_radius(menu_btn_objs[i], 15, LV_PART_MAIN);
            lv_obj_add_flag(menu_btn_objs[i], LV_OBJ_FLAG_CHECKABLE);
            if(i == 1)
            {
                lv_obj_t * img_box = lv_obj_create(menu_btn_objs[i]);
                lv_obj_set_size(img_box, LV_HOR_RES*5/43, LV_VER_RES*3/34);
                lv_obj_align_to(img_box, menu_btn_objs[i], LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_style_bg_color(img_box, lv_color_make(250, 250, 250), LV_PART_MAIN);
                lv_obj_clear_flag(img_box, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_add_flag(img_box, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_set_style_pad_top(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_bottom(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_left(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_right(img_box, 0, LV_PART_MAIN);
                lv_obj_t * img = lv_img_create(img_box);
                lv_img_set_src(img, "C:/IMG/wifi.png");
                lv_obj_align(img, LV_ALIGN_LEFT_MID,16, 0);
                lv_obj_t * label_WIFI = lv_label_create(img_box);
                lv_obj_set_size(label_WIFI, LV_HOR_RES/18, LV_VER_RES/19);
                lv_label_set_text(label_WIFI, "WiFi");
                lv_obj_set_style_text_color(label_WIFI, lv_color_make(24, 24, 24), LV_PART_MAIN);
                lv_obj_align_to(label_WIFI, img_box, LV_ALIGN_LEFT_MID, LV_HOR_RES*1/27,LV_VER_RES*1/64);
                lv_obj_set_style_text_font(label_WIFI, &lv_font_montserrat_10, LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_set_style_text_align(label_WIFI, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_t * wifi_box_btn = lv_btn_create(img_box);
                lv_obj_set_size(wifi_box_btn, 16, 16);
                lv_obj_set_style_bg_color(wifi_box_btn, lv_color_make(220, 220, 220), LV_PART_MAIN);
                lv_obj_align_to(wifi_box_btn, img_box, LV_ALIGN_RIGHT_MID, -3, 0);
                img = lv_img_create(wifi_box_btn);
                lv_img_set_src(img, "C:/IMG/right_arrow.png");
                lv_obj_align_to(img, wifi_box_btn, LV_ALIGN_CENTER, 0,  1);
            }
            else if(i == 2)
            {
                lv_obj_t * img_box = lv_obj_create(menu_btn_objs[i]);
                lv_obj_set_size(img_box, LV_HOR_RES*5/43, LV_VER_RES*3/34);
                lv_obj_align_to(img_box, menu_btn_objs[i], LV_ALIGN_CENTER, 0, 0);
                lv_obj_set_style_bg_color(img_box, lv_color_make(250, 250, 250), LV_PART_MAIN);
                lv_obj_clear_flag(img_box, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_add_flag(img_box, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_set_style_pad_top(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_bottom(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_left(img_box, 0, LV_PART_MAIN);
                lv_obj_set_style_pad_right(img_box, 0, LV_PART_MAIN);
                lv_obj_t * img = lv_img_create(img_box);
                lv_img_set_src(img, "C:/IMG/bluez.png");
                lv_obj_align(img, LV_ALIGN_LEFT_MID,13, 0);
                lv_obj_t * label_BLUZ = lv_label_create(img_box);
                lv_obj_set_size(label_BLUZ, LV_HOR_RES/18, LV_VER_RES/19);
                lv_label_set_text(label_BLUZ, "Bluetooth");
                lv_obj_set_style_text_color(label_BLUZ, lv_color_make(24, 24, 24), LV_PART_MAIN);
                lv_obj_align_to(label_BLUZ, img_box, LV_ALIGN_LEFT_MID, LV_HOR_RES*1/31,LV_VER_RES*1/64);
                lv_obj_set_style_text_font(label_BLUZ, &lv_font_montserrat_10, LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_set_style_text_align(label_BLUZ, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_t * bluz_box_btn = lv_btn_create(img_box);
                lv_obj_set_size(bluz_box_btn, 16, 16);
                lv_obj_set_style_bg_color(bluz_box_btn, lv_color_make(220, 220, 220), LV_PART_MAIN);
                lv_obj_align_to(bluz_box_btn, img_box, LV_ALIGN_RIGHT_MID, -3, 0);
                img = lv_img_create(bluz_box_btn);
                lv_img_set_src(img, "C:/IMG/right_arrow.png");
                lv_obj_align_to(img, bluz_box_btn, LV_ALIGN_CENTER, 0,  1);
            }
        }
        else if(i > 2 && i <= 7)
        {
            lv_obj_t * img = lv_img_create(menu_btn_objs[i]);
            (i<=3)?lv_img_set_src(img, "C:/IMG/moble_data.png"):((i<=4)?lv_img_set_src(img, "C:/IMG/light.png"):((i<=5)?lv_img_set_src(img, "C:/IMG/post.png"):((i<=6)?lv_img_set_src(img, "C:/IMG/fly_mode.png"):((i<=7)?lv_img_set_src(img, "C:/IMG/Screen_rotation.png"):lv_img_set_src(img, "C:/IMG/Screen_rotation.png")))));
            lv_obj_align_to(img, menu_btn_objs[i], LV_ALIGN_CENTER, 0,  0);
            lv_obj_align_to(menu_btn_objs[i], menu_box1, LV_ALIGN_TOP_LEFT, 8+(i-3)*50,  76);
        }
        else if(i > 7 && i <= 11)
        {
            lv_obj_t * img = lv_img_create(menu_btn_objs[i]);
            (i<=8)?lv_img_set_src(img, "C:/IMG/Screen_rotation.png"):((i<=9)?lv_img_set_src(img, "C:/IMG/Ambient Light.png"):((i<=10)?lv_img_set_src(img, "C:/IMG/img_improve.png"):(i<=11)?lv_img_set_src(img, "C:/IMG/peye_mode.png"):(void)0));
            lv_obj_align_to(img, menu_btn_objs[i], LV_ALIGN_CENTER, 0,  0);
            lv_obj_align_to(menu_btn_objs[i], menu_box1, LV_ALIGN_TOP_LEFT, 17+(i-8)*62,  123);
        }
        else if(i > 11 && i <= 15)
        {
            lv_obj_t * img = lv_img_create(menu_btn_objs[i]);
            (i<=12)?lv_img_set_src(img, "C:/IMG/power_saving_mode.png"):((i<=13)?lv_img_set_src(img, "C:/IMG/night_mode.png"):((i<=14)?lv_img_set_src(img, "C:/IMG/screenshot.png"):(i<=15)?lv_img_set_src(img, "C:/IMG/recording.png"):(void)0));
            lv_obj_align_to(img, menu_btn_objs[i], LV_ALIGN_CENTER, 0,  0);
            lv_obj_align_to(menu_btn_objs[i], menu_box1, LV_ALIGN_TOP_LEFT, 17+(i-12)*62,  170);
        }
        else if(i > 15 && i <= 19)
        {
            lv_obj_t * img = lv_img_create(menu_btn_objs[i]);
            (i<=16)?lv_img_set_src(img, "C:/IMG/power_saving_mode.png"):((i<=17)?lv_img_set_src(img, "C:/IMG/night_mode.png"):((i<=18)?lv_img_set_src(img, "C:/IMG/screenshot.png"):(i<=19)?lv_img_set_src(img, "C:/IMG/recording.png"):(void)0));
            lv_obj_align_to(img, menu_btn_objs[i], LV_ALIGN_CENTER, 0,  0);
            lv_obj_align_to(menu_btn_objs[i], menu_box1, LV_ALIGN_TOP_LEFT, 17+(i-12)*62,  76);
        }
        menu_btn_objs[i]->user_data = menu_btn_objs[i+1];
        menu_btn_objs[i]->num = i*11;
    }

    //6.创建一个独属于亮度滑动条与亮度自动调节按钮的下拉状态栏
    lv_obj_set_size(menu_covered_tiles, LV_HOR_RES*6/23 , LV_VER_RES*7/13);
    lv_obj_align_to(menu_covered_tiles, menu_box1, LV_ALIGN_LEFT_MID, 0, LV_VER_RES*1/15);
    lv_obj_set_style_bg_color(menu_covered_tiles, lv_color_make(240, 240, 240), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_covered_tiles, 0xff, LV_PART_MAIN);
    lv_obj_set_style_pad_top(menu_covered_tiles, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_covered_tiles, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu_covered_tiles, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(menu_covered_tiles, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_covered_tiles, 0, LV_PART_MAIN);
    menu_covered_tiles->num = menu_covered_tiles_num;
    menu_covered_tiles->user_data = menu_btn_objs[3];
    //调节亮度滑动条样式
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
    static lv_style_transition_dsc_t transition_dsc;
    lv_style_transition_dsc_init(&transition_dsc, props, lv_anim_path_linear, 300, 0, NULL);
    static lv_style_t style_main;
    static lv_style_t style_indicator;
    static lv_style_t style_pressed_color;
    lv_style_init(&style_main);
    lv_style_set_bg_opa(&style_main, LV_OPA_COVER);
    lv_style_set_bg_color(&style_main, lv_color_hex3(0xbbb));
    lv_style_set_radius(&style_main, LV_RADIUS_CIRCLE);
    lv_style_set_pad_ver(&style_main, 0);
    lv_style_init(&style_indicator);
    lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indicator, lv_palette_main(LV_PALETTE_BLUE_GREY));
    lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);
    lv_style_set_transition(&style_indicator, &transition_dsc);
    lv_style_init(&style_pressed_color);
    lv_style_set_bg_color(&style_pressed_color, lv_palette_darken(LV_PALETTE_BROWN, 2));
    //调节亮度滑动条
    lv_obj_set_size(menu_box1_slider, 220, 36);
    lv_obj_align_to(menu_box1_slider, menu_covered_tiles, LV_ALIGN_TOP_LEFT,0,12);
    lv_obj_set_style_bg_opa(menu_box1_slider, 0, LV_PART_KNOB);
    lv_obj_set_style_height(menu_box1_slider, 20, LV_PART_INDICATOR);
    lv_slider_set_value(menu_box1_slider, 20, LV_ANIM_OFF);
    lv_obj_add_style(menu_box1_slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(menu_box1_slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(menu_box1_slider, &style_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(menu_box1_slider, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_t * slider_obj_img = lv_img_create(menu_box1_slider);
    lv_img_set_src(slider_obj_img, "C:/IMG/sun.png");
    lv_obj_align_to(slider_obj_img, menu_box1_slider, LV_ALIGN_LEFT_MID, 11,  0);
    //亮度自动调节按键
    lv_obj_set_size(slider_btn_obj, 36, 36);
    lv_obj_add_flag(slider_btn_obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_align_to(slider_btn_obj, menu_covered_tiles, LV_ALIGN_TOP_LEFT, LV_HOR_RES*2/9,12);
    lv_obj_set_style_radius(slider_btn_obj, 25, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_btn_obj, lv_color_make(210,210,210), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_btn_obj, lv_color_make(173, 216, 230), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(slider_btn_obj, lv_color_make(0Xd2, 0XB4, 0X8C), LV_STATE_CHECKED);
    lv_obj_t * slider_btn_obj_img = lv_img_create(slider_btn_obj);
    lv_img_set_src(slider_btn_obj_img, "C:/IMG/sunset.png");
    lv_obj_align_to(slider_btn_obj_img, slider_btn_obj, LV_ALIGN_CENTER, 0,  0);
    lv_img_set_src(Covered_tiles_menu_set, "C:/IMG/tiles_menu_set.png");
    lv_obj_align_to(Covered_tiles_menu_set, menu_covered_tiles, LV_ALIGN_TOP_MID, 0, 26);
    //我的设备按键
    lv_obj_set_size(mydev_btn_obj, LV_HOR_RES*7/28, LV_VER_RES*1/13);
    lv_obj_align_to(mydev_btn_obj, menu_covered_tiles, LV_ALIGN_LEFT_MID, 0,-82);
    lv_obj_set_style_radius(mydev_btn_obj, 15, LV_PART_MAIN);
    lv_obj_set_style_bg_color(mydev_btn_obj, lv_color_make(210,210,210), LV_PART_MAIN);
    lv_obj_set_style_bg_color(mydev_btn_obj, lv_color_make(173, 216, 230), LV_STATE_PRESSED);
    lv_obj_set_style_opa(mydev_btn_obj, 255, LV_PART_MAIN);
    lv_obj_clear_flag(mydev_btn_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(mydev_btn_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_t * mydev_btn_obj_img1 = lv_img_create(mydev_btn_obj);
    lv_img_set_src(mydev_btn_obj_img1, "C:/IMG/right_arrow.png");
    lv_obj_align_to(mydev_btn_obj_img1, mydev_btn_obj, LV_ALIGN_RIGHT_MID, 0,  0);
    lv_obj_t * mydev_btn_obj_img2 = lv_img_create(mydev_btn_obj);
    lv_img_set_src(mydev_btn_obj_img2, "C:/IMG/home.png");
    lv_obj_align_to(mydev_btn_obj_img2, mydev_btn_obj, LV_ALIGN_LEFT_MID, 0,  0);
    lv_obj_t * mydev_btn_label = lv_label_create(mydev_btn_obj);
    lv_obj_align_to(mydev_btn_label, mydev_btn_obj, LV_ALIGN_CENTER, -66, 0);
    lv_label_set_text(mydev_btn_label, "My Device");
    lv_obj_set_style_text_color(mydev_btn_label, lv_color_make(24, 24, 24), LV_PART_MAIN);
    lv_obj_set_style_text_font(mydev_btn_label, &lv_font_montserrat_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_size(mydev_btn_obj, 0, 0);
    mydev_btn_obj->num = mydev_btn_obj_num;
    menu_covered_tiles->user_data_backup = mydev_btn_obj;

    //7.下拉菜单盒子2
    lv_obj_set_size(menu_box2, LV_HOR_RES*11/17*99/100 , LV_VER_RES*11/13);
    lv_obj_align_to(menu_box2, menu, LV_ALIGN_TOP_RIGHT, -7, LV_VER_RES*1/16);
    lv_obj_set_style_bg_color(menu_box2, lv_color_make(240, 240, 240), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_box2, 0xff, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_box2, 3, LV_PART_MAIN);
    lv_obj_set_style_border_opa(menu_box2,0xff,LV_PART_MAIN);
    lv_obj_set_style_border_color(menu_box2, lv_color_make(128, 128, 128), LV_PART_MAIN);
    lv_obj_set_style_pad_top(menu_box2, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_box2, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu_box2, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_right(menu_box2, 12, LV_PART_MAIN);
    lv_obj_set_style_radius(menu_box2, 25, LV_PART_MAIN);
    lv_obj_clear_flag(menu_box2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(menu_box2, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu_box2->num = menu_box2_num;
    menu_box2->user_data = menu_box2_qr_bg;
    //下拉菜单盒子的第一个应用按键player
    lv_obj_set_size(player_btn_obj, LV_HOR_RES*1/13, LV_HOR_RES*1/13);
    lv_obj_set_style_bg_color(player_btn_obj, lv_color_make(220, 220, 220), LV_PART_MAIN);
    lv_obj_set_style_border_width(player_btn_obj, 6, LV_PART_MAIN);
    lv_obj_set_style_border_opa(player_btn_obj, 128, LV_PART_MAIN);
    lv_obj_set_style_radius(player_btn_obj, 15, LV_PART_MAIN);
    lv_obj_align_to(player_btn_obj, menu_box2, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t * player_btn_obj_img2 = lv_img_create(player_btn_obj);
    lv_img_set_src(player_btn_obj_img2, "C:/IMG/player_btn.png");
    lv_obj_align_to(player_btn_obj_img2, player_btn_obj, LV_ALIGN_CENTER, 3,  0);
    player_btn_obj->num = player_btn_obj_num;
    //下拉菜单盒子的第二个应用按键githubQR
    lv_obj_set_size(GITHUB_qrcode_btn_obj, LV_HOR_RES*1/13, LV_HOR_RES*1/13);
    lv_obj_set_style_bg_color(GITHUB_qrcode_btn_obj, lv_color_make(220, 220, 220), LV_PART_MAIN);
    lv_obj_set_style_border_width(GITHUB_qrcode_btn_obj, 6, LV_PART_MAIN);
    lv_obj_set_style_border_opa(GITHUB_qrcode_btn_obj, 128, LV_PART_MAIN);
    lv_obj_set_style_radius(GITHUB_qrcode_btn_obj, 15, LV_PART_MAIN);
    lv_obj_align_to(GITHUB_qrcode_btn_obj, menu_box2, LV_ALIGN_TOP_LEFT, LV_HOR_RES/10, 0);
    lv_obj_t * GITHUB_qrcode_btn_obj_img = lv_img_create(GITHUB_qrcode_btn_obj);
    lv_img_set_src(GITHUB_qrcode_btn_obj_img, "C:/IMG/github.png");
    lv_obj_align_to(GITHUB_qrcode_btn_obj_img, GITHUB_qrcode_btn_obj, LV_ALIGN_CENTER, 0,  0);
    GITHUB_qrcode_btn_obj->num = GITHUB_qrcode_btn_obj_num;
    //下拉菜单盒子2选项卡
    lv_obj_set_size(menu_box2_tabview, LV_HOR_RES*11/18, LV_VER_RES*11/14);
    lv_obj_clear_flag(menu_box2_tabview, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(menu_box2_tabview, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(menu_box2_tabview, lv_color_make(240, 240, 240), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_box2_tabview, LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_border_opa(menu_box2_tabview, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(menu_box2_tabview, 0, LV_PART_MAIN);
    lv_obj_add_flag(menu_box2_tabview, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(lv_tabview_get_content(menu_box2_tabview), LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(menu_box2_tabview_tab1, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_pad_top(menu_box2_tabview_tab1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_box2_tabview_tab1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu_box2_tabview_tab1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(menu_box2_tabview_tab1, 0, LV_PART_MAIN);
    lv_obj_add_flag(menu_box2_tabview_tab2, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu_box2_tabview->num = menu_box2_tabview_num;
    player_btn_obj->user_data = menu_box2_tabview;
    //8.下拉菜单盒子2FFMPEG播放盒子
    lv_ffmpeg_player_set_src(player, "C:/MOV/1.mov");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_STOP);
    player_run_flag = init_state;
    lv_obj_align_to(player, menu_box2_tabview_tab1, LV_ALIGN_CENTER, 0, -30);
    lv_obj_clear_flag(menu_box2_tabview_tab1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(player, 25, LV_PART_MAIN);
    //播放盒子控制菜单
    lv_obj_t * player_ctrl_menu = lv_obj_create(menu_box2_tabview_tab1);
    lv_obj_set_size(player_ctrl_menu, LV_HOR_RES*11/18, LV_VER_RES*1/12);
    lv_obj_align(player_ctrl_menu, LV_ALIGN_TOP_LEFT, 0, 400);
    lv_obj_set_style_radius(player_ctrl_menu, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_ctrl_menu, lv_color_make(64, 64, 64), LV_PART_MAIN);
    lv_obj_set_style_border_width(player_ctrl_menu, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(player_ctrl_menu, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(player_ctrl_menu, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(player_ctrl_menu, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(player_ctrl_menu, 0, LV_PART_MAIN);
    lv_obj_clear_flag(player_ctrl_menu, LV_OBJ_FLAG_SCROLLABLE);
    //播放盒子控制菜单播放按键
    lv_obj_t * player_ctrl_menu_play_pause_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_ctrl_menu_play_pause_btn, 32, 32);
    lv_obj_align_to(player_ctrl_menu_play_pause_btn, player_ctrl_menu, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(player_ctrl_menu_play_pause_btn, 25, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_ctrl_menu_play_pause_btn, lv_color_make(230, 230, 230), LV_PART_MAIN);
    lv_obj_t * player_ctrl_menu_play_pause_btn_img = lv_img_create(player_ctrl_menu_play_pause_btn);
    lv_img_set_src(player_ctrl_menu_play_pause_btn_img, "C:/IMG/play.png");
    lv_obj_align_to(player_ctrl_menu_play_pause_btn_img, player_ctrl_menu_play_pause_btn, LV_ALIGN_CENTER, 0, 0);
    player_ctrl_menu_play_pause_btn->num = player_ctrl_menu_play_pause_btn_num;
    player_ctrl_menu_play_pause_btn->user_data = player;
    player_ctrl_menu_play_pause_btn->user_data_backup = player_ctrl_menu_play_pause_btn_img;
    //播放盒子控制菜单视频循环模型按键
    lv_obj_t * player_ctrl_playmode_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_ctrl_playmode_btn, 32, 32);
    lv_obj_align_to(player_ctrl_playmode_btn, player_ctrl_menu, LV_ALIGN_CENTER, 100, 0);
    lv_obj_set_style_radius(player_ctrl_playmode_btn, 25, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_ctrl_playmode_btn, lv_color_make(230, 230, 230), LV_PART_MAIN);
    lv_obj_t * player_ctrl_playmode_btn_img = lv_img_create(player_ctrl_playmode_btn);
    lv_img_set_src(player_ctrl_playmode_btn_img, "C:/IMG/repeat.png");
    player_run_mode_set_flag = repeat_mode;
    lv_obj_align_to(player_ctrl_playmode_btn_img, player_ctrl_playmode_btn, LV_ALIGN_CENTER, 0, 0);
    player_ctrl_playmode_btn->num = player_ctrl_playmode_btn_num;
    player_ctrl_playmode_btn->user_data = player;
    player_ctrl_playmode_btn->user_data_backup = player_ctrl_playmode_btn_img;
    //切换视频按键(下一个)
    lv_obj_t * player_ctrl_next_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_ctrl_next_btn, 32, 32);
    lv_obj_align_to(player_ctrl_next_btn, player_ctrl_menu, LV_ALIGN_CENTER, 50, 0);
    lv_obj_set_style_radius(player_ctrl_next_btn, 25, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_ctrl_next_btn, lv_color_make(230, 230, 230), LV_PART_MAIN);
    lv_obj_t * player_ctrl_next_btn_img = lv_img_create(player_ctrl_next_btn);
    lv_img_set_src(player_ctrl_next_btn_img, "C:/IMG/next_btn.png");
    lv_obj_align_to(player_ctrl_next_btn_img, player_ctrl_next_btn, LV_ALIGN_CENTER, 0, 0);
    player_ctrl_next_btn->num = player_ctrl_next_btn_num;
    player_ctrl_next_btn->user_data = player;
    player_ctrl_next_btn->user_data_backup = player_ctrl_menu_play_pause_btn_img;
    //切换视频按键(上一个)
    lv_obj_t * player_ctrl_last_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_ctrl_last_btn, 32, 32);
    lv_obj_align_to(player_ctrl_last_btn, player_ctrl_menu, LV_ALIGN_CENTER, -50, 0);
    lv_obj_set_style_radius(player_ctrl_last_btn, 25, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_ctrl_last_btn, lv_color_make(230, 230, 230), LV_PART_MAIN);
    lv_obj_t * player_ctrl_last_btn_img = lv_img_create(player_ctrl_last_btn);
    lv_img_set_src(player_ctrl_last_btn_img, "C:/IMG/last_btn.png");
    lv_obj_align_to(player_ctrl_last_btn_img, player_ctrl_last_btn, LV_ALIGN_CENTER, 0, 0);
    player_ctrl_last_btn->num = player_ctrl_last_btn_num;
    player_ctrl_last_btn->user_data = player;
    player_ctrl_last_btn->user_data_backup = player_ctrl_menu_play_pause_btn_img;
    //两个按键要能互相影响
    player_ctrl_last_btn->user_data_backup_backup = player_ctrl_next_btn;
    player_ctrl_next_btn->user_data_backup_backup = player_ctrl_last_btn;
    if(player_run_video_num == 0)
    {
        lv_obj_clear_flag(player_ctrl_last_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(player_ctrl_last_btn, lv_color_make(150, 150, 150), LV_PART_MAIN);
    }
    else if(player_run_video_num == 1)
    {
        lv_obj_clear_flag(player_ctrl_next_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(player_ctrl_next_btn, lv_color_make(150, 150, 150), LV_PART_MAIN);
    }
    //关闭视频按键
    lv_obj_t * player_close_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_close_btn, 32, 32);
    lv_obj_align_to(player_close_btn, player_ctrl_menu, LV_ALIGN_CENTER, 290, 0);
    lv_obj_set_style_radius(player_close_btn, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_close_btn, lv_color_make(150, 150, 150), LV_PART_MAIN);
    lv_obj_t * player_close_btn_img = lv_img_create(player_close_btn);
    lv_img_set_src(player_close_btn_img, "C:/IMG/close_btn.png");
    lv_obj_align_to(player_close_btn_img, player_close_btn, LV_ALIGN_CENTER, 0, 0);
    player_close_btn->num = player_close_btn_num;
    //最小化视频按键
    lv_obj_t * player_minimize_btn = lv_btn_create(player_ctrl_menu);
    lv_obj_set_size(player_minimize_btn, 32, 32);
    lv_obj_align_to(player_minimize_btn, player_ctrl_menu, LV_ALIGN_CENTER, 250, 0);
    lv_obj_set_style_radius(player_minimize_btn, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(player_minimize_btn, lv_color_make(150, 150, 150), LV_PART_MAIN);
    lv_obj_t * player_minimize_btn_img = lv_img_create(player_minimize_btn);
    lv_img_set_src(player_minimize_btn_img, "C:/IMG/minimize.png");
    lv_obj_align_to(player_minimize_btn_img, player_minimize_btn, LV_ALIGN_CENTER, 0, 0);
    player_minimize_btn->num = player_minimize_btn_num;
    player_minimize_btn->user_data = menu_box2_tabview;
    auto_size_set(menu_box2_tabview, 10, LV_HOR_RES*11/18, 0, LV_VER_RES*11/14, 0);
    //视频进度条
    lv_obj_t * player_ctrl_menu_slider = lv_slider_create(player_ctrl_menu);
    lv_obj_set_size(player_ctrl_menu_slider, LV_HOR_RES*11/18, 3);
    lv_obj_align_to(player_ctrl_menu_slider, player_ctrl_menu, LV_ALIGN_TOP_LEFT,0,1);
    lv_obj_set_style_bg_opa(player_ctrl_menu_slider, 0, LV_PART_KNOB);
    lv_obj_set_style_height(player_ctrl_menu_slider, 20, LV_PART_INDICATOR);
    lv_slider_set_value(player_ctrl_menu_slider, 1, LV_ANIM_OFF);
    lv_obj_add_style(player_ctrl_menu_slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(player_ctrl_menu_slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(player_ctrl_menu_slider, &style_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(player_ctrl_menu_slider, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_clear_flag(player_ctrl_menu_slider, LV_OBJ_FLAG_CLICKABLE);


    //x.下拉菜单盒子2二维码盒子背景
    lv_obj_set_size(menu_box2_qr_bg, LV_HOR_RES*13/43 , LV_VER_RES*10/13);
    lv_obj_align_to(menu_box2_qr_bg, menu_box2, LV_ALIGN_RIGHT_MID, 400, 0);
    lv_obj_set_style_pad_top(menu_box2_qr_bg, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(menu_box2_qr_bg, 8, LV_PART_MAIN);
    lv_obj_set_style_radius(menu_box2_qr_bg, 25, LV_PART_MAIN);
    lv_obj_clear_flag(menu_box2_qr_bg, LV_OBJ_FLAG_SCROLLABLE);
    menu_box2_qr_bg->num = menu_box2_qr_bg_num;
    //下拉菜单二维码
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    lv_obj_t * qr1 = lv_qrcode_create(menu_box2_qr_bg, 155, fg_color, bg_color);
    const char * data = "https://github.com/Aurora-init/LVGL_Subplot/";
    lv_qrcode_update(qr1, data, strlen(data));
    lv_obj_align_to(qr1, menu_box2_qr_bg, LV_ALIGN_CENTER, 0, -66);
    lv_obj_set_style_border_color(qr1, bg_color, 0);
    lv_obj_set_style_border_width(qr1, 5, 0);
    lv_obj_t * qr_label = lv_label_create(menu_box2_qr_bg);
    lv_obj_set_size(qr_label, LV_HOR_RES/4, LV_VER_RES/5);
    lv_obj_align_to(qr_label, menu_box2_qr_bg, LV_ALIGN_CENTER, 10, 100);
    lv_label_set_text_fmt(qr_label,
                     "LVGL project GitHub website URL\n"
                     " \n"
                     " \n"
                     " Click to close the QR code page");
    GITHUB_qrcode_btn_obj->user_data = menu_box2_qr_bg;

    lv_obj_add_event_cb(Status_Bar, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(menu, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(menu_covered_tiles, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(menu_box2, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_btn_obj, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(GITHUB_qrcode_btn_obj, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_ctrl_menu_play_pause_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_ctrl_playmode_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_ctrl_next_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_ctrl_last_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_minimize_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_close_btn, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(player_ctrl_menu_slider, main_obj_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(menu_box2_qr_bg, main_obj_event_cb, LV_EVENT_ALL, NULL);

    //日期更新定时器（1000ms进入一次回调函数）
    lv_timer_t* timer = lv_timer_create(timeset_timer_cb, 1000, NULL);
    lv_timer_ready(timer);
    timer->user_data = label_time;//应用列表的时间
    label_time->user_data = menu_label_time;//下拉菜单的时间
    menu_label_time->user_data = menu_label_data;//下拉菜单的日期
    //播放器进度条更新定时器（100ms进入一次回调函数）
    lv_timer_t* ffmpeg_video_end_detect_timer = lv_timer_create(video_detect_timer_cb, 100, NULL);
    lv_timer_ready(ffmpeg_video_end_detect_timer);
    ffmpeg_video_end_detect_timer->user_data = player;
    player->user_data = player_ctrl_menu_play_pause_btn_img;
    player->user_data_backup = player_ctrl_menu_slider;
}

static void main_obj_event_cb(lv_event_t * e)
{
    //获取当前活跃的输入设备的指针
    lv_indev_t * dev = lv_indev_get_act();
    //获取输入设备的XY坐标
    lv_point_t point = {0};
    lv_indev_get_point(dev, &point);
    //获取输入设备的输入方向和速度
    lv_point_t vector = {0};
    lv_indev_get_vect(dev, &vector);

    lv_obj_t *obj = lv_event_get_current_target(e);

    switch(e->code)
    {
        case LV_EVENT_PRESSED:
            gpress_x = point.x;
            gpress_y = point.y;
            break;
        case LV_EVENT_RELEASED:
            gmove_x = point.x;
            gmove_y = point.y;
            #define MOVE_OFFSET_X LV_HOR_RES/5
            #define MOVE_OFFSET_Y LV_VER_RES/5
            //根据控件停留的当前位置来判断需要执行哪个动画
            if(obj->num == Status_Bar_num){
                uint16_t menu_y = lv_obj_get_y2(obj->user_data);
                (menu_y<LV_VER_RES/2)?auto_move(obj->user_data,menu_amin_speed_delay,-LV_VER_RES):((menu_y>LV_VER_RES/2)?auto_move(obj->user_data,menu_amin_speed_delay,0):(void)0);
                //(y_speed>1000)?auto_move(obj->user_data,menu_amin_speed_delay,0):((y_speed<-1000)?auto_move(obj->user_data,menu_amin_speed_delay,-LV_VER_RES):(void)0);
            }
            else if(obj->num == menu_num){
                uint16_t menu_y = lv_obj_get_y2(obj);
                (menu_y<LV_VER_RES/2)?(auto_move(obj,menu_amin_speed_delay,-LV_VER_RES)):((menu_y>LV_VER_RES/2)?(auto_move(obj,menu_amin_speed_delay,0)):(void)0);
                //(y_speed>1000)?auto_move(obj,menu_amin_speed_delay,0):((y_speed<-1000)?auto_move(obj,menu_amin_speed_delay,-LV_VER_RES):(void)0);
            }
            else if(obj->num == menu_covered_tiles_num){
                uint16_t menu_y = lv_obj_get_y2(obj);
                (menu_y<500)?auto_move(obj,Covered_tiles_amin_speed_delay,120):((menu_y>500)?auto_move(obj,Covered_tiles_amin_speed_delay,230):(void)0);
                //(y_speed>300)?auto_move(obj,Covered_tiles_amin_speed_delay,230):((y_speed<-300)?auto_move(obj,Covered_tiles_amin_speed_delay,120):(void)0);

                (menu_y<500)?auto_size_set((lv_obj_t *)obj->user_data_backup, Covered_tiles_amin_speed_delay, lv_obj_get_width((lv_obj_t *)obj->user_data_backup),0 , lv_obj_get_height((lv_obj_t *)obj->user_data_backup), 0):((menu_y>500)?auto_size_set((lv_obj_t *)obj->user_data_backup, Covered_tiles_amin_speed_delay,lv_obj_get_width((lv_obj_t *)obj->user_data_backup), LV_HOR_RES*7/28, lv_obj_get_height((lv_obj_t *)obj->user_data_backup), LV_VER_RES*1/13):(void)0);
                //(y_speed>300)?auto_size_set((lv_obj_t *)obj->user_data_backup, Covered_tiles_amin_speed_delay, lv_obj_get_width((lv_obj_t *)obj->user_data_backup), LV_HOR_RES*7/28, lv_obj_get_height((lv_obj_t *)obj->user_data_backup), LV_VER_RES*1/13):((y_speed<-300)?auto_size_set((lv_obj_t *)obj->user_data_backup, Covered_tiles_amin_speed_delay, lv_obj_get_width((lv_obj_t *)obj->user_data_backup),0 , lv_obj_get_height((lv_obj_t *)obj->user_data_backup), 0):(void)0);

                (menu_y<500)?lv_obj_clear_flag((lv_obj_t *)obj->user_data_backup, LV_OBJ_FLAG_CLICKABLE):((menu_y>500)?lv_obj_add_flag((lv_obj_t *)obj->user_data_backup, LV_OBJ_FLAG_CLICKABLE):(void)0);
            }

            //根据释放后行为进行对应选择
            if (gpress_x != gmove_x || gpress_y != gmove_y)
            {
                if (gmove_x > gpress_x)//右滑
                {
                    //如果释放后的坐标大于按下的坐标，且差值大于阈值，并且上下滑动的差值不大，则判断为右滑操作
                    if ((gmove_x - gpress_x) >= MOVE_OFFSET_X && abs(gmove_y - gpress_y) <= MOVE_OFFSET_Y/2){

                    }
                    else{
                        printf("right swipe Fail\r\n");
                    }
                }
                else if(gpress_x > gmove_x)//左滑
                {
                    //如果释放后的坐标小于按下的坐标，且差值大于阈值，并且上下滑动的差值不大，则判断为左滑操作
                    if ((gpress_x - gmove_x) >= MOVE_OFFSET_X && abs(gmove_y - gpress_y) <= MOVE_OFFSET_Y/2){

                    }
                    else{
                        printf("left swipe Fail\r\n");
                    }
                }

                if(gmove_y > gpress_y)//下滑
                {
                    if ((gmove_y - gpress_y) >= MOVE_OFFSET_Y && abs(gmove_x - gpress_x) <= MOVE_OFFSET_X/2)
                    {
                        if(obj->num == Status_Bar_num){
                            lv_obj_t *obj_temp = (lv_obj_t *)obj->user_data;
                            auto_move(obj_temp,menu_amin_speed_delay,0);
                        }
                        else if(obj->num == menu_num){
                            auto_move(obj,menu_amin_speed_delay,0);
                        }
                    }
                    else{
                        printf("down swipe Fail\r\n");
                    }
                }
                else if(gpress_y > gmove_y)//上滑
                {
                    if ((gpress_y - gmove_y) >= MOVE_OFFSET_Y && abs(gmove_x - gpress_x) <= MOVE_OFFSET_X/2)
                    {
                        if(obj->num == Status_Bar_num){
                            lv_obj_t *obj_temp = (lv_obj_t *)obj->user_data;
                            auto_move(obj_temp,menu_amin_speed_delay,-LV_VER_RES);
                        }
                        else if(obj->num == menu_num){
                            auto_move(obj,menu_amin_speed_delay,-LV_VER_RES);
                        }
                    }
                    else{
                        printf("up swipe Fail\r\n");
                    }
                }
            }
            break;
        case LV_EVENT_SHORT_CLICKED:
            switch(obj->num)
            {
                case menu_box2_qr_bg_num:
                    auto_move(obj, 300, 700);
                    break;
                case GITHUB_qrcode_btn_obj_num:
                    auto_move(obj->user_data, 300, 300);
                    break;
                case player_ctrl_menu_play_pause_btn_num:
                    switch(player_run_flag)
                    {
                        case init_state:
                            lv_ffmpeg_player_set_cmd((lv_obj_t *)obj->user_data, LV_FFMPEG_PLAYER_CMD_RESUME);
                            lv_img_set_src((lv_obj_t *)obj->user_data_backup, "C:/IMG/pause.png");
                            player_run_flag = play_state;
                            break;
                        case play_state:
                            lv_ffmpeg_player_set_cmd((lv_obj_t *)obj->user_data, LV_FFMPEG_PLAYER_CMD_PAUSE);
                            lv_img_set_src((lv_obj_t *)obj->user_data_backup, "C:/IMG/play.png");
                            player_run_flag = pause_state;
                            break;
                        case pause_state:
                            lv_ffmpeg_player_set_cmd((lv_obj_t *)obj->user_data, LV_FFMPEG_PLAYER_CMD_RESUME);
                            lv_img_set_src((lv_obj_t *)obj->user_data_backup, "C:/IMG/pause.png");
                            player_run_flag = play_state;
                        default:
                            break;
                    }
                    break;

                case player_ctrl_next_btn_num:
                    player_run_video_num++;
                    player_run_flag = play_state;

                    if (player_run_video_num == 1) {
                        lv_obj_add_flag(((lv_obj_t *)obj->user_data_backup_backup), LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_set_style_bg_color(((lv_obj_t *)obj->user_data_backup_backup), lv_color_make(230, 230, 230), LV_PART_MAIN);
                    }
                    if (player_run_video_num == video_count - 1) {
                        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_set_style_bg_color(obj, lv_color_make(150, 150, 150), LV_PART_MAIN);
                    }
                    play_video((lv_obj_t *)obj->user_data, (lv_obj_t *)obj->user_data_backup, player_run_video_num);
                    break;

                case player_ctrl_last_btn_num:
                    player_run_video_num--;
                    player_run_flag = play_state;

                    if (player_run_video_num == 0) {
                        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_set_style_bg_color(obj, lv_color_make(150, 150, 150), LV_PART_MAIN);
                    }
                    if (player_run_video_num == video_count - 2) {
                        lv_obj_add_flag(((lv_obj_t *)obj->user_data_backup_backup), LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_set_style_bg_color(((lv_obj_t *)obj->user_data_backup_backup), lv_color_make(230, 230, 230), LV_PART_MAIN);
                    }
                    play_video((lv_obj_t *)obj->user_data, (lv_obj_t *)obj->user_data_backup, player_run_video_num);
                    break;

                case player_ctrl_playmode_btn_num:
                    switch(player_run_mode_set_flag)
                    {
                        case repeat_mode:
                            lv_ffmpeg_player_set_auto_restart((lv_obj_t *)obj->user_data, false);
                            lv_img_set_src((lv_obj_t *)obj->user_data_backup, "C:/IMG/unrepeat.png");
                            player_run_mode_set_flag = unrepeat_mode;
                            break;
                        case unrepeat_mode:
                            lv_ffmpeg_player_set_auto_restart((lv_obj_t *)obj->user_data, true);
                            lv_img_set_src((lv_obj_t *)obj->user_data_backup, "C:/IMG/repeat.png");
                            player_run_mode_set_flag = repeat_mode;
                            break;
                        default:
                            break;
                    }
                    break;

                case player_minimize_btn_num:
                    auto_size_set((lv_obj_t *)obj->user_data, 100, LV_HOR_RES*11/18, 0, LV_VER_RES*11/14, 0);
                    break;

                case player_btn_obj_num:
                    auto_size_set((lv_obj_t *)obj->user_data, 100, 0, LV_HOR_RES*11/18, 0, LV_VER_RES*11/14);
                    printf("player_btn");
                    break;

                default:
                    break;
            }
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            long_press++;
            if(long_press > 2)
            {
                long_press = 0;
            }
            break;
        case LV_EVENT_PRESSING:
            if( vector.x != 0 || vector.y !=0)
            {
                lv_coord_t x = lv_obj_get_x(obj) +vector.x;
                lv_coord_t y = lv_obj_get_y(obj)+ vector.y;
                uint32_t curr_time = lv_tick_get(); //获取当前时间
                float dt = (curr_time - last_time) / 1000.0f; // 计算时间间隔(单位:秒)
                x_speed = vector.x / dt; // 计算水平速度（dt时间的平均速度）
                y_speed = vector.y / dt; // 计算垂直速度（dt时间的平均速度）
                last_time = curr_time;
                printf("x = %d,y = %d\r\n",x,y);
                printf("num = %d\r\n",obj->num);
                printf("xspeed = %f,yspeed = %f\r\n",x_speed,y_speed);
                if(obj->num == menu_num)
                {
                   lv_obj_set_pos(obj,(x<0)?0:((x>0)?0:x),(y<0)?y:((y>0)?0:y));
                }
                else if(obj->num == Status_Bar_num)
                {
                    lv_obj_t *obj_temp = (lv_obj_t *)obj->user_data;
                    lv_coord_t x = lv_obj_get_x(obj_temp) +vector.x;
                    lv_coord_t y = lv_obj_get_y(obj_temp)+ vector.y;
                    lv_obj_set_pos(obj_temp,(x<0)?0:((x>0)?0:x),(y<0)?y:((y>0)?0:y));
                }
                else if(obj->num == menu_covered_tiles_num)
                {
                    lv_obj_set_pos(obj,(x<0)?0:((x>0)?0:x),(y<120)?120:((y>230)?230:y));
                    (y<120)?y=120:((y>230)?y=230:(void)0);
                    int temp = (y*3/66<8)?y/66/3+7:y*3/66+8;
                    lv_obj_set_pos((lv_obj_t *)obj->user_data,temp-20,-13);
                    temp = (y*23/66<65)?58:y*23/66;
                    lv_obj_set_pos(((lv_obj_t *)((lv_obj_t *)obj->user_data)->user_data),temp-20,-13);
                    temp = (y*45/66-15<115)?108:y*45/66-15;
                    lv_obj_set_pos(((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)obj->user_data)->user_data)->user_data),temp-20,-13);
                    temp = (y*67/66-30<165)?158:y*67/66-30;
                    lv_obj_set_pos(((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)obj->user_data)->user_data)->user_data)->user_data),temp-20,-13);
                    temp = (y*89/66-45<215)?208:y*89/66-45;
                    lv_obj_set_pos(((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)obj->user_data)->user_data)->user_data)->user_data)->user_data),temp-20,-13);

                    lv_obj_set_size((lv_obj_t *)obj->user_data_backup, (y*256/130-236<=0)?0:y*256/130-194, (y*24/65-44<=0)?0:y*24/65-36);
                }
                else{
                    break;
                }
            }
            break;
        default:
            break;
    }
}

/**< 每种动画最基本的模板 */
static void start_animation(lv_obj_t * obj, lv_coord_t start, lv_coord_t end, uint32_t time, lv_anim_exec_xcb_t exec_cb)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, time);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_start(&a);
}
/**< 用于实现位置变换的动画 */
static void auto_move(lv_obj_t * obj, uint32_t delay, lv_coord_t target)
{
    if(obj->num == Status_Bar_num || obj->num == menu_num || obj->num == menu_covered_tiles_num)
    {
        start_animation(obj, lv_obj_get_y(obj), target, delay, (lv_anim_exec_xcb_t)lv_obj_set_y);
    }

    #define menu_btn_move_speed_delay 300
    if(obj->user_data != NULL && obj->num == menu_covered_tiles_num)
    {
        lv_coord_t targets[5];
        if(target == 120) {
            targets[0] = -4;
            targets[1] = 46;
            targets[2] = 96;
            targets[3] = 146;
            targets[4] = 196;
        } else if(target == 230) {
            targets[0] = target/66-5;
            targets[1] = target*23/66-20;
            targets[2] = target*45/66-35;
            targets[3] = target*67/66-50;
            targets[4] = target*89/66-69;
        }

        lv_obj_t * current_obj = (lv_obj_t *)obj->user_data;
        for(int i = 0; i < 5; ++i) {
            start_animation(current_obj, lv_obj_get_x(current_obj), targets[i], menu_btn_move_speed_delay, (lv_anim_exec_xcb_t)lv_obj_set_x);
            current_obj = (lv_obj_t *)current_obj->user_data;
        }
    }

    if(obj->num == menu_box2_qr_bg_num)
    {
        start_animation(obj, lv_obj_get_x(obj), target, delay, (lv_anim_exec_xcb_t)lv_obj_set_x);
    }
}

/**< 用于实现大小变换的动画 */
static void auto_size_set(lv_obj_t * obj, uint32_t delay,lv_coord_t start_width, lv_coord_t target_width, lv_coord_t start_hight, lv_coord_t target_hight)
{
    if(obj->num == mydev_btn_obj_num)
    {
        start_animation(obj, start_hight, target_hight, delay, (lv_anim_exec_xcb_t)lv_obj_set_height);
        start_animation(obj, start_width, target_width, delay, (lv_anim_exec_xcb_t)lv_obj_set_width);
    }
    else if(obj->num == menu_box2_tabview_num ||obj->num == player_btn_obj_num)
    {
        start_animation(obj, start_hight, target_hight, delay, (lv_anim_exec_xcb_t)lv_obj_set_height);
        start_animation(obj, start_width, target_width, delay, (lv_anim_exec_xcb_t)lv_obj_set_width);
    }
}

/**< 日期更新定时器回调函数 */
static void timeset_timer_cb(lv_timer_t* timer)
{
    time_t c_time = time(NULL);
    struct tm* l_time = localtime(&c_time);

    // 设置时间
    sprintf(str, "%02d:%02d", l_time->tm_hour, l_time->tm_min);
    lv_label_set_text((lv_obj_t *)timer->user_data, str);
    lv_label_set_text((lv_obj_t *)((lv_obj_t *)(timer->user_data))->user_data, str);

    // 设置日期
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    sprintf(str, "%s  %02d", months[l_time->tm_mon], l_time->tm_mday);

    // 添加后缀
    const char* suffix;
    if (l_time->tm_mday == 1 || l_time->tm_mday == 21 || l_time->tm_mday == 31) {
        suffix = "st";
    } else if (l_time->tm_mday == 2 || l_time->tm_mday == 22) {
        suffix = "nd";
    } else if (l_time->tm_mday == 3 || l_time->tm_mday == 23) {
        suffix = "rd";
    } else {
        suffix = "th";
    }
    strcat(str, suffix);

    // 添加星期
    const char* weekdays[] = {"    Sunday", "    Monday", "    Tuesday", "    Wednesday", "    Thursday", "    Friday", "    Saturday"};
    strcat(str, weekdays[l_time->tm_wday]);

    lv_label_set_text((lv_obj_t *)((lv_obj_t *)((lv_obj_t *)(timer->user_data))->user_data)->user_data, str);
}

/**< 播放器进度条更新定时器回调函数 */
static void video_detect_timer_cb(lv_timer_t* timer)
{
    //当前帧获取
    int frame_index = ffmpeg_get_current_frame_index((lv_obj_t *)timer->user_data);
    //总帧数
    int total_frame = lv_ffmpeg_get_frame_num_user_write((lv_obj_t *)timer->user_data);
    //更新进度条
    lv_slider_set_value((lv_obj_t *)((lv_obj_t *)timer->user_data)->user_data_backup,(int)(((float)(frame_index%total_frame)/(float)total_frame)*100), LV_ANIM_OFF);
}
