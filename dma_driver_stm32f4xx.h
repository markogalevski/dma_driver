#ifndef _DMA_H
#define _DMA_H

#include "stm32f411xe.h"

#define EN_SET 1
#define EN_RESET 0

// DMA SxCR defines
#define FLOW_DMA 0
#define FLOW_PERIPH 1

#define DMA_DIR_P2M 0
#define DMA_DIR_M2P 1
#define DMA_DIR_M2M 2

#define PAR_FIXED 0
#define PAR_INC 1

#define MAR_FIXED 0
#define MAR_INC 1

#define PSIZE_BYTE 0
#define PSIZE_HWORD 1
#define PSIZE_WORD 2

#define MSIZE_BYTE 0
#define MSIZE_HWORD 1
#define MSIZE_WORD 2

#define PAR_INC_OFFSET_PSIZE 0
#define PAR_INC_OFFSET_WORD 1

#define DMA_STREAM_PRIORITY_LOW 0
#define DMA_STREAM_PRIORITY_MEDIUM 1
#define DMA_STREAM_PRIORIITY_HIGH 2
#define DMA_STREAM_PRIORITY_VERY_HIGH 3

#define DBUFFER_TARGET_MEM0 0
#define DBUFFER_TARGET_MEM1 1

#define DMA_BURST_SINGLE 0
#define DMA_BURST_INCR4 1
#define DMA_BURST_INCR8 2
#define DMA_BURST_INCR16 3

#define DMA_CH_0 0
#define DMA_CH_1 1
#define DMA_CH_2 2
#define DMA_CH_3 3
#define DMA_CH_4 4
#define DMA_CH_5 5
#define DMA_CH_6 6
#define DMA_CH_7 7

//DMA FIFO CR Defines
#define FIFO_THRESH_QUARTER 0
#define FIFO_THRESH_HALF 1
#define FIFO_THRESH_THREE_QUARTERS 2
#define FIFO_THRESH_FULL 3

#define FIFO_DIRECT_ENABLE 0
#define FIFO_DIRECT_DISABLE 1

typedef struct
{
  uint32_t peripheral_flow_ctrl;
  uint32_t transfer_dir;
  uint32_t circ_mode_en;
  uint32_t peripheral_increment_mode;
  uint32_t memory_increment_mode;
  uint32_t peripheral_data_size;
  uint32_t memory_data_size;
  uint32_t peripheral_increment_offset;
  uint32_t stream_priority_level;
  uint32_t double_buffer_en;
  uint32_t current_target; //Double Buffer On only
  uint32_t peripheral_burst_transfer;
  uint32_t memory_burst_transfer;
  uint32_t channel_select;
} dma_init_t;

typedef struct
{
  uint32_t fifo_threshold;
  uint32_t direct_mode_disable;
} dma_fifo_t;

typedef struct
{
  DMA_TypeDef *controller;
  DMA_Stream_TypeDef *stream;
  dma_init_t init;
  dma_fifo_t fifo_config;
  uint32_t *p_memory0; //memory 0 is source in M2M transers
  uint32_t *p_memory1; //memory 1 is destination in M2M transfers
  uint32_t *p_periph;
  uint16_t data_length;
} dma_handle_t;

typedef enum
{
  direct_mode_error = 1,
  transfer_error = 2,
  half_transfer = 3,
  transfer_complete = 4,
  fifo_error = 7
}dma_it_t;

void dma_transfer(dma_handle_t *hdma);
void dma_disable(dma_handle_t *hdma);
void dma_enable_interrupt(dma_handle_t *hdma, dma_it_t interrupt);
void dma_disable_interrupt(dma_handle_t *hdma, dma_it_t interrupt);
#endif /* DMA_DRIVER */
