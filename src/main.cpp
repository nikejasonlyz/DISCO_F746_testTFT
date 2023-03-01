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
// #include "stm32f746xx.h" // Provide specific declarations for LTDC, TFT, etc
#include <Arduino.h>
#include <yjTask.hpp>
/*
   Touch example for STM32F476 Discvery
   using Adafruit compatible API
   June 2017, ChrisMicro
*/
#include "LTDC_F746_Discovery.h" // TFT
#include "TouchScreen_F7_Discovery.h" // TOUCH

TaskLed tBlink1(LED_BUILTIN,350);//设引脚LED,num为LED灯的闪烁周期

/* inherented from Adafruit */
LTDC_F746_Discovery tft;  /* TFT instance */
TouchScreen         ts;   /* TouchScreen instance */

/* 参考作用 */
/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 272;

/* LVGL */
#include <lvgl.h>
#include <demos/lv_demos.h>
static lv_disp_draw_buf_t draw_buf;   // Draw buffer of lv_demo
static lv_color_t buf[ screenWidth * 10 ];

/* TFT_eSPI */
// #include <TFT_eSPI.h>
// TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void pig_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void pig_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
  int32_t x;
  int32_t y;
  for(y = area->y1; y <= area->y2; y++) {
      for(x = area->x1; x <= area->x2; x++) {
          /*Put a pixel to the display. For example:*/
          /*put_px(x, y, *color_p)*/
          color_p++;
      }
  }
  /*IMPORTANT!!!
    *Inform the graphics library that you are ready with the flushing*/
  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void pig_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;
    touchX = ts.readTouchX();
    touchY = ts.readTouchY();
    bool touched = (ts.getPoint().z != 0);

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

void setup()
{
  Serial.begin( 115200 ); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb( pig_print ); /* register print function for debugging */
#endif

  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height * sizeof(uint16_t));
  
  tft.begin((uint16_t *)buffer);
  // tft.setRotation( 3 ); /* Landscape orientation, flipped */
  // tft.fillScreen(LTDC_BLACK);

  /*Set the touchscreen calibration data,
    the actual data for your display can be acquired using
    the Generic -> Touch_calibrate example from the TFT_eSPI library*/
  // uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  // tft.setTouch( calData );

  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );
  //tft.setRotation(0);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = pig_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = pig_touchpad_read;
  lv_indev_drv_register( &indev_drv );

#if 1
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
  lv_demo_widgets();            // OK
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
