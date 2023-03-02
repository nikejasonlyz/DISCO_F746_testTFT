/*
 * @description: 
 * @param: 
 * @return: 
 * @author: Universe
 */
/************************************************************************
  STM32F746 Discovery TFT example template
  Required libraries: GFX, Adafruit
  Installation Instructions:
  1. Import the Adadruit library in the menue
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit ILI9341
  2. Import the Adadruit GFX-Library
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit GFX Library
  June 2017, ChrisMicro
************************************************************************/
/* 
 * Using Adafruit GFX-Library and LVGL
 *
 * March 2023, Nikejasonlyz
 */

#include <Arduino.h>
#include <lvgl.h>       /* LVGL */
#include <yjTask.hpp>
// #include "Adafruit_ILI9341.h"

#include <LTDC_F746_Discovery.h>      /* TFT */
#include <TouchScreen_F7_Discovery.h> /* TOUCH */

TaskLed tBlink1(LED_BUILTIN,350);     /* 设引脚LED,num为LED灯的闪烁周期 */

// inherented from Adafruit
LTDC_F746_Discovery tft;  /* TFT instance */
TouchScreen         ts;   /* TouchScreen instance */

// 参考作用
/*Change to your screen resolution*/
static const uint16_t LV_HOR_RES_MAX  = 480;
static const uint16_t LV_VER_RES_MAX = 272;

/* 
 * 变量“oldPoint”稍后将用于保存旧的接触点值。
 * 变量“disp_buf”和“buf”是LVGL使用的缓冲区。
 * 此外，“disp_drv”和“indev_drv”用于定义LCD和输入功能。
 * 最后，所有“lv_obj_t”变量都是指向使用的屏幕对象的指针，如按钮、标签或屏幕本身。
 */
TSPoint oldPoint;

static lv_disp_draw_buf_t disp_buf;   // Draw buffer of lv_demo
static lv_color_t buf1_1[LV_HOR_RES_MAX * 68];
static lv_color_t buf1_2[LV_HOR_RES_MAX * 68];
lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv;

lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *screenMain;
lv_obj_t *label;
/* End */

#if LV_USE_LOG != 0
/* Serial debugging */
void pig_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

// The buffer is memory mapped
// You can directly draw on the display by writing to the buffer
// uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height * sizeof(uint16_t));
uint32_t *buffer = (uint32_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height * sizeof(uint32_t));

void tft_init(void);
void ex_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void ex_disp_clean_dcache(lv_disp_drv_t *drv);
void ts_init(void);
void touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();
  
#if LV_USE_LOG != 0
  lv_log_register_print_cb( pig_print ); /* register print function for debugging */
#endif

  tft_init();

  ts_init();

#if 0
  /* Create simple label */
  lv_obj_t *label = lv_label_create( lv_scr_act() );
  lv_label_set_text( label, LVGL_Arduino.c_str() );
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
#else
  /* Try an example from the lv_examples Arduino library
      make sure to include it as written above.
  lv_example_btn_1();
  */

  // uncomment one of these demos
  // lv_demo_widgets();            // OK
  // lv_demo_benchmark();          // OK
  // lv_demo_keypad_encoder();     // works, but I haven't an encoder
  // lv_demo_music();              // NOK
  // lv_demo_printer();
  // lv_demo_stress();             // seems to be OK
#endif

  Serial.println( "Setup done" );

  // tft.setCursor(0, 0);
  // tft.setTextColor(LTDC_BLUE);  tft.setTextSize(3);
  // tft.println("STM32F746 Discovery Touch");
}

// #define CURSOR_SIZE 100
// TSPoint OldPoint;

void loop()
{
  // TSPoint p = ts.getPoint();

  // if( OldPoint != p )
  // {
  //   OldPoint=p;
  //   tft.fillScreen( LTDC_BLACK );
  //   tft.setCursor(0, 0);
  //   tft.print(p.x); tft.print(" , "); tft.print(p.y);
  //   tft.print(" pressure: "); tft.println( p.z );
  //   if ( p.z )
  //   {
  //     tft.fillRect(p.x - CURSOR_SIZE/2, p.y - CURSOR_SIZE/2, CURSOR_SIZE, CURSOR_SIZE, LTDC_GREEN);
  //   }
  //   delay(10);
  // }

  tBlink1.poll();

  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}


void tft_init(void) {
  tft.begin((uint32_t *)buffer);
  tft.setRotation(0); /* Landscape orientation, flipped */
  // tft.fillScreen(LTDC_BLUE);

  /* Create a buffer for drawing */
  lv_disp_draw_buf_init(&disp_buf, buf1_1, buf1_2, LV_HOR_RES_MAX * 68);

  /*---------------------------------
  *Initialize the display，显示驱动注册
  *--------------------------------*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = LV_HOR_RES_MAX;
  disp_drv.ver_res = LV_VER_RES_MAX;
  /*Used to copy the buffer's content to the display*/
  disp_drv.flush_cb = ex_disp_flush;
  disp_drv.clean_dcache_cb = ex_disp_clean_dcache;

  /*Set a display buffer*/
  disp_drv.draw_buf = &disp_buf;
  
  // disp_drv.clean_dcache_cb = ex_disp_clean_dcache;
	/*Set a display buffer*/
	disp_drv.draw_buf = &disp_buf;
  lv_disp_drv_register(&disp_drv);
}

/*Initialize the (dummy) input device driver，触摸输入驱动注册*/
void ts_init(void) {
  // static lv_indev_drv_t indev_drv;
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);
}

/* Display flushing，定义显示器刷新缓存 */
void ex_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  // FIXME
  /* 调用区域填充颜色函数 */
  // tft.startWrite();
  // tft.writePixel(x1, y1, &color_p->full);
  // tft.endWrite();

  /*IMPORTANT!!!
    *Inform the graphics library that you are ready with the flushing*/
  lv_disp_flush_ready(disp);
}

void ex_disp_clean_dcache(lv_disp_drv_t *drv)
{
    SCB_CleanInvalidateDCache();
}

/*Read the touchpad*/
void touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  /* To Be Optimized */
  TSPoint p = ts.getPoint();

  if (p.z != 0) {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = p.x;
    data->point.y = p.y;

    Serial.print("Data x ");Serial.println(p.x);
    Serial.print("Data y ");Serial.println(p.y);
  }
  else {
    data->state = LV_INDEV_STATE_REL;
  }
}