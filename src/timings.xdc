create_clock -period 6.25 -name adc_clk [get_ports jd_p[2]]

set_false_path -from [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FIFO_DUALCLOCK_MACRO_inst/genblk5_0.fifo_18_bl_1.fifo_18_bl_1/WREN]
set_false_path -from [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FIFO_DUALCLOCK_MACRO_inst/genblk5_0.fifo_18_bl_1.fifo_18_bl_1/RST]
set_false_path -from [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/data_extend_inst/data_cs_reg*/CLR]
set_false_path -from [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/data_extend_inst/state_cs_reg/CLR]


# Center-Aligned Double Data Rate Source Synchronous Inputs 
#
# For a center-aligned Source Synchronous interface, the clock
# transition is aligned with the center of the data valid window.
# The same clock edge is used for launching and capturing the
# data. The constraints below rely on the default timing
# analysis (setup = 1/2 cycle, hold = 0 cycle).
#
# input                  ____________________
# clock    _____________|                    |_____________
#                       |                    |                 
#                dv_bre | dv_are      dv_bfe | dv_afe
#               <------>|<------>    <------>|<------>
#          _    ________|________    ________|________    _
# data     _XXXX____Rise_Data____XXXX____Fall_Data____XXXX_
#

set input_clock         adc_clk;      # Name of input clock
set input_clock_period  6.25;    # Period of input clock (full-period)
set dv_bre              1.000;             # Data valid before the rising clock edge
set dv_are              1.000;             # Data valid after the rising clock edge
set dv_bfe              1.000;             # Data valid before the falling clock edge
set dv_afe              1.000;             # Data valid after the falling clock edge
set input_ports         "jd_p[1:0] jd_p[3] jc_p[3:0] jb_p[1]";       # List of input ports

# Input Delay Constraint
set_input_delay -clock $input_clock -max [expr $input_clock_period/2 - $dv_bfe] [get_ports $input_ports];
set_input_delay -clock $input_clock -min $dv_are                                [get_ports $input_ports];
set_input_delay -clock $input_clock -max [expr $input_clock_period/2 - $dv_bre] [get_ports $input_ports] -clock_fall -add_delay;
set_input_delay -clock $input_clock -min $dv_afe                                [get_ports $input_ports] -clock_fall -add_delay;

# Report Timing Template
#report_timing -rise_from [get_ports $input_ports] -max_paths 20 -nworst 2 -delay_type min_max -name src_sync_cntr_ddr_in_rise  -file src_sync_cntr_ddr_in_rise.txt;
#report_timing -fall_from [get_ports $input_ports] -max_paths 20 -nworst 2 -delay_type min_max -name src_sync_cntr_ddr_in_fall  -file src_sync_cntr_ddr_in_fall.txt;

set_property IDELAY_VALUE 15 [get_cells {design_1_i/adc16dv160_input_0/inst/gen_adc_data[0].IDELAYE2_ADC_DATA}]  
set_property IDELAY_VALUE 15 [get_cells {design_1_i/adc16dv160_input_0/inst/gen_adc_data[1].IDELAYE2_ADC_DATA}]       
        