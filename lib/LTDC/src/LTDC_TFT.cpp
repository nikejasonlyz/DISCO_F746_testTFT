/*** 
 * @Author: nikejasonlyz
 * @Date: 2023-03-03 01:51:38
 * @LastEditors: nikejasonlyz
 * @LastEditTime: 2023-03-04 04:00:15
 * @FilePath: \DISCO_F746_testTFT\lib\LTDC\src\LTDC_TFT.cpp
 * @Description: Interfaces for LVGL support
 * @
 * @Copyright (c) 2023 by nikejasonlyz, All Rights Reserved. 
 */

#include "lvgl.h"
#include "LTDC_TFT.h"
#include "LTDC_F746_Discovery.h"

// static void LCD_DisplayOn(void) {
//     /* Display On */
//     /* Assert LCD_DISP pin */
//     __HAL_LTDC_ENABLE(&hLtdcHandler);
//     pinMode(PI12, OUTPUT);
//     digitalWrite(PI12, HIGH);
//     /* Assert LCD_BL_CTRL pin */
//     pinMode(PK3, OUTPUT);
//     digitalWrite(PK3, HIGH);
// }
/**********************
 *  STATIC PROTOTYPES
 **********************/

/*These 3 functions are needed by LittlevGL*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p);
static void ex_disp_clean_dcache(lv_disp_drv_t *drv);

static uint8_t LCD_Init(void);
static void LCD_LayerRgb565Init(uint32_t FB_Address);
static void LCD_DisplayOn(void);

static void DMA_Config(void);
static void DMA_TransferComplete(DMA_HandleTypeDef *han);
static void DMA_TransferError(DMA_HandleTypeDef *han);

/**********************
 *  STATIC VARIABLES
 **********************/
// static LTDC_HandleTypeDef  hLtdcHandler;
static lv_disp_drv_t disp_drv;

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif

static uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height * sizeof(uint16_t));

static DMA_HandleTypeDef  DmaHandle;
static int32_t            x1_flush;
static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;

static lv_disp_t *our_disp = NULL;

static LTDC_F746_Discovery tft;  /* TFT instance */

/**********************
 *      MACROS
 **********************/

/**
 * Initialize your display here
 */
void tft_init(void)
{
	/* There is only one display on STM32 */
	if(our_disp != NULL)
		abort();
    /* LCD Initialization */
    tft.begin((uint16_t *)buffer);

    /* Enable the LCD */
    tft.LCD_DisplayOn();

    DMA_Config();

    /*-----------------------------
	*  Create a buffer for drawing
	*----------------------------*/

    /* LittlevGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row*/
	static lv_disp_draw_buf_t disp_buf_1;
	static lv_color_t buf1_1[LV_HOR_RES_MAX * 68];
	static lv_color_t buf1_2[LV_HOR_RES_MAX * 68];
	lv_disp_draw_buf_init(&disp_buf_1, buf1_1, buf1_2, LV_HOR_RES_MAX * 68);   /*Initialize the display buffer*/

	/*-----------------------------------
	* Register the display in LittlevGL
	*----------------------------------*/

	lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	disp_drv.hor_res = 480;
	disp_drv.ver_res = 272;

	/*Used to copy the buffer's content to the display*/
	disp_drv.flush_cb = ex_disp_flush;
	disp_drv.clean_dcache_cb = ex_disp_clean_dcache;

	/*Set a display buffer*/
	disp_drv.draw_buf = &disp_buf_1;


	/*Finally register the driver*/
	our_disp = lv_disp_drv_register(&disp_drv);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p)
{
	int32_t x1 = area->x1;
	int32_t x2 = area->x2;
	int32_t y1 = area->y1;
	int32_t y2 = area->y2;
    /*Return if the area is out the screen*/

    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > LV_HOR_RES_MAX - 1) return;
    if(y1 > LV_VER_RES_MAX - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES_MAX - 1 ? LV_HOR_RES_MAX - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES_MAX - 1 ? LV_VER_RES_MAX - 1 : y2;

    x1_flush = act_x1;
    y1_flush = act_y1;
    x2_flush = act_x2;
    y2_fill = act_y2;
    y_fill_act = act_y1;
    buf_to_flush = color_p;

	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
    /*##-7- Start the DMA transfer using the interrupt mode #*/
    /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
    /* Enable All the DMA interrupts */
    HAL_StatusTypeDef err;
    uint32_t length = (x2_flush - x1_flush + 1);
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
    err = HAL_DMA_Start_IT(&DmaHandle,(uint32_t)buf_to_flush, (uint32_t)&buffer[y_fill_act * LV_HOR_RES_MAX + x1_flush],
             length);
    if(err != HAL_OK)
    {
        while(1);	/*Halt on error*/
    }
}

static void ex_disp_clean_dcache(lv_disp_drv_t *drv)
{
    SCB_CleanInvalidateDCache();
}


static void DMA_Config(void)
{
    /*## -1- Enable DMA2 clock #################################################*/
    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-2- Select the DMA functional Parameters ###############################*/
    DmaHandle.Init.Channel = CPY_BUF_DMA_CHANNEL;                   /* DMA_CHANNEL_0                    */
    DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;                /* M2M transfer mode                */
    DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;                     /* Peripheral increment mode Enable */
    DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                        /* Memory increment mode Enable     */
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Peripheral data alignment : 16bit */
    DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;      /* memory data alignment : 16bit     */
    DmaHandle.Init.Mode = DMA_NORMAL;                               /* Normal DMA mode                  */
    DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;                    /* priority level : high            */
    DmaHandle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;                  /* FIFO mode enabled                */
    DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL; /* FIFO threshold: 1/4 full   */
    DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;                    /* Memory burst                     */
    DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;                 /* Peripheral burst                 */

    /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
    DmaHandle.Instance = CPY_BUF_DMA_STREAM;

    /*##-4- Initialize the DMA stream ##########################################*/
    if(HAL_DMA_Init(&DmaHandle) != HAL_OK)
    {
        while(1)
        {
        }
    }

    /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    HAL_NVIC_SetPriority(CPY_BUF_DMA_STREAM_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(CPY_BUF_DMA_STREAM_IRQ);
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef *han)
{
    y_fill_act ++;

    if(y_fill_act > y2_fill) {
    	SCB_CleanInvalidateDCache();
    	SCB_InvalidateICache();
        lv_disp_flush_ready(&disp_drv);
    } else {
    	uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += x2_flush - x1_flush + 1;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        if(HAL_DMA_Start_IT(han,(uint32_t)buf_to_flush, (uint32_t)&buffer[y_fill_act * 480 + x1_flush],
                            length) != HAL_OK)
        {
            while(1);	/*Halt on error*/
        }
    }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void DMA_TransferError(DMA_HandleTypeDef *han)
{

}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void CPY_BUF_DMA_STREAM_IRQHANDLER(void)
{
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(&DmaHandle);
}
