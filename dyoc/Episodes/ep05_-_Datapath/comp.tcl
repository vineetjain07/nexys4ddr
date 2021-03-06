# This is a tcl command script for the Vivado tool chain
read_vhdl -vhdl2008 { \
   comp.vhd waiter.vhd clk.vhd cdc.vhd \
   vga/vga.vhd vga/digits.vhd vga/pix.vhd vga/font.vhd \
   main/main.vhd \
   main/cpu/cpu.vhd main/cpu/datapath.vhd main/cpu/ctl.vhd \
   main/mem/ram.vhd \
}
read_xdc comp.xdc
set_property XPM_LIBRARIES {XPM_FIFO} [current_project]
synth_design -top comp -part xc7a100tcsg324-1 -flatten_hierarchy none
place_design
route_design
write_checkpoint -force comp.dcp
write_bitstream -force comp.bit
report_methodology
report_timing_summary -file timing_summary.rpt
exit
