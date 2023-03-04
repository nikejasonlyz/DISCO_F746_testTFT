// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.1
// LVGL VERSION: 8.3.4
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_background;
lv_obj_t * ui_Button2;
lv_obj_t * ui_Switch2;
lv_obj_t * ui_Bar2;
lv_obj_t * ui_brightSetSlider;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_background = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_background, &ui_img_mario_png);
    lv_obj_set_width(ui_background, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_background, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_background, -2);
    lv_obj_set_y(ui_background, 0);
    lv_obj_set_align(ui_background, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_background, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_background, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button2 = lv_btn_create(ui_Screen1);
    lv_obj_set_width(ui_Button2, 100);
    lv_obj_set_height(ui_Button2, 50);
    lv_obj_set_align(ui_Button2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Switch2 = lv_switch_create(ui_Screen1);
    lv_obj_set_width(ui_Switch2, 50);
    lv_obj_set_height(ui_Switch2, 25);
    lv_obj_set_x(ui_Switch2, -61);
    lv_obj_set_y(ui_Switch2, -78);
    lv_obj_set_align(ui_Switch2, LV_ALIGN_CENTER);

    ui_Bar2 = lv_bar_create(ui_Screen1);
    lv_bar_set_value(ui_Bar2, 25, LV_ANIM_OFF);
    lv_obj_set_width(ui_Bar2, 150);
    lv_obj_set_height(ui_Bar2, 10);
    lv_obj_set_x(ui_Bar2, -2);
    lv_obj_set_y(ui_Bar2, 50);
    lv_obj_set_align(ui_Bar2, LV_ALIGN_CENTER);

    ui_brightSetSlider = lv_slider_create(ui_Screen1);
    lv_slider_set_range(ui_brightSetSlider, 0, 255);
    lv_slider_set_value(ui_brightSetSlider, 30, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_brightSetSlider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_brightSetSlider, 0,
                                                                                                    LV_ANIM_OFF);
    lv_obj_set_width(ui_brightSetSlider, 150);
    lv_obj_set_height(ui_brightSetSlider, 10);
    lv_obj_set_x(ui_brightSetSlider, 1);
    lv_obj_set_y(ui_brightSetSlider, 86);
    lv_obj_set_align(ui_brightSetSlider, LV_ALIGN_CENTER);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}
