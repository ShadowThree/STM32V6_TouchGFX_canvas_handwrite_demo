/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LCD_BL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = TP_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(TP_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 12, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */
#include "dbger.h"
#include "gpio.h"
#include "gt911.h"
#include "atk_ncr.h"
#define LINE_WIDTH	2
extern osMessageQueueId_t coordinateHandle;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static COORDINATE_t last_pos[NUM_TOUCH_SUPPORT] = {0};
	static uint32_t last_tick[NUM_TOUCH_SUPPORT] = {0};
	osStatus_t osSta;

	if(GPIO_Pin == TP_INT_Pin) {
		gt911_get_touch((COORDINATE_t*)touch_coordinate, &num_touched);
		if(num_touched) {
			//INT_DBG(" detect %d touch(s):\n", num_touched);
			for(uint8_t i = 0; i < num_touched; i++) {
				//LOG_DBG("\tP%d(%d, %d)\n", i, touch_coordinate[i].x, touch_coordinate[i].y);
				
				#if 1		// process the handwrite event
				if(touch_coordinate[i].x > 100 + LINE_WIDTH && touch_coordinate[i].x < 400 - LINE_WIDTH &&
					 touch_coordinate[i].y > 100 + LINE_WIDTH && touch_coordinate[i].y < 400 - LINE_WIDTH &&
					 last_pos[i].x > 100 + LINE_WIDTH && last_pos[i].x < 400 - LINE_WIDTH &&
					 last_pos[i].y > 100 + LINE_WIDTH && last_pos[i].y < 400 - LINE_WIDTH)
				{
					if(HAL_GetTick() - last_tick[i] > 20) {
						last_tick[i] = HAL_GetTick();
						last_pos[i].x = touch_coordinate[i].x;
						last_pos[i].y = touch_coordinate[i].y;
						continue;
					}
					last_tick[i] = HAL_GetTick();
					if(last_pos[i].x != touch_coordinate[i].x || last_pos[i].y != touch_coordinate[i].y) {
						#if 0		// draw point
						for(int16_t m = touch_coordinate[i].x - LINE_WIDTH; m <= touch_coordinate[i].x + LINE_WIDTH; m++) {
							for(int16_t n = touch_coordinate[i].y - LINE_WIDTH; n <= touch_coordinate[i].y + LINE_WIDTH; n++) {
								addr = (LTDC_L1_ADDR + (m - LTDC_L1_START_X) * 2 + (n - LTDC_L1_START_Y) * LTDC_L1_WIDTH * 2);
								INT_DBG("P%d(%d, %d) 0x%08x\n", i, touch_coordinate[i].x, touch_coordinate[i].y, addr);
								if(addr >= LTDC_L1_ADDR && addr < LTDC_L1_ADDR + LTDC_L1_WIDTH * LTDC_L1_HEIGHT * 2) {
									*(uint16_t*)addr = 0;
								}
							}
						}
						#else		// draw line
						LOG_DBG("A(%d, %d) --> A(%d, %d)\n", last_pos[i].x, last_pos[i].y, touch_coordinate[i].x, touch_coordinate[i].y);
						//LTDC_draw_line(last_pos[i].x, last_pos[i].y, touch_coordinate[i].x, touch_coordinate[i].y);
						COORDINATE_t coordinate[2] = {{.x = last_pos[i].x, .y = last_pos[i].y}, {.x = touch_coordinate[i].x, .y = touch_coordinate[i].y}};
						if(point_num == 0) {
							draw_coor[0].x = last_pos[i].x;
							draw_coor[0].y = last_pos[i].y;
							draw_coor[1].x = touch_coordinate[i].x;
							draw_coor[1].y = touch_coordinate[i].y;
							point_num = 2;
						} else {
							draw_coor[point_num].x = touch_coordinate[i].x;
							draw_coor[point_num++].y = touch_coordinate[i].y;
						}
						osSta = osMessageQueuePut(coordinateHandle, coordinate, NULL, 0);
						if(osSta != osOK) {
							LOG_ERR("queue put err[%d]\n", osSta);
						}
						#endif
					}
				}
				last_pos[i].x = touch_coordinate[i].x;
				last_pos[i].y = touch_coordinate[i].y;
				#endif
				
			}
		}
	}
}
/* USER CODE END 2 */
