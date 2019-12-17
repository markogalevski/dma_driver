#include <dma_interface.h>
#include <stdlib.h>
static void dma_clear_interrupt_flags(dma_handle_t *hdma);
static void dma_config_endpoints(dma_handle_t *hdma);
static void dma_enable(dma_handle_t *hdma);

void dma_init(dma_handle_t *hdma)
{

  // Init procedure specified in 9.3.17 of the RM
  dma_disable(hdma);
  if (hdma->controller == DMA1)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
  }
  else if (hdma->controller == DMA2)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
  }

  hdma->stream->CR &= ~(DMA_SxCR_CHSEL_Msk);
  hdma->stream->CR |= hdma->init.channel_select << DMA_SxCR_CHSEL_Pos;

  hdma->stream->CR &= ~(DMA_SxCR_PFCTRL_Msk);
  hdma->stream->CR |= hdma->init.peripheral_flow_ctrl << DMA_SxCR_PFCTRL_Pos;

  hdma->stream->CR &= ~(DMA_SxCR_PL_Msk);
  hdma->stream->CR |= hdma->init.stream_priority_level << DMA_SxCR_PL_Pos;

  hdma->stream->FCR &= ~(DMA_SxFCR_DMDIS_Msk | DMA_SxFCR_FTH_Msk);
  hdma->stream->FCR |= hdma->fifo_config.direct_mode_disable << DMA_SxFCR_DMDIS_Pos
                      | (hdma->fifo_config.fifo_threshold << DMA_SxFCR_FTH_Pos);

  hdma->stream->CR &= ~(DMA_SxCR_DIR_Msk
                         | DMA_SxCR_PINC_Msk
                         | DMA_SxCR_PINCOS_Msk
                         | DMA_SxCR_MINC_Msk
                         | DMA_SxCR_PBURST_Msk
                         | DMA_SxCR_MBURST_Msk
                         | DMA_SxCR_CIRC_Msk
                         | DMA_SxCR_DBM_Msk);

  hdma->stream->CR |= hdma->init.transfer_dir << DMA_SxCR_DIR_Pos
                     | hdma->init.peripheral_increment_mode << DMA_SxCR_PINC_Pos
                     | hdma->init.peripheral_increment_offset << DMA_SxCR_PINCOS_Pos
                     | hdma->init.memory_increment_mode << DMA_SxCR_MINC_Pos
                     | hdma->init.peripheral_burst_transfer << DMA_SxCR_PBURST_Pos
                     | hdma->init.memory_burst_transfer << DMA_SxCR_MBURST_Pos
                     | hdma->init.circ_mode_en << DMA_SxCR_CIRC_Pos
                     | hdma->init.double_buffer_en << DMA_SxCR_DBM_Pos;

  dma_clear_interrupt_flags(hdma);
}

void dma_transfer(dma_handle_t *hdma, uint32_t *dst, uint32_t *src, uint32_t data_length)
{
	uint32_t direction_state = hdma->stream->CR & DMA_SxCR_DIR_Msk;
	  if (direction_state == DMA_DIR_P2M)
		{
		  hdma->p_periph = src;
		  hdma->p_memory0 = dst;
		  hdma->p_memory1 = NULL;

		}
	  else if (direction_state == DMA_DIR_M2P)
	  {
		  hdma->p_memory0 = src;
		  hdma->p_periph = dst;
		  hdma->p_memory1 = NULL;
	  }
	  else
	  {
		  hdma->p_memory0 = src;
		  hdma->p_memory1 = dst;
		  hdma->p_periph = NULL;
	  }
	  hdma->data_length = data_length;

	  dma_config_endpoints(hdma);
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
      dma_flag_pointer = (uint32_t *) hdma->controller->LIFCR;
    }
    else
    {
      dma_flag_pointer = (uint32_t *) hdma->controller->HIFCR;
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
      hdma->stream->PAR = (uint32_t) hdma->p_periph;
    }
    if (hdma->p_memory0 != NULL)
    {
      hdma->stream->M0AR = (uint32_t) hdma->p_memory0;
    }
    if (hdma->p_memory1 != NULL)
    {
      hdma->stream->M1AR = (uint32_t) hdma->p_memory1;
    }
    hdma->stream->NDTR = (uint32_t) hdma->data_length;
}

static void dma_enable(dma_handle_t *hdma)
{
  hdma->stream->CR |= (DMA_SxCR_EN);
}

void dma_enable_interrupt(dma_handle_t *hdma, dma_interrupt_t interrupt)
{
  if (interrupt != fifo_error)
  {
    hdma->stream->CR |= 0x01 << interrupt;
  }
  else
  {
    hdma->stream->FCR |= (0x01 << interrupt);
  }
}

void dma_disable_interrupt(dma_handle_t *hdma, dma_interrupt_t interrupt)
{
  if (interrupt != fifo_error)
  {
    hdma->stream->CR &= ~(0x01 << interrupt);
  }
  else
  {
    hdma->stream->FCR &= ~(0x01 << interrupt);
  }
}
