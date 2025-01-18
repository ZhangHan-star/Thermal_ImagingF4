/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ST7789.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include <stdio.h>
#include "tim.h"
#include "gui_guider.h"
#include "custom.h"
#include "usbd_cdc_if.h"

#include "arm_math.h"
#include "interpolation_functions.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define abs(x)      ((x)>0?(x):-(x))
void GrayToPseColor(uint8_t grayValue, uint8_t* colorR, uint8_t* colorG, uint8_t* colorB)
{
  if (colorR == NULL || colorG == NULL || colorB == NULL)
  {
    ST7789_WriteString(0, 0, "Error:", Font_16x26, RED,BLACK );
    return;
  }

  // float grayValue;
  // grayValue = grayValue1;
  if ((grayValue >= 0) && (grayValue <= 63))
  {
    *colorR = 0;
    *colorG = 0;
    *colorB = (uint8_t)((float)grayValue / 64.0f * 31.0f);
  }
  else if ((grayValue >= 64) && (grayValue <= 127))
  {
    *colorR = 0;
    *colorG = (uint8_t)((float)(grayValue - 64) / 64.0f * 63.0f);
    *colorB = (uint8_t)((float)(127 - grayValue) / 64.0f * 31.0f);
  }
  else if ((grayValue >= 128) && (grayValue <= 191))
  {
    *colorR = (uint8_t)((float)(grayValue - 128) / 64.0f * 31.0f);
    *colorG = 255;
    *colorB = 0;
  }
  else if ((grayValue >= 192) && (grayValue <= 255))
  {
    *colorR = 127;
    *colorG = (uint8_t)((float)(255 - grayValue) / 64.0f * 63.0f);
    *colorB = 0;
  }
}

static int32_t float_to_1220(float value);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Lvgl_TsakHandle;
osThreadId ReadMLX_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartLvglTask(void const * argument);
void StartReadMLX_Task(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Lvgl_Tsak */
  osThreadDef(Lvgl_Tsak, StartLvglTask, osPriorityNormal, 0, 1024);
  Lvgl_TsakHandle = osThreadCreate(osThread(Lvgl_Tsak), NULL);

  /* definition and creation of ReadMLX_Task */
  osThreadDef(ReadMLX_Task, StartReadMLX_Task, osPriorityIdle, 0, 2048);
  ReadMLX_TaskHandle = osThreadCreate(osThread(ReadMLX_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartLvglTask */
lv_ui guider_ui;
/**
* @brief Function implementing the Lvgl_Tsak thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLvglTask */
void StartLvglTask(void const * argument)
{
  /* USER CODE BEGIN StartLvglTask */
  // lv_init();
	// lv_port_disp_init();

  // setup_ui(&guider_ui);
  // HAL_TIM_Base_Start_IT(&htim12);
  /* Infinite loop */
  for(;;)
  {
    // lv_task_handler();
    osDelay(1);
  }
  /* USER CODE END StartLvglTask */
}

/* USER CODE BEGIN Header_StartReadMLX_Task */
unsigned short EE[832];
unsigned short Frame[834];
paramsMLX90640 MLXPars;
float Vdd,Ta,Tr;
float Temp[768];//原始温度

//共用体，用于色彩转换
union color {
  uint16_t raw;
  struct
  {
    uint16_t b:5;
    uint16_t g:6;
    uint16_t r:5;
  };
};

float MaxTemp = 0;
float MinTemp = 0;
uint8_t return1,return2;

uint8_t flag0=0,flag1=0,flagtime=0;
//浮点插值用
// float32_t temp_raw_buffer[32*24]={0};//存放原始数据,
// float32_t temp_interp_buffer[320*30]={0};//存放插值后数据
// arm_bilinear_interp_instance_f32 bilinearInterp;
// uint8_t TemptNorma[320*30];
// static union color color_temp;
// static uint16_t Picter[320*30];
// uint8_t R,G,B;

int16_t temp_raw_buffer[32*24]={0};
int16_t temp_interp_buffer[320*30]={0};//存放插值后数据
arm_bilinear_interp_instance_q15 bilinearInterp;
union color color_temp;//用于色彩转换的共用体
uint8_t R,G,B;//暂存RGB数据


/**
* @brief Function implementing the ReadMLX_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReadMLX_Task */
void StartReadMLX_Task(void const * argument)
{
  /* USER CODE BEGIN StartReadMLX_Task */
  osDelay(1000); //预留一点时间让 MLX 传感器完成自己的初始化
  MLX90640_SetRefreshRate(0x33,3); //测量速率 1Hz(0~7 对应 0.5,1,2,4,8,16,32,64Hz)
  MLX90640_DumpEE(0x33, EE);//读取像素校正参数
  MLX90640_ExtractParameters(EE, &MLXPars); //解析校正参数（计算温度时需要）
  osDelay(100);
  /* Infinite loop */
  for(;;)
  {
    uint32_t test=0;
    test=float_to_1220(2.5f);

    char str[50];
    return1=0;
    return1 = MLX90640_GetFrameData(0x33, Frame);
    if (return1==0 || return1 ==1) //有转换完成的帧
    {
      Vdd=MLX90640_GetVdd(Frame, &MLXPars); //计算 Vdd（这句可有可无）
      Ta=MLX90640_GetTa( Frame, &MLXPars); //计算实时外壳温度
      Tr=Ta-8.0; //计算环境温度用于温度补偿
      //手册上说的环境温度可以用外壳温度-8℃
      MLX90640_CalculateTo(Frame, &MLXPars, 0.95, Tr, Temp); //计算像素点温度
      // MLX90640_GetImage(Frame, &MLXPars, Temp);
      /*
      Temp 数组内即是转换完成的实时温度值，单位℃
      可以在这里对得到的 32*24=768 个温度值进行处理、转换为颜色值、显示
      关于温度转颜色方法，在后续的文章中会有专门介绍
      */
      //计算最大最小温度
      MaxTemp=0;MinTemp=1000;
      for (uint16_t i = 0; i < 768; i++)
      {
        if (MaxTemp < Temp[i])
        {
          MaxTemp = Temp[i];
        }
        if (MinTemp > Temp[i])
        {
          MinTemp = Temp[i];
        }
      }
      sprintf(str, "MAX:%1.f,MIN:%1.f",MaxTemp,MinTemp);
      ST7789_WriteString(0,220,str,Font_11x18,WHITE,BLACK);

      //上下翻转图像
      // for (uint8_t h = 0; h < 24; h++)
      // {
      //   for (uint8_t w = 0; w < 32; w++)
      //   {
      //     temp_raw_buffer[(23-h)*32+w] = Temp[h*32+w];
      //   }
      // }
      for (uint8_t h = 0; h < 24; h++)
      {
        for (uint8_t w = 0; w < 32; w++)
        {
          temp_raw_buffer[(23-h)*32+w] = (Temp[h*32+w]* 100);
        }
      }
      /**********************int16_t插值************************************************************************** */
      bilinearInterp.numCols = 32;
      bilinearInterp.numRows = 24;
      bilinearInterp.pData = temp_raw_buffer;

      for (uint16_t num = 0; num < 8; num++)
      {
        //插值计算
        for (uint16_t h = 0; h < 30; h++)
        {
          for (uint16_t w = 0; w < 320; w++)
          {
            temp_interp_buffer[h*320 + w] = arm_bilinear_interp_q15(&bilinearInterp, float_to_1220((float32_t)w / 10.29032258064516f), float_to_1220((float32_t)(h+num*30) / 10.39130434782609f));
          }
        }
        //归一化
        for (uint16_t h = 0; h < 30; h++)
        {
          for (uint16_t w = 0; w < 320; w++)
          {
            temp_interp_buffer[h*320 + w] = (int16_t)(((float)temp_interp_buffer[h*320 + w]/100.0f - MinTemp) / (MaxTemp - MinTemp) * 255);
          }
        }

        for (uint16_t h = 0; h < 30; h++)
        {
          for (uint16_t w = 0; w < 320; w++)
          {
            GrayToPseColor((uint8_t)temp_interp_buffer[h*320 + w], &R,&G,&B);
            color_temp.r = R;
            color_temp.g = G;
            color_temp.b = B;
            temp_interp_buffer[h*320 + w] = color_temp.raw;
          }
        }
        //大小端转换
        for (uint16_t i = 0; i < (320*30); i++)
        {
          temp_interp_buffer[i] = (temp_interp_buffer[i]<<8 | temp_interp_buffer[i]>>8);
        }
        ST7789_DrawImage(0, num*30, 320, 30, (uint16_t *)temp_interp_buffer);
      }
      /*********************************************************************************************************** */
      /**********************float插值************************************************************************** *
      bilinearInterp.numCols = 32;
      bilinearInterp.numRows = 24;
      bilinearInterp.pData = temp_raw_buffer;

      // memcpy(temp_raw_buffer, Temp,768*4);

      for (uint8_t num = 0; num < 8; num++)
      {
        //插值计算
        for (uint16_t h = 0; h < 30; h++)
        {
          for (uint16_t w = 0; w < 320; w++)
          {
            temp_interp_buffer[h*320+w] = arm_bilinear_interp_f32(&bilinearInterp, (float32_t)w / 10.29032258064516f, (float32_t)(h+num*30) / 10.39130434782609f);
          }
        }
        //归一化
        for (uint16_t i = 0; i < (320*30); i++)
        {
          TemptNorma[i] = (uint8_t)((temp_interp_buffer[i] - MinTemp) / (MaxTemp - MinTemp) * 255);
          // TemptNorma[i] = (uint8_t)((temp_interp_buffer[i] - 0) / (50 - 0) * 255);
        }
        //色彩转换
        for (uint32_t i = 0; i < (320*30); i++)
        {
          GrayToPseColor(TemptNorma[i], &R,&G,&B);
          color_temp.r = R;
          color_temp.g = G;
          color_temp.b = B;
          Picter[i] = color_temp.raw;
        }
        //大小端转换
        for (uint16_t i = 0; i < (320*30); i++)
        {
          Picter[i] = (Picter[i]<<8 | Picter[i]>>8);
        }
        ST7789_DrawImage(0, num*30, 320, 30, Picter);
      }
      /********************************************************************************************************** */
      /********************************************************************************************************** *
      // float32_t temp_raw_buffer[32*6]={0};//存放原始数据,原数据上1/4
      // float32_t temp_interp_buffer[320*60]={0};//存放插值后数据
      // arm_bilinear_interp_instance_f32 bilinearInterp;
      bilinearInterp.numCols = 32;
      bilinearInterp.numRows = 3;
      bilinearInterp.pData = temp_raw_buffer;

      for (uint8_t num = 0; num < 8; num++)
      {
        // memcpy(temp_raw_buffer, &Temp[32*3*num], 32*3*sizeof(float32_t));

        // for (uint16_t h = 0; h < 30; h++)
        // {
        //   for (uint16_t w = 0; w < 320; w++)
        //   {
        //     // temp_interp_buffer[h*320+w] = arm_bilinear_interp_f32(&bilinearInterp, (float32_t)w / 10.29032258064516f, (float32_t)h / 14.5f);
        //   }
        // }
        //归一化
        // for (uint16_t i = 0; i < (320*30); i++)
        // {
        //   // TemptNorma[i] = (uint8_t)((Temp[i] - MinTemp) / (MaxTemp - MinTemp) * 255);
        //   TemptNorma[i] = (uint8_t)((temp_interp_buffer[i] - 0) / (50 - 0) * 255);
        // }
        //色彩转换
        // for (uint32_t i = 0; i < (320*30); i++)
        // {
        //   GrayToPseColor(TemptNorma[i], &R,&G,&B);
        //   color_temp.r = R;
        //   color_temp.g = G;
        //   color_temp.b = B;
        //   Picter[i] = color_temp.raw;
        // }

        // for (uint16_t i = 0; i < (320*30); i++)
        // {
        //   Picter[i] = (Picter[i]<<8 | Picter[i]>>8);
        // }
        // ST7789_DrawImage(0, num*30, 320, 30, Picter);
      }
      ********************************************************************************************************* */

      /*********************************↓色块显示，没有插值↓（基本显示功能，勿删）*************************************
      float TemptNorma[768];
      static union color color_temp[768];
      uint8_t R,G,B;
      //归一化
      for (uint16_t i = 0; i < 768; i++)
      {
        TemptNorma[i] = (uint8_t)((Temp[i] - MinTemp) / (MaxTemp - MinTemp) * 255);
        // TemptNorma[i] = (uint8_t)((Temp[i] - 0) / (50 - 0) * 255);
      }
      for (uint16_t i = 0; i < 768; i++)
      {
        GrayToPseColor(TemptNorma[i], &R,&G,&B);
        color_temp[i].r = R;
        color_temp[i].g = G;
        color_temp[i].b = B;
      }
      // char str[40];
      // sprintf(str, "MAX:%1.f,MIN:%1.f,color:%x",MaxTemp,MinTemp,color_temp[383].raw);
      // ST7789_WriteString(0,20,str,Font_11x18,WHITE,BLACK);
      // 显示
      uint16_t i=0;
      for (int16_t h = 0; h < 24; h++)
      {
        for (int16_t w = 0; w < 32; w++)
        {
          ST7789_DrawFilledRectangle(w*10, h*10,10,10,color_temp[i++].raw);
        }
      }
      **********************************↑色块显示，没有插值↑*************************************/

      /***************插值测试(勿删)(已经成功)*************************************************************

      float32_t temp_data[16]={0};
      float32_t temp_data_interp[64]={0};
      arm_bilinear_interp_instance_f32 bilinearInterp;
      bilinearInterp.numCols = 4;
      bilinearInterp.numRows = 4;
      bilinearInterp.pData = temp_data;

      for (uint16_t h = 0; h < 4; h++)
      {
        for (uint16_t w = 0; w < 4; w++)
        {
          temp_data[h*4+w] = h*4+w;
        }
      }

      for (uint16_t h = 0; h < 8; h++)
      {
        for (uint16_t w = 0; w < 8; w++)
        {
          temp_data_interp[h*8+w] = arm_bilinear_interp_f32(&bilinearInterp, (float32_t)w / 2.333f, (float32_t)h / 2.333f);//2.333f是7÷3.
        }
      }

      char str[600];
      sprintf(str,"%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t\n\n",
                  temp_data[0], temp_data[1], temp_data[2], temp_data[3],
                  temp_data[4], temp_data[5], temp_data[6], temp_data[7],
                  temp_data[8], temp_data[9], temp_data[10], temp_data[11],
                  temp_data[12], temp_data[13], temp_data[14], temp_data[15]);
      CDC_Transmit_FS((uint8_t *)str,strlen(str));

      sprintf(str,"%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n"
                  "%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t%.2f \t\n\n",
                  temp_data_interp[0],temp_data_interp[1],temp_data_interp[2],temp_data_interp[3],temp_data_interp[4],temp_data_interp[5],temp_data_interp[6],temp_data_interp[7],
                  temp_data_interp[8],temp_data_interp[9],temp_data_interp[10],temp_data_interp[11],temp_data_interp[12],temp_data_interp[13],temp_data_interp[14],temp_data_interp[15],
                  temp_data_interp[16],temp_data_interp[17],temp_data_interp[18],temp_data_interp[19],temp_data_interp[20],temp_data_interp[21],temp_data_interp[22],temp_data_interp[23],
                  temp_data_interp[24],temp_data_interp[25],temp_data_interp[26],temp_data_interp[27],temp_data_interp[28],temp_data_interp[29],temp_data_interp[30],temp_data_interp[31],
                  temp_data_interp[32],temp_data_interp[33],temp_data_interp[34],temp_data_interp[35],temp_data_interp[36],temp_data_interp[37],temp_data_interp[38],temp_data_interp[39],
                  temp_data_interp[40],temp_data_interp[41],temp_data_interp[42],temp_data_interp[43],temp_data_interp[44],temp_data_interp[45],temp_data_interp[46],temp_data_interp[47],
                  temp_data_interp[48],temp_data_interp[49],temp_data_interp[50],temp_data_interp[51],temp_data_interp[52],temp_data_interp[53],temp_data_interp[54],temp_data_interp[55],
                  temp_data_interp[56],temp_data_interp[57],temp_data_interp[58],temp_data_interp[59],temp_data_interp[60],temp_data_interp[61],temp_data_interp[62],temp_data_interp[63]);
      CDC_Transmit_FS((uint8_t *)str,strlen(str));
      ****************************************************************************************/

      /************归一化色彩梯度测试(勿删)*
      uint8_t temp[32]={0};
      uint8_t R,G,B;
      union color color_temp1[32];

      for (uint8_t i = 0; i < 32; i++)
      {
        temp[i] = (uint8_t)((float)i/31.0f*255.f);
      }
      for (uint8_t i = 0; i < 32; i++)
      {
        GrayToPseColor(temp[i], &R,&G,&B);
        color_temp1[i].r = R;
        color_temp1[i].g = G;
        color_temp1[i].b = B;
      }
      for (uint8_t i = 0; i < 32; i++)
      {
        ST7789_DrawFilledRectangle(i*10, 0,10,10,color_temp1[i].raw);
      }
      *****************************/


    }
  }
  /* USER CODE END StartReadMLX_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
 * @brief  float格式的数据转换为12.20格式的数据
 *
 * @param value
 * @return int32_t
 */
int32_t float_to_1220(float value)
{
  int32_t intValue = (int32_t)value;
  int32_t fracValue = 0;
  float fracPart = value - (float)intValue;
  if ( fracPart > 0)fracValue = (int32_t)(fracPart * 1024);
  return (int32_t)(intValue << 20) + fracValue;
}
/* USER CODE END Application */
