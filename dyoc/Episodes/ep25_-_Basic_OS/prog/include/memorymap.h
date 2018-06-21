#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_

#include <stdint.h>

#define MEM_RAM  ((uint8_t *) 0x0000)
#define MEM_CHAR ((uint8_t *) 0x8000)
#define MEM_COL  ((uint8_t *) 0xA000)
#define MEM_ROM  ((uint8_t *) 0xC000)

#define SIZE_RAM  (0x8000)
#define SIZE_CHAR (0x2000)
#define SIZE_COL  (0x2000)
#define SIZE_ROM  (0x4000)

// Memory mapped IO
typedef struct
{
   uint8_t  vgaPalette[16];
   uint16_t vgaPixYInt;
   uint8_t  cpuCycLatch;
   uint8_t  _reserved[12];
   uint8_t  irqMask;
} t_memio_config;

typedef struct
{
   uint16_t vgaPixX;
   uint16_t vgaPixY;
   uint32_t cpuCyc;
   uint8_t  kbdData;
   uint8_t  _reserved[22];
   uint8_t  irqStatus;
} t_memio_status;

#define MEMIO_CONFIG ((t_memio_config *) 0x7FC0)
#define MEMIO_STATUS ((t_memio_status *) 0x7FE0)

#define IRQ_TIMER_NUM    0
#define IRQ_VGA_NUM      1
#define IRQ_KBD_NUM      2

#endif // _MEMORY_MAP_H_
