#include "memorymap.h"

// This contains the keyboard driver
// Currently, only a single function is visible:
// void __fastcall__ readFromKeyboard(void);
// This is a blocking call, i.e. it will not return to
// the caller, until the user presses a key.
// The function will handle shifted keys.

// Keyboard scan codes
#define KEYB_INITIALIZED  0xAAU
#define KEYB_RELEASED     0xF0U
#define KEYB_EXTENDED     0xE0U
#define KEYB_SHIFT_LEFT   0x12
#define KEYB_SHIFT_RIGHT  0x59

// Mapping from normal (unshifted) scancode to ASCII
static const unsigned char normal[128] = {

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

      0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x31, 0x00,
      0x00, 0x00, 0x7A, 0x73, 0x61, 0x77, 0x32, 0x00,

      0x00, 0x63, 0x78, 0x64, 0x65, 0x34, 0x33, 0x00,
      0x00, 0x20, 0x76, 0x66, 0x74, 0x72, 0x35, 0x00,

      0x00, 0x6E, 0x62, 0x68, 0x67, 0x79, 0x36, 0x00,
      0x00, 0x00, 0x6D, 0x6A, 0x75, 0x37, 0x38, 0x00,

      0x00, 0x2C, 0x6B, 0x69, 0x6F, 0x30, 0x39, 0x00,
      0x00, 0x2E, 0x2D, 0x6C, 0xE6, 0x70, 0x2B, 0x00,

      0x00, 0x00, 0xF8, 0x00, 0xE5, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x0D, 0x7E, 0x00, 0x27, 0x00, 0x00,

      0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
      0x00, 0x03, 0x00, 0x1B, 0x02, 0x00, 0x00, 0x00,

      0x00, 0x7F, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

// Mapping from shifted scancode to ASCII
static const unsigned char shifted[256] = {

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

      0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x21, 0x00,
      0x00, 0x00, 0x5A, 0x53, 0x41, 0x57, 0x22, 0x00,

      0x00, 0x43, 0x58, 0x44, 0x45, 0x24, 0x23, 0x00,
      0x00, 0x20, 0x56, 0x46, 0x54, 0x52, 0x25, 0x00,

      0x00, 0x4E, 0x42, 0x48, 0x47, 0x59, 0x26, 0x00,
      0x00, 0x00, 0x4D, 0x4A, 0x55, 0x2F, 0x28, 0x00,

      0x00, 0x3B, 0x4B, 0x49, 0x4F, 0x3D, 0x29, 0x00,
      0x00, 0x3A, 0x5F, 0x4C, 0xC6, 0x50, 0x3F, 0x00,

      0x00, 0x00, 0xD8, 0x00, 0xC5, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x0D, 0x5E, 0x00, 0x2A, 0x00, 0x00,

      0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

static unsigned char releaseMode = 0;
static unsigned char shiftMode = 0;
static unsigned char curKey = 0;
static unsigned char skipNext = 0;

// A little state machine to interpret the scan codes received
// from the kayboard.
// Note, to optimize, I avoid using function arguments, because that leads to
// very slow code.
#if 0
unsigned char readFromKeyboard(void)
{
   static unsigned char releaseMode = 0;
   static unsigned char shiftMode = 0;

   unsigned char scanCode = *((char *) VGA_KEY);

   // Check for special scan codes
   switch (scanCode)
   {
      case 0                : return 0;
      case KEYB_INITIALIZED : return 0;
      case KEYB_RELEASED    : releaseMode = 1; return 0;
      case KEYB_EXTENDED    : return 0; // Just ignore for now
   }

   // First check if we are releasing a key
   if (releaseMode)
   {
      // Special care must be made if we're releasing a shift key.
      if (scanCode == KEYB_SHIFT_LEFT ||
          scanCode == KEYB_SHIFT_RIGHT)
      {
         shiftMode = 0;
      }

      releaseMode = 0;
      return 0;
   }

   // Check for shift key.
   if (scanCode == KEYB_SHIFT_LEFT ||
         scanCode == KEYB_SHIFT_RIGHT)
   {
      shiftMode = 1;
      return 0;
   }

   if (shiftMode)
      return shifted[scanCode];

   return normal[scanCode];
} // end of readFromKeyboard
#endif

void __fastcall__ readCurrentKey(void)
{
   __asm__("LDA %w", VGA_KEY);   // Pop data from keyboard fifo
   __asm__("BEQ %g", noChange);  // Keyboard fifo empty

   // A byte from the keyboard fifo has now been popped.

   __asm__("TAX");
   __asm__("LDA %v", skipNext);
   __asm__("BNE %g", noSkip);   // Just skip this one byte
   __asm__("TXA");

   __asm__("CMP #%b", KEYB_INITIALIZED);
   __asm__("BEQ %g", noChange);     // We ignore the initialization code.
   __asm__("CMP #%b", KEYB_EXTENDED);
   __asm__("BEQ %g", skipByte);     // We ignore extended keys too.
   __asm__("CMP #%b", KEYB_RELEASED);
   __asm__("BEQ %g", release);

   __asm__("STA %v", curKey);
   __asm__("RTS");

noSkip:
   __asm__("LDA #$00");             // Only skip this one byte
   __asm__("STA %v", skipNext);

noChange:
   __asm__("LDA %v", curKey);
   __asm__("RTS");

release:
   __asm__("LDA #$00");
   __asm__("STA %v", curKey);

skipByte:
   __asm__("LDA #$01");
   __asm__("STA %v", skipNext);  // Skip next byte from keyboard fifo
   __asm__("LDA %v", curKey);
   __asm__("RTS");
} // end of readCurrentKey

void __fastcall__ readFromKeyboard(void)
{
   // The following code works as the keyboard driver.
   // It waits for and reads scan codes from the keyboard,
   // and converts it to ASCII characters.

clearReleaseMode:
   __asm__("LDA #$00"); 
   __asm__("STA %v", releaseMode); 
   __asm__("JMP %g", wait_for_keyboard);

setReleaseMode:
   __asm__("LDA #$01"); 
   __asm__("STA %v", releaseMode); 

   // Wait for information from keyboard
wait_for_keyboard:
   __asm__("LDA %w", VGA_KEY);
   __asm__("BEQ %g", wait_for_keyboard);     // Wait until keyboard information ready
   __asm__("CMP #%b", KEYB_EXTENDED);
   __asm__("BEQ %g", wait_for_keyboard);     // So far, we just ignore the extended keys.
   __asm__("CMP #%b", KEYB_INITIALIZED);
   __asm__("BEQ %g", wait_for_keyboard);     // We ignore the initialization code too.

   // It is key press or key release?
   __asm__("CMP #%b", KEYB_RELEASED);
   __asm__("BEQ %g", setReleaseMode);        // Go back and wait for next keyboard scan code

   __asm__("AND #$7F");                      // Clear bit 7
   __asm__("TAX");
   __asm__("LDA %v", releaseMode); 
   __asm__("BEQ %g", key_pressed); 
   __asm__("TXA");

   // A key has been released
   __asm__("CMP #%b", KEYB_SHIFT_LEFT);      // Is this a shift key?
   __asm__("BEQ %g", release_shift);
   __asm__("CMP #%b", KEYB_SHIFT_RIGHT);
   __asm__("BNE %g", clearReleaseMode);      // No, just a regular key. Ignore it.

release_shift:
   __asm__("LDA #$00");
   __asm__("STA %v", shiftMode);
   __asm__("JMP %g", clearReleaseMode);

press_shift:
   __asm__("LDA #$01");
   __asm__("STA %v", shiftMode);
   __asm__("JMP %g", wait_for_keyboard);

key_pressed:
   // A key is being pressed
   __asm__("TXA");
   __asm__("CMP #%b", KEYB_SHIFT_LEFT);      // Is this a shift key
   __asm__("BEQ %g", press_shift);
   __asm__("CMP #%b", KEYB_SHIFT_RIGHT);
   __asm__("BEQ %g", press_shift);

   // Ok, a regular key has been pressed. Convert it to ASCII
   __asm__("LDA %v", shiftMode);            // Is shift currently pressed?
   __asm__("BNE %g", shifted);
   __asm__("LDA %v,X", normal);
   __asm__("JMP %g", got_key);
shifted:
   __asm__("LDA %v,X", shifted);

got_key:
   __asm__("RTS");
} // end of readFromKeyboard

