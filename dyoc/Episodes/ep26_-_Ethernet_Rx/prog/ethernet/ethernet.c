#include <stdint.h>
#include <stdlib.h>
#include <conio.h>
#include "memorymap.h"

// This is the number of bytes to use for the entire receive buffer. It should be
// large enough to contain at least one complete frame, i.e. 1500 bytes.
#define BUF_SIZE 2000

// This macro reads a single byte from the buffer, while automatically
// taking care of wrap around.
// The end of the buffer must be in the variable 'pBufEnd', and
// the size of the buffer must be in 'BUF_SIZE'.
#define RD_BUF(ptr) ((ptr) < pBufEnd ? *(ptr) : *((ptr)-BUF_SIZE))

// Write a single byte in hexadecimal.
void putx8(uint8_t x)
{
   static const char hex[16] = "0123456789ABCDEF";
   cputc(hex[(x>>4) & 0x0F]);
   cputc(hex[(x>>0) & 0x0F]);
}

// Write a 16-bit value in hexadecimal.
void putx16(uint16_t x)
{
   putx8((x>>8) & 0xFF);
   putx8((x>>0) & 0xFF);
}

void main(void)
{
   // This variable is only used during simulation, to test the arbitration
   // between CPU and Ethernet while writing to memory.
   uint8_t dummy_counter = 0;

   // Allocate receive buffer. This will never be free'd.
   // Using malloc (rather than a globally allocated array) avoids a call to
   // memset generated by the compiler, thereby reducing simulation time.
   uint8_t *pBuf = (uint8_t *) malloc(BUF_SIZE);

   // First byte after the buffer. This is used in the macro RD_BUF().
   uint8_t *pBufEnd = pBuf + BUF_SIZE;

   // Current read pointer of the CPU
   uint8_t *rdPtr = pBuf;

   // Configure Ethernet DMA
   MEMIO_CONFIG->ethEnable = 0;  // DMA must be disabled during configuration.
   MEMIO_CONFIG->ethStart  = (uint16_t) pBuf;
   MEMIO_CONFIG->ethEnd    = (uint16_t) pBufEnd;
   MEMIO_CONFIG->ethRdPtr  = (uint16_t) rdPtr;
   MEMIO_CONFIG->ethEnable = 1;  // Enabling the DMA will automatically reset the ethWrPtr.
 
   // Wait for data to be received, and print to the screen
   while (1)
   {
      uint8_t *wrPtr;
      uint8_t  i;

      dummy_counter += 1;   // This generates a write to the main memory.
      wrPtr = (uint8_t *)(MEMIO_STATUS->ethWrPtr); // Check if data is present in buffer.
      if (rdPtr == wrPtr)
         continue;   // No? Then go back and wait for data

      // Show the pointer locations of the received Ethernet frame.
      putx16((uint16_t)rdPtr);
      cputs("-");
      putx16((uint16_t)wrPtr);
      cputs(":");

      // Show the first 16 bytes of the received Ethernet frame. This includes the
      // two byte length field, and the 14 bytes MAC header.
      for (i=0; i<16; ++i)
      {
         putx8(RD_BUF(rdPtr+i));
      }
      cputs("\n\r");

      // Instruct DMA that CPU is finished with this frame.
      rdPtr = wrPtr;
      MEMIO_CONFIG->ethRdPtr = rdPtr;
   }

} // end of main
