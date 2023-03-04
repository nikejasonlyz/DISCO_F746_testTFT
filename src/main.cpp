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
#include <lv_conf.h>

#include <yjTask.hpp>
TaskLed tBlink1(LED_BUILTIN,350);     //设引脚LED,num为LED灯的闪烁周期

#include "LTDC_TFT.h"                 // TFT
#include "TouchScreen_F7_Discovery.h" // TOUCH

TouchScreen         ts;   /* TouchScreen instance */

/* LVGL */
#include <lvgl.h>
#include <demos/lv_demos.h>

#if LV_USE_LOG != 0
/* Serial debugging */
void pig_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

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

    // Serial.print("Data x:");Serial.println(p.x);
    // Serial.print("Data y:");Serial.println(p.y);
    Serial.print("positionData:");Serial.print(p.x);
    Serial.print(",");Serial.println(p.y);
  }
  else {
    data->state = LV_INDEV_STATE_REL;
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
  
  tft_init();

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register( &indev_drv );

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

}

// #define CURSOR_SIZE 100
// TSPoint OldPoint;

// String comdata="";
// void ReceiveSerialMessage() {
//   comdata="";
//   while (Serial.available() > 0 )
//   {
//     comdata+=char(Serial.read());
//     delay(2);
//   }
// }

void loop()
{
  // ReceiveSerialMessage();
  // if (comdata.length() > 0) {
  //   Serial.println(comdata);
  //   if (comdata.compareTo("on")>0) {
  //     Serial.println("LCD on!");
  //     tft.LCD_DisplayOn();
  //   } else if (comdata.compareTo("off")>0) {
  //     Serial.println("LCD off!");
  //     tft.LCD_DisplayOff();
  //   }
  // }
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