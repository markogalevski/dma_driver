#include "dma_driver_stm32f4xx.h"

static void dma_clear_interrupt_flags(dma_handle_t *hdma);
static void dma_config_endpoints(dma_handle_t *hdma);
static void dma_enable(dma_handle_t *hdma);

void dma_transfer(dma_handle_t *hdma)
{
  // Init procedure specified in 9.3.17 of the RM
  dma_disable(hdma);
  dma_config_endpoints(hdma);
  hdma->stream->CR |= hdma->init->channel_select;
  hdma->stream->CR |= hdma->init->peripheral_flow_ctrl;
  hdma->stream->CR |= hdma->init->stream_priority_level;
  hdma->stream->FCR |= hdma->fifo_config->direct_mode_disable
                      | hdma->fifo_config->fifo_threshold;
  hdma->stream->CR |= hdma->init->transfer_dir
                     | hdma->init->peripheral_increment_mode
                     | hdma->init->peripheral_increment_size
                     | hdma->init->memory_increment_mode
                     | hdma->init->peripheral_burst_transfer
                     | hdma->init->memory_burst_transfer
                     | hdma->init->circular_mode
                     | hdma->init->double_buffer_mode;

  dma_clear_interrupt_flags(hdma);
  dma_enable(hdma);
}

void dma_disable(dma_handle_t *hdma)
{
  hdma->stream->CR &= ~(DMA_SxCR_EN_Msk);
  while(hdma->stream->CR & DMA_SxCR_EN_Msk); //wait for dma to be disabled
}

static void dma_clear_interrupt_flags(dma_handle_t *hdma)
{
    uint32_t stream_address = *((uint32_t *) &hdma->stream);
    uint32_t controller_address = *((uint32_t *) &hdma->controller);
    uint32_t stream_offset = stream_address - controller_address;
    uint32_t *dma_flag_pointer;
    if (stream_offset <= 0x058UL)
    {
      dma_flag_pointer = hdma->controller->LIFCR;
    }
    else
    {
      dma_flag_pointer = hdma->controller->HIFCR;
    }
    uint32_t stream_base = controller_address + 0x010UL;
    uint32_t stream_id = (stream_address - (stream_base))/(0x018UL);
    uint32_t mask = 0x3DUL; // 0b0000 0011 1101
    if (stream_id == 0)
    {
      *dma_flag_pointer |= (mask);
    }
    else if (stream_id == 1)
    {
      *dma_flag_pointer |= (mask << 6);
    }
    else if (stream_id == 2)
    {
      *dma_flag_pointer |= (mask << 16);
    }
    else if (stream_id == 3)
    {
      *dma_flag_pointer |= (mask << 22);
    }
}

static void dma_config_endpoints(dma_handle_t *hdma)
{
    if (hdma->p_periph != NULL)
    {
      hdma->stream->PAR = hdma->p_periph;
    }
    if (hdma->stream->p_memory0 != NULL)
    {
      hdma->stream->M0AR = hdma->p_memory0;
    }
    if (hdma->stream->p_memory1 != NULL)
    {
      hdma->stream->M1AR = hdma->p_memory1;
    }
    hdma->stream->NDTR = (uint32_t) hdma->data_length;
}

static void dma_enable(dma_handle_t *hdma)
{
  hdma->stream->CR |= (DMA_SxCR_EN);
}
