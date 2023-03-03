/*** 
 * @Author: nikejasonlyz
 * @Date: 2023-03-03 02:50:14
 * @LastEditors: nikejasonlyz
 * @LastEditTime: 2023-03-04 04:50:14
 * @FilePath: \DISCO_F746_testTFT\lib\LTDC\src\LTDC_TFT.h
 * @Description: LTDC_TFT.h
 * @
 * @Copyright (c) 2023 by nikejasonlyz, All Rights Reserved. 
 */

#ifndef LTDC_TFT_H
#define LTDC_TFT_H

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "misc/lv_color.h"
#include "misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/
// #define TFT_HOR_RES 480
// #define TFT_VER_RES 272
#define LV_HOR_RES_MAX 480
#define LV_VER_RES_MAX 272

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void tft_init(void);
void tft_on(void);
void tft_off(void);

/**********************
 *      MACROS
 **********************/

#endif
