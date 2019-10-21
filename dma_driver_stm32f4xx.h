#ifndef _DMA_H
#define _DMA_H

#include "stm32f411xe.h"

#define DMA_DIR_P2M 0
#define DMA_DIR_M2P 1
#define DMA_DIR_M2M 2

typedef struct
{
  uint32_t : 1;
  uint32_t direct_mode_err_IT_en : 1;
  uint32_t transfer_err_IT_en : 1;
  uint32_t half_transer_err_IT_en : 1;
  uint32_t transfer_complete_IT_en : 1;
  uint32_t peripheral_flow_ctrl : 2;
  uint32_t transfer_dir : 1;
  uint32_t circular_mode : 1;
  uint32_t peripheral_increment_mode : 1;
  uint32_t memory_increment_mode : 1;
  uint32_t peripheral_data_size : 2;
  uint32_t memory_data_size : 2;
  uint32_t peripheral_increment_size : 1;
  uint32_t stream_priority_level : 2;
  uint32_t double_buffer_mode : 1;
  uint32_t current_target : 1; //Double Buffer On only
  uint32_t : 1; //kept at reserved value
  uint32_t peripheral_burst_transfer : 2;
  uint32_t memory_burst_transfer : 2;
  uint32_t channel_select : 3;
  uint32_t : 0; //pad to nearest byte
} dma_init_t;

typedef struct
{
  uint32_t fifo_threshold : 2;
  uint32_t direct_mode_disable : 1;
  uint32_t : 4;
  uint32_t fifo_err_IT_en : 1;
  uint32_t : 24;
} dma_fifo_t;

typedef struct
{
  DMA_TypeDef *controller;
  DMA_Stream_TypeDef *stream;
  dma_init_t init;
  dma_fifo_t fifo_config;
  uint32_t state;
  uint32_t *p_memory0; //memory 0 is source in M2M transers
  uint32_t *p_memory1; //memory 1 is destination in M2M transfers
  uint32_t *p_periph;
  uint32_t data_length;
} dma_handle_t;

void dma_transfer(dma_handle_t *hdma);
void dma_disable(dma_handle_t *hdma);

#endif /* DMA_DRIVER */
