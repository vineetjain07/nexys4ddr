# Design Your Own Computer - Episode 4 : "Adding hexadecimal output to VGA"

Welcome to the fourth episode of "Design Your Own Computer". In this
episode we will be accomplishing several tasks:
* Adding a complete ASCII font to the system.
* Changing the VGA output to show data using hexadecimal digits.

## Adding a complete ASCII font
The font is taken from <https://github.com/dhepper/font8x8>.
In order for the synthesis tool to be able to correctly interpret
the contents of the file, all the formatting has been manually removed
from the file.
Essentially, you need to have each array element on a separate line.
The contents of the file are stored inside the bit file generated by
the synthesis tool.

To keep things separate, a new file vga/font.vhd is added.  The command hread()
in line 34 of vga/font.vbhd reads an entire line of hexadecimal digits.

The file name containing the font is passed as a generic in line 9 of
vga/font.vhd.

Notice that line 48 in vga/font.vhd (reading the font data) is similar to line
60 in mem/mem.vhd (reading from memory). This is no coincidence, and by looking
at the interface of the font block, the character input can be regarded as a
memory address, and the bitmap output can be regarded as the memory data.  In a
later episode we'll make use of this similarity and implement the font data
into a RAM that the CPU can read from and write to. This will enable the program
running on the CPU to update the character font.

## Showing hexadecimal digits.

Lines 86-88 in vga/digits.vhd are changed to read four bits at a time from the
input data. This is because a single hexadecimal digit consists of four bits.

The ASCII code is calculated in lines 95-96.

Line 117 has been changed, because of the way the font is stored in the text
file.  In the previous episode, the MSB of the font data corresponded to the
left most pixel.  But in the font data copied from the above web page, the MSB
is the right most pixel.  To be consistent, the font data should be changed
(and that can easily be done by a small separate program), but I decided this
extra processing was annoying.

Line 136 has been changed, to reflect that there are now only six characters
displayed on the screen.

And that is it! We are now in a position where we can display data on the screen
in the form of hexadecimal digits. We are therefore ready to begin the first
steps in designing the CPU.

## Learnings:
Initializing memory directly from a separate text file.
