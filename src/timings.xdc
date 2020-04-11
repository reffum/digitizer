create_clock -period 6.250 -name <adc_clk> [get_ports hdmi_clk_p]

set_false_path -from [get_pins {design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C}] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FIFO_DUALCLOCK_MACRO_inst/genblk5_0.fifo_18_bl_1.fifo_18_bl_1/WREN]
set_false_path -from [get_pins {design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FSM_sequential_state_cs_reg*/C}] -to [get_pins design_1_i/adc16dv160_input_0/inst/data_receiver_inst/FIFO_DUALCLOCK_MACRO_inst/genblk5_0.fifo_18_bl_1.fifo_18_bl_1/RST]
