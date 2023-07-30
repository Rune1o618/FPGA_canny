#ifndef __DMA_H
#define __DMA_H

#include "xparameters.h"
#include "xaxidma.h"

#define DMA_DEV_ID			XPAR_AXIDMA_0_DEVICE_ID
#define DDR_BASE_ADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)
#define TX_BD_SPACE_BASE	(MEM_BASE_ADDR)
#define TX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x00000FFF)
#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00001000)
#define RX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x00001FFF)

#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)

#define MAX_PKT_LEN			0x20
#define MARK_UNCACHEABLE    0x701

#define TEST_START_VALUE	0xC

/*
 * FOR SG
 */
int RxSetup(XAxiDma * AxiDmaInstPtr);
int TxSetup(XAxiDma * AxiDmaInstPtr);
int dma_send_packet(u8* TxPacket);
int CheckData(void);
int dma_check_result();

int dma_init();

/*
 * FOR SIMPLE
 */

int dma_init_simple();
int dma_transaction_simple(u8* TxBufferPtr, u8* RxBufferPtr, u32 size);
int dma_send_simple(u8* TxBufferPtr, u32 size);
int dma_recv_simple(u8* RxBufferPtr, u32 size);


#endif
