/*
 * @description: 
 * @param: 
 * @return: 
 * @author: Universe
 */
//LTDC for F746 Discovery board

#ifndef LTDC_F7_H
#define LTDC_F7_H

#include "LTDC.h"

// The display on STM32F746NG Discovery
constexpr LTDCSettings LTDC_F746_ROKOTECH = {
    .width = 480,
    .height = 272,
    .horizontalSync = 41,
    .horizontalFrontPorch = 32,
    .horizontalBackPorch = 13,
    .verticalSync = 10,
    .verticalFrontPorch = 2,
    .verticalBackPorch = 10,
};

/*********************
 *      DEFINES
 *********************/
#if LV_COLOR_DEPTH != 16 && LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
#error LV_COLOR_DEPTH must be 16, 24, or 32
#endif
/**
  * @brief LCD special pins
  */
/* Display enable pin */
#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()
/* Backlight control pin */
#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()
#define CPY_BUF_DMA_STREAM               DMA2_Stream0
#define CPY_BUF_DMA_CHANNEL              DMA_CHANNEL_0
#define CPY_BUF_DMA_STREAM_IRQ           DMA2_Stream0_IRQn
#define CPY_BUF_DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler
/*********************
 *        END
 *********************/

/* Test */
#define DMAtest 0
#if DMAtest == 1
#include "lvgl.h"
static lv_disp_drv_t disp_drv;
static uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height * sizeof(uint16_t));

static DMA_HandleTypeDef  DmaHandle;
static int32_t            x1_flush;
static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;

// static lv_disp_t *our_disp = NULL;

// static LTDC_F746_Discovery tft;  /* TFT instance */
#endif

class LTDC_F746_Discovery : public LTDCClass<480, 272> {
public:
    LTDC_F746_Discovery(): LTDCClass(LTDC_F746_ROKOTECH) {

    };

    /* Configure LCD pins, and peripheral clocks. */
    void LCD_MspInit() {
        GPIO_InitTypeDef gpio_init_structure;

        /* Enable the LTDC and DMA2D clocks */
        __HAL_RCC_LTDC_CLK_ENABLE();
        // __HAL_RCC_DMA2D_CLK_ENABLE();   /* TODO:DMA部分没有特殊处理。故开启与否似乎无影响 */
        /* Enable GPIOs clock */
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();   /* LCD_DISP_GPIO_CLK_ENABLE */
        __HAL_RCC_GPIOJ_CLK_ENABLE();
        __HAL_RCC_GPIOK_CLK_ENABLE();   /* LCD_BL_CTRL_GPIO_CLK_ENABLE */
        /* BUG:重复的原因？只是便于阅读？ */
        LCD_DISP_GPIO_CLK_ENABLE();
        LCD_BL_CTRL_GPIO_CLK_ENABLE();

        /*** LTDC Pins configuration ***/
        /* GPIOE configuration */
        gpio_init_structure.Pin       = GPIO_PIN_4;
        gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
        gpio_init_structure.Pull      = GPIO_NOPULL;
        gpio_init_structure.Speed     = GPIO_SPEED_FAST;
        gpio_init_structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOE, &gpio_init_structure);
        /* GPIOG configuration */
        gpio_init_structure.Pin       = GPIO_PIN_12;
        gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
        gpio_init_structure.Alternate = GPIO_AF9_LTDC;
        HAL_GPIO_Init(GPIOG, &gpio_init_structure);
        /* GPIOI LTDC alternate configuration */
        gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 | \
                                      GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
        gpio_init_structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOI, &gpio_init_structure);
        /* GPIOJ configuration */
        gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                      GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                      GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                      GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
        gpio_init_structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOJ, &gpio_init_structure);
        /* GPIOK configuration */
        gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                      GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
        gpio_init_structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOK, &gpio_init_structure);

        /* BUG:再次重复 */
        /* LCD_DISP GPIO configuration */
        gpio_init_structure.Pin       = LCD_DISP_PIN;     /* LCD_DISP pin has to be manually controlled */
        gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);
        /* LCD_BL_CTRL GPIO configuration */
        gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;  /* LCD_BL_CTRL pin has to be manually controlled */
        gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
    }

    /*--------------------------------------
    * Configure LTDC PLL.
    *-------------------------------------*/
    void LCD_ClockConfig() {
        static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

        /* RK043FN48H LCD clock configuration */
        /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
        /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
        /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
        /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
        periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
        periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
        periph_clk_init_struct.PLLSAI.PLLSAIR = 5;
        periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
        HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
    }

    void LCD_DisplayOn() {
        /* Display On */
        /* Assert LCD_DISP pin */
        __HAL_LTDC_ENABLE(&hLtdcHandler);
        pinMode(PI12, OUTPUT);
        digitalWrite(PI12, HIGH);
        /* Assert LCD_BL_CTRL pin */
        pinMode(PK3, OUTPUT);
        digitalWrite(PK3, HIGH);
    }
    /* TODO:seems need to optimize */
    void LCD_DisplayOff() {
        /* Display Off */
        /* Assert LCD_DISP pin */
        pinMode(PI12, OUTPUT);
        digitalWrite(PI12, LOW);
        /* Assert LCD_BL_CTRL pin */
        pinMode(PK3, OUTPUT);
        digitalWrite(PK3, LOW);
    }

    // void DMA_Config() {
    //     /*## -1- Enable DMA2 clock #################################################*/
    //     __HAL_RCC_DMA2_CLK_ENABLE();

    //     /*##-2- Select the DMA functional Parameters ###############################*/
    //     DmaHandle.Init.Channel = CPY_BUF_DMA_CHANNEL;                   /* DMA_CHANNEL_0                    */
    //     DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;                /* M2M transfer mode                */
    //     DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;                     /* Peripheral increment mode Enable */
    //     DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                        /* Memory increment mode Enable     */
    //     DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Peripheral data alignment : 16bit */
    //     DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;      /* memory data alignment : 16bit     */
    //     DmaHandle.Init.Mode = DMA_NORMAL;                               /* Normal DMA mode                  */
    //     DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;                    /* priority level : high            */
    //     DmaHandle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;                  /* FIFO mode enabled                */
    //     DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL; /* FIFO threshold: 1/4 full   */
    //     DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;                    /* Memory burst                     */
    //     DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;                 /* Peripheral burst                 */

    //     /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
    //     DmaHandle.Instance = CPY_BUF_DMA_STREAM;

    //     /*##-4- Initialize the DMA stream ##########################################*/
    //     if(HAL_DMA_Init(&DmaHandle) != HAL_OK)
    //     {
    //         while(1)
    //         {
    //         }
    //     }

    //     /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    //     HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, LTDC_F746_Discovery::DMA_TransferComplete);
    //     HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, LTDC_F746_Discovery::DMA_TransferError);

    //     /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    //     HAL_NVIC_SetPriority(CPY_BUF_DMA_STREAM_IRQ, 0, 0);
    //     HAL_NVIC_EnableIRQ(CPY_BUF_DMA_STREAM_IRQ);
    // }

    // /**
    //  * @brief  DMA conversion complete callback
    //  * @note   This function is executed when the transfer complete interrupt
    //  *         is generated
    //  * @retval None
    //  */
    // void DMA_TransferComplete(DMA_HandleTypeDef *han)
    // {
    //     y_fill_act ++;

    //     if(y_fill_act > y2_fill) {
    //         SCB_CleanInvalidateDCache();
    //         SCB_InvalidateICache();
    //         lv_disp_flush_ready(&disp_drv);
    //     } else {
    //         uint32_t length = (x2_flush - x1_flush + 1);
    //         buf_to_flush += x2_flush - x1_flush + 1;
    //         /*##-7- Start the DMA transfer using the interrupt mode ####################*/
    //         /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
    //         /* Enable All the DMA interrupts */
    // #if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    //         length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
    // #endif
    //         if(HAL_DMA_Start_IT(han,(uint32_t)buf_to_flush, (uint32_t)&buffer[y_fill_act * 480 + x1_flush],
    //                             length) != HAL_OK)
    //         {
    //             while(1);	/*Halt on error*/
    //         }
    //     }
    // }

    // /**
    //  * @brief  DMA conversion error callback
    //  * @note   This function is executed when the transfer error interrupt
    //  *         is generated during DMA transfer
    //  * @retval None
    //  */
    // void DMA_TransferError(DMA_HandleTypeDef *han)
    // {

    // }

    // /**
    //  * @brief  This function handles DMA Stream interrupt request.
    //  * @param  None
    //  * @retval None
    //  */
    // void CPY_BUF_DMA_STREAM_IRQHANDLER()
    // {
    //     // static DMA_HandleTypeDef  DmaHandle; /* TODO:测试DMA */
    //     /* Check the interrupt and clear flag */
    //     HAL_DMA_IRQHandler(&DmaHandle);
    // }
};

#endif
