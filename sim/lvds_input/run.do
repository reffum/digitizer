
set IP "../../ip_repo/lvds_input_1.0"
set XILINX "C:/Xilinx/Vivado/2019.2/data"
proc compilecode {} {
    global IP
    global XILINX

    vlog  ${IP}/src/lvds_input_common.sv
    vlog  ${IP}/src/lvds_input.sv
    vlog  ${IP}/src/lvds_input_axi_read.sv
    vlog  ${IP}/src/lvds_input_data_receiver.sv
    vlog  ${IP}/src/lvds_input_write.sv
    vlog  ${IP}/src/synchronizer.sv

    vlog $XILINX/verilog/src/glbl.v
    vlog axi_stream_receiver.sv
    vlog -novopt tb.sv
}

proc simulate {} {
    vsim -onfinish final \
	-voptargs="+acc=p+/UUT +acc=m+/UUT/lvds_input_write_inst" \
	-voptargs="+acc=f+/UUT/lvds_input_write_inst/state_cs"  \
	-voptargs="+acc=n+/UUT/adc_data" \
	-voptargs="+acc=n+/UUT/dsize"  \
	-voptargs="+acc=n+/UUT/cr_test"  \
	-voptargs="+acc=n+/UUT/cr_start"  \
	-voptargs="+acc=n+/UUT/sr_pc"  \
	-voptargs="+acc=n+/UUT/cr_rt"  \
	-voptargs="+acc=m+/UUT/data_receiver_inst"  \
	-voptargs="+acc=f+/UUT/data_receiver_inst/state_cs"  \
	-voptargs="+acc=n+/sync"  \
	-voptargs="+acc=n+/m00_axis_aclk"  \
	-voptargs="+acc=nfr+/UUT/level_sync_inst" \
	-L unisims_ver -L unimacro_ver tb glbl
    
    assertion fail -recursive -action break tb
    log -r /*
    add wave -group AXI /UUT/s_axi_*
    add wave -group AXIS /UUT/m00_axis_*
    add wave /m00_axis_aclk
    add wave /UUT/adc_clk
    add wave /UUT/adc_data_p /UUT/adc_data_n
    add wave /sync
    add wave /UUT/lvds_input_write_inst/state_cs
    add wave /UUT/dsize
    add wave /UUT/cr_test
    add wave /UUT/cr_start
    add wave /UUT/sr_pc
    add wave /UUT/data_receiver_inst/state_cs

    config wave -signalnamewidth 1
    
    run -all

    wave zoom full
}

alias c "compilecode"
alias s "simulate"
